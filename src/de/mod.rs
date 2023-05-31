use half::f16;
use num_traits::{Num, NumCast};
use serde::de::{
    self, DeserializeSeed, Deserializer as _, EnumAccess, MapAccess, SeqAccess, VariantAccess,
    Visitor,
};

use crate::de::byteseq::ByteSeqAccess;
use crate::de::parser::Parser;
use crate::header::{Arg, Header, Major};
#[cfg(feature = "std")]
use crate::io::Reader;
use crate::io::{Read, Reference, SliceReader};
use crate::scratch::Scratch;
use crate::simple::Simple;
use crate::{simple, Error};

mod byteseq;
mod parser;

#[derive(Default, Debug)]
#[non_exhaustive]
pub struct DeserializerConfig {}

pub struct Deserializer<R, S> {
    parser: Parser<R>,
    scratch: S,

    #[allow(dead_code)]
    config: DeserializerConfig,
}

impl<'a, S> Deserializer<SliceReader<'a>, S> {
    pub fn from_slice(input: &'a [u8], scratch: S, config: DeserializerConfig) -> Self {
        Self {
            parser: Parser::new(SliceReader::new(input)),
            scratch,

            config,
        }
    }
}

#[cfg(feature = "std")]
impl<S, R: std::io::Read> Deserializer<Reader<R>, S> {
    pub fn from_reader(input: R, scratch: S, config: DeserializerConfig) -> Self {
        Self {
            parser: Parser::new(Reader::new(input)),
            scratch,

            config,
        }
    }
}

impl<'de, R: Read<'de>, S: Scratch> Deserializer<R, S> {
    fn parse_number<T: NumCast + Num>(&mut self) -> Result<T, Error> {
        let Header(major, arg) = self.parser.read_header()?;

        match (major, arg) {
            (Major::Positive, arg) => arg.as_integer(),
            (Major::Negative, arg) => {
                Ok(T::from(-1).ok_or(Error::NumberOutOfRange)? - arg.as_integer::<T>()?)
            }
            (Major::Other, Arg::Next2(v)) => {
                T::from(f16::from_be_bytes(v)).ok_or(Error::NumberOutOfRange)
            }
            (Major::Other, Arg::Next4(v)) => {
                T::from(f32::from_be_bytes(v)).ok_or(Error::NumberOutOfRange)
            }
            (Major::Other, Arg::Next8(v)) => {
                T::from(f64::from_be_bytes(v)).ok_or(Error::NumberOutOfRange)
            }
            _ => Err(Error::ExpectedNumber),
        }
    }

    fn deserialize_simple<V>(&mut self, visitor: V) -> Result<V::Value, Error>
    where
        V: Visitor<'de>,
    {
        match Simple::try_from(self.parser.read_header()?)? {
            simple::FALSE => visitor.visit_bool(false),
            simple::TRUE => visitor.visit_bool(true),
            simple::UNDEFINED | simple::NULL => visitor.visit_none(),
            v => Err(Error::UnknownSimpleValue(v.into())),
        }
    }
}

impl<'de, 'a, R: Read<'de>, S: Scratch> de::Deserializer<'de> for &'a mut Deserializer<R, S> {
    type Error = Error;

    fn deserialize_any<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        let Header(major, arg) = self.parser.peek_header()?;
        match (major, arg) {
            (Major::Positive, Arg::First(_) | Arg::Next1(_)) => self.deserialize_u8(visitor),
            (Major::Positive, Arg::Next2(_)) => self.deserialize_u16(visitor),
            (Major::Positive, Arg::Next4(_)) => self.deserialize_u32(visitor),
            (Major::Positive, Arg::Next8(_)) => self.deserialize_u64(visitor),
            (Major::Negative, Arg::First(_) | Arg::Next1(_)) => self.deserialize_i8(visitor),
            (Major::Negative, Arg::Next2(_)) => self.deserialize_i16(visitor),
            (Major::Negative, Arg::Next4(_)) => self.deserialize_i32(visitor),
            (Major::Negative, Arg::Next8(_)) => self.deserialize_i64(visitor),
            (Major::Tag, _) => todo!(),
            (Major::Bytes, _) => self.deserialize_bytes(visitor),
            (Major::Text, _) => self.deserialize_str(visitor),
            (Major::Array, _) => self.deserialize_seq(visitor),
            (Major::Map, _) => self.deserialize_map(visitor),
            (Major::Other, Arg::First(_) | Arg::Next1(_)) => self.deserialize_simple(visitor),
            (Major::Other, Arg::Next2(_) | Arg::Next4(_)) => self.deserialize_f32(visitor), // f32 is intentional here, as f16 is not a native type
            (Major::Other, Arg::Next8(_)) => self.deserialize_f64(visitor),
            (_, Arg::Indef) => Err(Error::UnexpectedBreak),
        }
    }

    fn deserialize_bool<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        let b = match Simple::try_from(self.parser.read_header()?) {
            Ok(simple::TRUE) => true,
            Ok(simple::FALSE) => false,
            _ => return Err(Error::ExpectedBool),
        };
        visitor.visit_bool(b)
    }

    fn deserialize_i8<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        visitor.visit_i8(self.parse_number()?)
    }

    fn deserialize_i16<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        visitor.visit_i16(self.parse_number()?)
    }

    fn deserialize_i32<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        visitor.visit_i32(self.parse_number()?)
    }

    fn deserialize_i64<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        visitor.visit_i64(self.parse_number()?)
    }

    fn deserialize_i128<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        visitor.visit_i128(self.parse_number()?)
    }

    fn deserialize_u8<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        visitor.visit_u8(self.parse_number()?)
    }

    fn deserialize_u16<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        visitor.visit_u16(self.parse_number()?)
    }

    fn deserialize_u32<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        visitor.visit_u32(self.parse_number()?)
    }

    fn deserialize_u64<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        visitor.visit_u64(self.parse_number()?)
    }

    fn deserialize_u128<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        visitor.visit_u128(self.parse_number()?)
    }

    fn deserialize_f32<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        visitor.visit_f32(self.parse_number()?)
    }

    fn deserialize_f64<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        visitor.visit_f64(self.parse_number()?)
    }

    fn deserialize_char<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        let (Reference::Borrowed(s) | Reference::Copied(s)) =
            self.parser.read_buffer_chain(&mut self.scratch)?.as_str()?;
        visitor.visit_str(s)
    }

    #[inline]
    fn deserialize_str<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        ByteSeqAccess::new(self).deserialize_slice(visitor)
    }

    #[cfg(not(feature = "alloc"))]
    #[inline]
    fn deserialize_string<V>(self, _visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        unreachable!()
    }

    #[cfg(feature = "alloc")]
    #[inline]
    fn deserialize_string<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        ByteSeqAccess::new(self).deserialize_owned(visitor)
    }

    #[inline]
    fn deserialize_bytes<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        ByteSeqAccess::new(self).deserialize_slice(visitor)
    }

    #[cfg(not(feature = "alloc"))]
    #[inline]
    fn deserialize_byte_buf<V>(self, _visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        unreachable!()
    }

    #[cfg(feature = "alloc")]
    #[inline]
    fn deserialize_byte_buf<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        ByteSeqAccess::new(self).deserialize_owned(visitor)
    }

    fn deserialize_option<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        match Simple::try_from(self.parser.peek_header()?) {
            Ok(simple::NULL | simple::UNDEFINED) => visitor.visit_none(),
            Ok(_) | Err(Error::ExpectedSimple) => visitor.visit_some(self),
            Err(e) => Err(e),
        }
    }

    fn deserialize_unit<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        match Simple::try_from(self.parser.read_header()?)? {
            simple::UNDEFINED | simple::NULL => visitor.visit_unit(),
            _ => Err(Error::ExpectedSimple),
        }
    }

    #[inline]
    fn deserialize_unit_struct<V>(
        self,
        _name: &'static str,
        visitor: V,
    ) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        self.deserialize_unit(visitor)
    }

    #[inline]
    fn deserialize_newtype_struct<V>(
        self,
        _name: &'static str,
        visitor: V,
    ) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        visitor.visit_newtype_struct(self)
    }

    #[inline]
    fn deserialize_seq<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        let Header(major, arg) = self.parser.read_header()?;
        let sequence = match major {
            Major::Array => Sequence::new(self, arg.sequence_length()?),
            _ => return Err(Error::ExpectedArray),
        };
        visitor.visit_seq(sequence)
    }

    #[inline]
    fn deserialize_tuple<V>(self, _len: usize, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        self.deserialize_seq(visitor)
    }

    #[inline]
    fn deserialize_tuple_struct<V>(
        self,
        _name: &'static str,
        len: usize,
        visitor: V,
    ) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        self.deserialize_tuple(len, visitor)
    }

    fn deserialize_map<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        let Header(major, arg) = self.parser.read_header()?;
        let sequence = match major {
            Major::Map => Sequence::new(self, arg.sequence_length()?),
            _ => return Err(Error::ExpectedMap),
        };
        visitor.visit_map(sequence)
    }

    #[inline]
    fn deserialize_struct<V>(
        self,
        _name: &'static str,
        _fields: &'static [&'static str],
        visitor: V,
    ) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        self.deserialize_map(visitor)
    }

    fn deserialize_enum<V>(
        self,
        _name: &'static str,
        _variants: &'static [&'static str],
        visitor: V,
    ) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        visitor.visit_enum(Enum::new(self))
    }

    #[inline]
    fn deserialize_identifier<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        self.deserialize_str(visitor)
    }

    #[inline]
    fn deserialize_ignored_any<V>(self, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        self.deserialize_any(visitor)
    }
}

struct Enum<'a, R, S> {
    de: &'a mut Deserializer<R, S>,
}

impl<'a, 'de: 'a, R: Read<'de>, S: Scratch> Enum<'a, R, S> {
    fn new(de: &'a mut Deserializer<R, S>) -> Self {
        Self { de }
    }
}

impl<'a, 'de: 'a, R: Read<'de>, S: Scratch> EnumAccess<'de> for Enum<'a, R, S> {
    type Error = Error;
    type Variant = Self;

    #[inline]
    fn variant_seed<V>(self, seed: V) -> Result<(V::Value, Self::Variant), Self::Error>
    where
        V: DeserializeSeed<'de>,
    {
        let variant = seed.deserialize(&mut *self.de)?;
        Ok((variant, self))
    }
}

impl<'a, 'de: 'a, R: Read<'de>, S: Scratch> VariantAccess<'de> for Enum<'a, R, S> {
    type Error = Error;

    #[inline]
    fn unit_variant(self) -> Result<(), Self::Error> {
        Ok(())
    }

    #[inline]
    fn newtype_variant_seed<T>(self, seed: T) -> Result<T::Value, Self::Error>
    where
        T: DeserializeSeed<'de>,
    {
        seed.deserialize(&mut *self.de)
    }

    #[inline]
    fn tuple_variant<V>(self, len: usize, visitor: V) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        self.de.deserialize_tuple(len, visitor)
    }

    #[inline]
    fn struct_variant<V>(
        self,
        fields: &'static [&'static str],
        visitor: V,
    ) -> Result<V::Value, Self::Error>
    where
        V: Visitor<'de>,
    {
        self.de.deserialize_struct("", fields, visitor)
    }
}

struct Sequence<'a, R, S> {
    de: &'a mut Deserializer<R, S>,
    len: Option<usize>,
}

impl<'a, 'de: 'a, R: Read<'de>, S: Scratch> Sequence<'a, R, S> {
    fn new(de: &'a mut Deserializer<R, S>, len: Option<usize>) -> Self {
        Self { de, len }
    }

    fn end(&mut self) -> Result<bool, Error> {
        Ok(match self.len {
            Some(0) => true,
            Some(_) => false,
            None => self.de.parser.peek_header()? == Header::BREAK,
        })
    }

    #[inline]
    fn finish<K>(&mut self) -> Result<Option<K>, Error> {
        if self.len.is_none() {
            let header = self.de.parser.read_header()?;
            assert_eq!(header, Header::BREAK);
        } else {
            assert_eq!(self.len, Some(0));
        }
        Ok(None)
    }
}

impl<'a, 'de: 'a, R: Read<'de>, S: Scratch> MapAccess<'de> for Sequence<'a, R, S> {
    type Error = Error;

    fn next_key_seed<K>(&mut self, seed: K) -> Result<Option<K::Value>, Self::Error>
    where
        K: DeserializeSeed<'de>,
    {
        if self.end()? {
            self.finish()
        } else {
            seed.deserialize(&mut *self.de).map(Some)
        }
    }

    fn next_value_seed<V>(&mut self, seed: V) -> Result<V::Value, Self::Error>
    where
        V: DeserializeSeed<'de>,
    {
        if self.end()? {
            return Err(Error::UnexpectedBreak);
        }
        self.len = self.len.map(|l| l - 1);
        seed.deserialize(&mut *self.de)
    }

    fn size_hint(&self) -> Option<usize> {
        self.len
    }
}

impl<'a, 'de: 'a, R: Read<'de>, S: Scratch> SeqAccess<'de> for Sequence<'a, R, S> {
    type Error = Error;

    fn next_element_seed<T>(&mut self, seed: T) -> Result<Option<T::Value>, Self::Error>
    where
        T: DeserializeSeed<'de>,
    {
        if self.end()? {
            self.finish()
        } else {
            self.len = self.len.map(|l| l - 1);
            seed.deserialize(&mut *self.de).map(Some)
        }
    }

    fn size_hint(&self) -> Option<usize> {
        self.len
    }
}

#[cfg(test)]
#[allow(clippy::missing_panics_doc)]
pub mod tests {
    use core::fmt::Debug;

    use num_traits::Float;
    use serde::Deserialize;
    use test_case::case;

    use super::*;
    use crate::scratch::NullScratch;

    #[case("0x00", 0u8)]
    #[case("0x01", 1u8)]
    #[case("0x0a", 10u8)]
    #[case("0x17", 23u8)]
    #[case("0x1818", 24u8)]
    #[case("0x1819", 25u8)]
    #[case("0x1864", 100u8)]
    #[case("0x1903e8", 1000u16)]
    #[case("0x1a000f4240", 1_000_000_u32)]
    #[case("0x1b000000e8d4a51000", 1_000_000_000_000_u64)]
    #[case("0x1bffffffffffffffff", 18_446_744_073_709_551_615_u64)]
    // #[case("0xc249010000000000000000", 18446744073709551616)]
    #[case("0x3bffffffffffffffff", -18_446_744_073_709_551_616_i128)]
    #[case("0x20", -1)]
    #[case("0x29", -10)]
    #[case("0x3863", -100)]
    #[case("0x3903e7", -1000)]
    #[case("0xf90000", 0.0)]
    #[case("0xf98000", -0.0)]
    #[case("0xf93c00", 1.0)]
    #[case("0xfb3ff199999999999a", 1.1)]
    #[case("0xf93e00", 1.5)]
    #[case("0xf97bff", 65504.0)]
    #[case("0xfa47c35000", 100_000.0)]
    #[case("0xfa7f7fffff", 3.402_823_466_385_288_6e38)]
    #[case("0xfb7e37e43c8800759c", 1.0e+300)]
    #[case("0xf90001", 5.960_464_477_539_063e-8)]
    #[case("0xf90400", 0.000_061_035_156_25)]
    #[case("0xf9c400", -4.0)]
    #[case("0xfbc010666666666666", -4.1)]
    #[case("0xf97c00", f32::INFINITY)]
    #[case("0xf9fc00", f32::NEG_INFINITY)]
    #[case("0xfa7f800000", f32::INFINITY)]
    #[case("0xfaff800000", f32::NEG_INFINITY)]
    #[case("0xfb7ff0000000000000", f64::INFINITY)]
    #[case("0xfbfff0000000000000", f64::NEG_INFINITY)]
    #[case("0xf4", false)]
    #[case("0xf5", true)]
    // #[case("0x40", ByteBuf::from(*b""))]
    // #[case("0x4401020304", ByteBuf::from(hex::decode("01020304").unwrap()))]
    // #[case("0x60", String::from(""))]
    #[case("0x6161", 'a')]
    // #[case("0x6449455446", String::from("IETF"))]
    // #[case("0x62225c", String::from("\"\\"))]
    // #[case("0x62c3bc", String::from("\u{00fc}"))]
    // #[case("0x63e6b0b4", String::from("\u{6c34}"))]
    // #[case("0x64f0908591", String::from_utf8(hex::decode("f0908591").unwrap()).unwrap())]
    // #[case("0xa0", HashMap::<u8, u8>::new())]
    // #[case("0xa201020304", HashMap::from([(1,2), (3,4)]))]
    // #[case("0xa26161016162820203", ]
    // #[case("0x826161a161626163", ]
    // #[case("0xa56161614161626142616361436164614461656145", HashMap::<String, String>::from([("a".into(),"A".into()), ("b".into(),"B".into()), ("c".into(), "C".into()), ("d".into(), "D".into()), ("e".into(), "E".into())]))]
    pub fn test_primitive<T: for<'a> Deserialize<'a> + Debug + PartialEq + Copy>(
        input: &str,
        expected: T,
    ) {
        let mut input_hex: heapless::Vec<u8, 1024> = heapless::Vec::new();
        input_hex.resize_default(input.len() / 2 - 1).unwrap();
        hex::decode_to_slice(&input[2..], &mut input_hex).unwrap();

        let mut des = Deserializer::from_slice(
            &input_hex,
            NullScratch::new(),
            DeserializerConfig::default(),
        );

        let value = T::deserialize(&mut des).unwrap();
        assert_eq!(value, expected);
    }

    #[case("0xf97e00", f32::NAN)]
    #[case("0xfa7fc00000", f32::NAN)]
    #[case("0xfb7ff8000000000000", f64::NAN)]
    pub fn test_nan<T: for<'a> Deserialize<'a> + Debug + Float>(input: &str, _expected: T) {
        let mut input_hex: heapless::Vec<u8, 1024> = heapless::Vec::new();
        input_hex.resize_default(input.len() / 2 - 1).unwrap();
        hex::decode_to_slice(&input[2..], &mut input_hex).unwrap();

        let mut des = Deserializer::from_slice(
            &input_hex,
            NullScratch::new(),
            DeserializerConfig::default(),
        );

        let value = T::deserialize(&mut des).unwrap();
        assert!(value.is_nan());
    }

    #[test]
    pub fn test_struct() {
        #[derive(Deserialize)]
        struct X<'a> {
            a: &'a str,
            b: &'a str,
            c: &'a str,
            d: &'a str,
            e: &'a str,
        }

        let test = hex::decode("a56161614161626142616361436164614461656145").unwrap();

        let mut des =
            Deserializer::from_slice(&test, NullScratch::new(), DeserializerConfig::default());

        let x = X::deserialize(&mut des).unwrap();
        assert_eq!(x.a, "A");
        assert_eq!(x.b, "B");
        assert_eq!(x.c, "C");
        assert_eq!(x.d, "D");
        assert_eq!(x.e, "E");
    }
}

#[cfg(all(test, feature = "std"))]
#[allow(clippy::missing_panics_doc)]
pub mod std_tests {
    use core::fmt::Debug;
    use std::collections::HashMap;

    use serde::Deserialize;
    use serde_bytes::ByteBuf;
    use test_case::case;

    use super::*;
    use crate::scratch::NullScratch;

    #[case("0x40", &ByteBuf::new())]
    #[case("0x4401020304", &ByteBuf::from(hex::decode("01020304").unwrap()))]
    #[case("0x60", &String::new())]
    #[case("0x6449455446", &String::from("IETF"))]
    #[case("0x62225c", &String::from("\"\\"))]
    #[case("0x62c3bc", &String::from("\u{00fc}"))]
    #[case("0x63e6b0b4", &String::from("\u{6c34}"))]
    #[case("0x64f0908591", &String::from_utf8(hex::decode("f0908591").unwrap()).unwrap())]
    #[case("0xa0", &HashMap::<u8, u8>::new())]
    #[case("0xa201020304", &HashMap::from([(1,2), (3,4)]))]
    // #[case("0xa26161016162820203", ]
    // #[case("0x826161a161626163", ]
    #[case("0xa56161614161626142616361436164614461656145", &HashMap::<String, String>::from([("a".into(),"A".into()), ("b".into(),"B".into()), ("c".into(), "C".into()), ("d".into(), "D".into()), ("e".into(), "E".into())]))]
    pub fn test_primitive<T: for<'a> Deserialize<'a> + Debug + PartialEq>(
        input: &str,
        expected: &T,
    ) {
        let input_hex = hex::decode(&input[2..]).unwrap();

        let mut des = Deserializer::from_slice(
            &input_hex,
            NullScratch::new(),
            DeserializerConfig::default(),
        );

        let value = T::deserialize(&mut des).unwrap();
        assert_eq!(&value, expected);
    }
}
