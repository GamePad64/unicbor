#[cfg(feature = "alloc")]
use alloc::vec::Vec;
use core::ops::{Deref, DerefMut};

use crate::Error;

pub trait Scratch: Deref<Target = [u8]> + DerefMut {
    fn resize(&mut self, new_len: usize) -> Result<(), Error>;
    fn clear(&mut self) {
        self.resize(0).unwrap();
    }
}

#[cfg(feature = "alloc")]
impl Scratch for Vec<u8> {
    #[inline]
    fn resize(&mut self, new_len: usize) -> Result<(), Error> {
        self.resize(new_len, 0);
        Ok(())
    }

    #[inline]
    fn clear(&mut self) {
        self.clear();
    }
}

impl<const N: usize> Scratch for heapless::Vec<u8, N> {
    #[inline]
    fn resize(&mut self, new_len: usize) -> Result<(), Error> {
        self.resize(new_len, 0)
            .map_err(|_| Error::ScratchBufferTooSmall)
    }

    #[inline]
    fn clear(&mut self) {
        self.clear();
    }
}

#[derive(Default)]
#[repr(transparent)]
pub struct NullScratch([u8; 0]);

impl NullScratch {
    #[must_use]
    pub fn new() -> Self {
        Self::default()
    }
}

impl Scratch for NullScratch {
    #[inline(always)]
    fn resize(&mut self, new_len: usize) -> Result<(), Error> {
        match new_len {
            0 => Ok(()),
            _ => Err(Error::ScratchBufferTooSmall),
        }
    }
}

impl Deref for NullScratch {
    type Target = [u8];

    #[inline]
    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

impl DerefMut for NullScratch {
    #[inline]
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.0
    }
}
