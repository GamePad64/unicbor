#[cfg(feature = "alloc")]
use alloc::vec::Vec;

use serde::de::Visitor;

use crate::de::Deserializer;
#[allow(unused)]
use crate::header::Header;
use crate::io::{Read, Reference};
use crate::scratch::Scratch;
use crate::Error;

pub struct ByteSeqAccess<'a, R, S> {
    de: &'a mut Deserializer<R, S>,
}

impl<'a, 'de: 'a, R: Read<'de>, S: Scratch> ByteSeqAccess<'a, R, S> {
    pub fn new(de: &'a mut Deserializer<R, S>) -> Self {
        Self { de }
    }

    pub fn deserialize_slice<V>(&mut self, visitor: V) -> Result<V::Value, Error>
    where
        V: Visitor<'de>,
    {
        let buffer = self.de.parser.read_buffer_chain(&mut self.de.scratch)?;
        match buffer {
            Reference::Borrowed(s) => visitor.visit_borrowed_bytes(s),
            Reference::Copied(s) => visitor.visit_bytes(s),
        }
    }

    #[cfg(feature = "alloc")]
    pub fn deserialize_owned<V>(&mut self, visitor: V) -> Result<V::Value, Error>
    where
        V: Visitor<'de>,
    {
        let Header(expected_major, arg) = self.de.parser.read_header()?;

        let mut buffer = Vec::new();

        if let Some(length) = arg.sequence_length()? {
            buffer.resize(length, 0);
            self.de.parser.read_exact(&mut buffer)?;
        } else {
            loop {
                let Header(major, arg) = self.de.parser.read_header()?;
                if major != expected_major {
                    return Err(Error::IndefiniteStringInconsistent);
                }

                if let Some(local_length) = arg.sequence_length()? {
                    let new_slice_index = buffer.len()..buffer.len() + local_length;
                    buffer.reserve(local_length);
                    buffer.resize(new_slice_index.end, 0);

                    let local_buffer = buffer.get_mut(new_slice_index).unwrap();
                    self.de.parser.read_exact(local_buffer)?;
                } else {
                    break;
                }
            }
        }

        visitor.visit_byte_buf(buffer)
    }
}
