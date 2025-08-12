/* RLE compression/decompression library
 * Copyright (c) Trickster Games, 2007
 */

#ifndef _RLELIB_H_
#define _RLELIB_H_

#include <base/types.h>

//! Compress data with run-length encoding (RLE)
/**
  Compresses data from source buffer with RLE and stores in 
  into destination buffer. Checks destination size to fit
  compressed data.
  
  Returns size of compressed data or 0 on error.
  */
U32 rle_compress( U8 * dst, U32 dstSize, const U8 * src, U32 srcSize );

//! Uncompress data compressed with run-length encoding (RLE)
/**
  Uncompresses data compressed by compress function from source
  buffer to destination buffer. Checks destination size to fit
  uncompressed data.

  Returns size of uncompressed data or 0 on error.
*/
U32 rle_uncompress( U8 * dst, U32 dstSize, const U8 * src, U32 srcSize );

#endif // _RLELIB_H_
