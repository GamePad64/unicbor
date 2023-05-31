use crate::header::{Header, PartialHeader};
use crate::io::{Read, Reference};
use crate::scratch::Scratch;
use crate::Error;

pub struct Parser<R> {
    reader: R,
    peeked_header: Option<Header>,
}

impl<'de, R: Read<'de>> Parser<R> {
    pub const fn new(reader: R) -> Self {
        Self {
            reader,
            peeked_header: None,
        }
    }

    pub fn read_header(&mut self) -> Result<Header, Error> {
        if let Some(header) = self.peeked_header.take() {
            return Ok(header);
        }

        let mut header_buf = [0u8; 9];
        self.reader.read_exact(&mut header_buf[..1])?;

        let partial = PartialHeader::from_byte(header_buf[0])?;

        self.reader.read_exact(&mut header_buf[1..partial.len()])?;
        Header::from_partial(partial, &header_buf[..partial.len()])
    }

    pub fn peek_header(&mut self) -> Result<Header, Error> {
        if let Some(header) = self.peeked_header {
            return Ok(header);
        }
        self.peeked_header = Some(self.read_header()?);
        Ok(self.peeked_header.unwrap())
    }

    pub fn read_exact(&mut self, buf: &mut [u8]) -> Result<(), Error> {
        self.reader.read_exact(buf)
    }

    pub fn read_buffer<'c, S: Scratch>(
        &mut self,
        length: usize,
        scratch: &'c mut S,
    ) -> Result<Reference<'de, 'c, [u8]>, Error> {
        self.reader.read_buffer(length, scratch)
    }

    pub fn read_buffer_chain<'c, S: Scratch>(
        &mut self,
        scratch: &'c mut S,
    ) -> Result<Reference<'de, 'c, [u8]>, Error> {
        let Header(expected_major, arg) = self.read_header()?;

        if let Some(length) = arg.sequence_length()? {
            self.read_buffer(length, scratch)
        } else {
            scratch.clear();

            let mut maybe_borrowed = true;

            loop {
                let Header(major, arg) = self.read_header()?;
                if major != expected_major {
                    return Err(Error::IndefiniteStringInconsistent);
                }

                if let Some(local_length) = arg.sequence_length()? {
                    if local_length == 0 {
                        continue;
                    }

                    if maybe_borrowed && self.peek_header()? == Header::BREAK {
                        return self.read_buffer(local_length, scratch);
                    }

                    let new_slice_index = scratch.len()..scratch.len() + local_length;
                    scratch.resize(new_slice_index.end)?;

                    let local_buffer = scratch.get_mut(new_slice_index.clone()).unwrap();
                    self.read_exact(local_buffer)?;

                    maybe_borrowed = false;
                } else {
                    break;
                }
            }

            Ok(Reference::Copied(scratch))
        }
    }
}
