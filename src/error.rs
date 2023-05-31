#[cfg(feature = "alloc")]
use alloc::string::String;
use core::fmt::Display;
use core::fmt::Formatter;
#[cfg(feature = "std")]
use std::{error::Error as StdError, io};

use cfg_if::cfg_if;
use serde::{de, ser};

pub enum Category {
    Io,
    Syntax,
    Semantic,
    Eof,
    System,
}

#[derive(Debug)]
pub enum Error {
    #[cfg(feature = "alloc")]
    Message(String),
    #[cfg(not(feature = "alloc"))]
    Message(heapless::String<64>),

    Eof,

    ItemIllFormed,
    UnexpectedBreak,

    NumberOutOfRange,

    ExpectedBool,
    ExpectedNumber,
    ExpectedBytes,
    ExpectedString,
    ExpectedArray,
    ExpectedMap,
    ExpectedSimple,
    ExpectedChar,

    IndefiniteStringInconsistent,

    ScratchBufferTooSmall,

    InvalidUnicodeCodePoint,

    UnknownSimpleValue(u8),

    #[cfg(feature = "std")]
    Io(io::Error),
}

impl Error {
    #[must_use]
    pub fn classify(&self) -> Category {
        match self {
            #[cfg(feature = "alloc")]
            Self::Message(_) => Category::Semantic,
            #[cfg(not(feature = "alloc"))]
            Self::Message(_) => Category::Semantic,

            Self::Eof => Category::Eof,

            Self::ItemIllFormed | Self::UnexpectedBreak => Category::Syntax,
            Self::NumberOutOfRange
            | Self::ExpectedBool
            | Self::ExpectedNumber
            | Self::ExpectedBytes
            | Self::ExpectedString
            | Self::ExpectedArray
            | Self::ExpectedMap
            | Self::ExpectedSimple
            | Self::ExpectedChar
            | Self::InvalidUnicodeCodePoint
            | Self::IndefiniteStringInconsistent
            | Self::UnknownSimpleValue(_) => Category::Semantic,
            Self::ScratchBufferTooSmall => Category::System,
            #[cfg(feature = "std")]
            Self::Io(_) => Category::Io,
        }
    }
}

#[cfg(feature = "std")]
impl From<io::Error> for Error {
    #[inline]
    fn from(value: io::Error) -> Self {
        Self::Io(value)
    }
}

#[cfg(feature = "std")]
impl StdError for Error {}

impl Display for Error {
    fn fmt(&self, f: &mut Formatter<'_>) -> core::fmt::Result {
        write!(f, "{self:?}")
    }
}

impl ser::Error for Error {
    fn custom<T: Display>(msg: T) -> Self {
        cfg_if! {
            if #[cfg(feature = "alloc")] {
                use alloc::string::ToString;

                Self::Message(msg.to_string())
            }else{
                use core::fmt::Write;

                let mut string = heapless::String::new();
                write!(string, "{:.64}", msg).unwrap();
                Self::Message(string)
            }
        }
    }
}

impl de::Error for Error {
    fn custom<T: Display>(msg: T) -> Self {
        <Self as ser::Error>::custom(msg)
    }
}
