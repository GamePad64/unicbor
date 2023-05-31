use crate::header::{Arg, Header, Major};
use crate::Error;

#[derive(Default, Debug, Copy, Clone, Ord, PartialOrd, Eq, PartialEq)]
pub struct Simple(u8);

pub const FALSE: Simple = Simple(20);
pub const TRUE: Simple = Simple(21);

pub const NULL: Simple = Simple(22);
pub const UNDEFINED: Simple = Simple(23);

impl TryFrom<u8> for Simple {
    type Error = Error;

    #[inline]
    fn try_from(value: u8) -> Result<Self, Self::Error> {
        match value {
            24..=31 => Err(Error::ExpectedSimple),
            _ => Ok(Self(value)),
        }
    }
}

impl From<Simple> for u8 {
    #[inline]
    fn from(value: Simple) -> Self {
        value.0
    }
}

impl TryFrom<Header> for Simple {
    type Error = Error;

    #[inline]
    fn try_from(value: Header) -> Result<Self, Self::Error> {
        let Header(Major::Other, arg@ (Arg::First(_) | Arg::Next1(_))) = value else {
            return Err(Error::ExpectedSimple);
        };

        arg.as_integer::<u8>()?.try_into()
    }
}
