use num_traits::{NumCast, ToPrimitive};

use crate::Error;

/// Major Type
/// https://datatracker.ietf.org/doc/html/rfc8949#name-major-types
#[repr(u8)]
#[derive(Debug, Copy, Clone, PartialEq, Eq)]
pub enum Major {
    Positive = 0,
    Negative = 1,
    Bytes = 2,
    Text = 3,
    Array = 4,
    Map = 5,
    Tag = 6,
    Other = 7,
}

impl Major {
    #[inline(always)] // Compiles into a simple bit shift in asm
    pub const fn from_byte(b: u8) -> Self {
        match b >> 5 {
            0 => Self::Positive,
            1 => Self::Negative,
            2 => Self::Bytes,
            3 => Self::Text,
            4 => Self::Array,
            5 => Self::Map,
            6 => Self::Tag,
            7 => Self::Other,
            _ => unreachable!(),
        }
    }

    #[inline(always)] // Compiles into a simple bit shift in asm
    pub(crate) const fn as_byte(self) -> u8 {
        (self as u8) << 5
    }
}

#[derive(Debug, Copy, Clone, PartialEq, Eq)]
#[repr(u8)]
pub enum Arg {
    First(u8) = 0,
    Next1([u8; 1]) = 24,
    Next2([u8; 2]) = 25,
    Next4([u8; 4]) = 26,
    Next8([u8; 8]) = 27,
    Indef = 31,
}

fn read_array<const N: usize>(s: &[u8]) -> Result<[u8; N], Error> {
    let mut array: [u8; N] = [0; N];
    let s = s.get(1..=N).ok_or(Error::Eof)?;

    array.copy_from_slice(s);
    Ok(array)
}

impl Arg {
    pub fn from_slice(s: &[u8]) -> Result<Self, Error> {
        Self::from_partial(PartialArg::from_byte(*s.first().ok_or(Error::Eof)?)?, s)
    }

    pub fn from_partial(partial: PartialArg, s: &[u8]) -> Result<Self, Error> {
        Ok(match partial {
            PartialArg::First(v) => Self::First(v),
            PartialArg::Next1 => Self::Next1(read_array(s)?),
            PartialArg::Next2 => Self::Next2(read_array(s)?),
            PartialArg::Next4 => Self::Next4(read_array(s)?),
            PartialArg::Next8 => Self::Next8(read_array(s)?),
            PartialArg::Indef => Self::Indef,
        })
    }

    #[inline]
    pub fn mask(&self) -> u8 {
        PartialArg::from(*self).as_byte()
    }

    pub const fn as_slice(&self) -> &[u8] {
        match self {
            Self::First(..) | Self::Indef => &[],
            Self::Next1(v) => v.as_slice(),
            Self::Next2(v) => v.as_slice(),
            Self::Next4(v) => v.as_slice(),
            Self::Next8(v) => v.as_slice(),
        }
    }

    pub fn as_integer<T: NumCast>(&self) -> Result<T, Error> {
        match *self {
            Self::First(b) => T::from(b),
            Self::Next1(v) => T::from(u8::from_be_bytes(v)),
            Self::Next2(v) => T::from(u16::from_be_bytes(v)),
            Self::Next4(v) => T::from(u32::from_be_bytes(v)),
            Self::Next8(v) => T::from(u64::from_be_bytes(v)),
            Self::Indef => return Err(Error::ItemIllFormed),
        }
        .ok_or(Error::NumberOutOfRange)
    }

    pub fn from_integer<T: ToPrimitive + Copy>(v: T) -> Self {
        if let Some(v) = v.to_u8() {
            if v <= 23 {
                Self::First(v)
            } else {
                Self::Next1(v.to_be_bytes())
            }
        } else if let Some(v) = v.to_u16() {
            Self::Next2(v.to_be_bytes())
        } else if let Some(v) = v.to_u32() {
            Self::Next4(v.to_be_bytes())
        } else if let Some(v) = v.to_u64() {
            Self::Next8(v.to_be_bytes())
        } else {
            unimplemented!()
        }
    }

    pub fn sequence_length(&self) -> Result<Option<usize>, Error> {
        match *self {
            Self::Indef => Ok(None),
            arg => arg.as_integer().map(Some),
        }
    }

    pub fn len(&self) -> usize {
        PartialArg::from(*self).len()
    }
}

/// Fully parsed header of a data item, consisting of a major type and an argument.
#[derive(Debug, Copy, Clone, PartialEq, Eq)]
pub struct Header(pub Major, pub Arg);

impl Header {
    pub const BREAK: Self = Self(Major::Other, Arg::Indef);

    pub fn from_slice(s: &[u8]) -> Result<Self, Error> {
        Ok(Self(
            Major::from_byte(*s.first().ok_or(Error::Eof)?),
            Arg::from_slice(s)?,
        ))
    }

    pub fn from_partial(partial: PartialHeader, s: &[u8]) -> Result<Self, Error> {
        Ok(Self(partial.0, Arg::from_partial(partial.1, s)?))
    }

    pub fn as_vec(&self) -> heapless::Vec<u8, 9> {
        let mut res = heapless::Vec::new();
        res.resize_default(9).unwrap();

        let s = self.1.as_slice();

        res[0] = PartialHeader::from(*self).as_byte();
        res[1..=s.len()].copy_from_slice(s);
        res.truncate(1 + s.len());

        res
    }

    #[inline]
    pub fn len(&self) -> usize {
        1 + self.1.len()
    }
}

#[derive(Debug, Copy, Clone, PartialEq, Eq)]
#[repr(u8)]
pub enum PartialArg {
    First(u8) = 0,
    Next1 = 24,
    Next2 = 25,
    Next4 = 26,
    Next8 = 27,
    Indef = 31,
}

impl PartialArg {
    pub const fn from_byte(b: u8) -> Result<Self, Error> {
        match b & 0b0001_1111 {
            b @ 0..=23 => Ok(Self::First(b)),
            24 => Ok(Self::Next1),
            25 => Ok(Self::Next2),
            26 => Ok(Self::Next4),
            27 => Ok(Self::Next8),
            28..=30 => Err(Error::ItemIllFormed),
            31 => Ok(Self::Indef),
            _ => unreachable!(),
        }
    }

    #[inline]
    pub const fn as_byte(self) -> u8 {
        match self {
            Self::First(b) => b,
            Self::Next1 => 24,
            Self::Next2 => 25,
            Self::Next4 => 26,
            Self::Next8 => 27,
            Self::Indef => 31,
        }
    }

    #[inline]
    pub const fn len(self) -> usize {
        match self {
            Self::Next1 => 1,
            Self::Next2 => 2,
            Self::Next4 => 4,
            Self::Next8 => 8,
            _ => 0,
        }
    }
}

impl From<Arg> for PartialArg {
    #[inline]
    fn from(value: Arg) -> Self {
        match value {
            Arg::First(b) => Self::First(b),
            Arg::Next1(_) => Self::Next1,
            Arg::Next2(_) => Self::Next2,
            Arg::Next4(_) => Self::Next4,
            Arg::Next8(_) => Self::Next8,
            Arg::Indef => Self::Indef,
        }
    }
}

/// Same as [Header], but can be parsed from a single byte.
#[derive(Debug, Copy, Clone, PartialEq, Eq)]
pub struct PartialHeader(pub Major, pub PartialArg);

impl PartialHeader {
    pub fn from_byte(b: u8) -> Result<Self, Error> {
        Ok(Self(Major::from_byte(b), PartialArg::from_byte(b)?))
    }

    #[inline]
    pub const fn as_byte(self) -> u8 {
        self.0.as_byte() & self.1.as_byte()
    }

    #[inline]
    pub const fn len(self) -> usize {
        1 + self.1.len()
    }
}

impl From<Header> for PartialHeader {
    #[inline(always)]
    fn from(value: Header) -> Self {
        Self(value.0, PartialArg::from(value.1))
    }
}
