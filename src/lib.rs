#![cfg_attr(not(feature = "std"), no_std)]
#![forbid(unsafe_code)]
#![warn(clippy::all, clippy::pedantic, clippy::nursery, clippy::cargo)]

#[cfg(feature = "alloc")]
extern crate alloc;

pub use crate::error::Error;

pub mod de;
mod error;
mod header;
pub mod io;
pub mod scratch;
mod simple;

pub use heapless;
