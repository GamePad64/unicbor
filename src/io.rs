use crate::scratch::Scratch;
use crate::Error;

pub enum Reference<'b, 'c, T>
where
    T: ?Sized + 'static,
{
    Borrowed(&'b T),
    Copied(&'c T),
}

impl<'b, 'c> Reference<'b, 'c, [u8]> {
    pub fn as_str(&self) -> Result<Reference<'b, 'c, str>, Error> {
        Ok(match self {
            Self::Borrowed(s) => Reference::Borrowed(
                core::str::from_utf8(s).map_err(|_| Error::InvalidUnicodeCodePoint)?,
            ),
            Self::Copied(s) => Reference::Copied(
                core::str::from_utf8(s).map_err(|_| Error::InvalidUnicodeCodePoint)?,
            ),
        })
    }
}

pub trait Read<'b> {
    // Read into provided buffer
    fn read_exact(&mut self, buf: &mut [u8]) -> Result<(), Error>;

    // If possible, return subslice from internal buffer. If not, read to scratchpad and return.
    fn read_buffer<'c, S: Scratch>(
        &mut self,
        length: usize,
        scratch: &'c mut S,
    ) -> Result<Reference<'b, 'c, [u8]>, Error>;
}

pub struct SliceReader<'b> {
    slice: &'b [u8],
}

impl<'b> SliceReader<'b> {
    #[inline]
    #[must_use]
    pub const fn new(slice: &'b [u8]) -> Self {
        SliceReader { slice }
    }

    #[inline]
    const fn check_len(&self, len: usize) -> Result<(), Error> {
        if len > self.slice.len() {
            return Err(Error::Eof);
        }
        Ok(())
    }
}

impl<'b> Read<'b> for SliceReader<'b> {
    #[inline]
    fn read_exact(&mut self, buf: &mut [u8]) -> Result<(), Error> {
        self.check_len(buf.len())?;

        let (result, slice) = self.slice.split_at(buf.len());
        self.slice = slice;
        buf.copy_from_slice(result);
        Ok(())
    }

    #[inline]
    fn read_buffer<'c, S: Scratch>(
        &mut self,
        length: usize,
        _scratch: &'c mut S,
    ) -> Result<Reference<'b, 'c, [u8]>, Error> {
        self.check_len(length)?;

        let (result, slice) = self.slice.split_at(length);
        self.slice = slice;
        Ok(Reference::Borrowed(result))
    }
}

#[cfg(feature = "std")]
mod stdio {
    use std::io;

    use super::*;

    pub struct Reader<R> {
        inner: R,
    }

    impl<R: io::Read> Reader<R> {
        pub fn new(inner: R) -> Self {
            Self { inner }
        }
    }

    impl<'b, R: io::Read> Read<'b> for Reader<R> {
        fn read_exact(&mut self, buf: &mut [u8]) -> Result<(), Error> {
            Ok(self.inner.read_exact(buf)?)
        }

        fn read_buffer<'c, S: Scratch>(
            &mut self,
            length: usize,
            scratch: &'c mut S,
        ) -> Result<Reference<'b, 'c, [u8]>, Error> {
            scratch.resize(length)?;
            let buf = scratch.as_mut();
            self.read_exact(buf)?;
            Ok(Reference::Copied(buf))
        }
    }
}
#[cfg(feature = "std")]
pub use stdio::Reader;
