#ifndef _LZMALIB_H_
#define _LZMALIB_H_

#include <base/types.h>

//! Compress data with lzma
/**
  Compresses data from source buffer with LZMA and stores in 
  into destination buffer. Checks destination size to fit
  compressed data.
  
  Returns size of compressed data or 0 on error.
  */
U32 lzma_compress( U8 * dest,  U32 dstSize, const U8 * src, U32 srcSize );

//! Uncompress data compressed with LZMA
/**
  Uncompresses data compressed by compress function from source
  buffer to destination buffer. Checks destination size to fit
  uncompressed data.

  Returns size of uncompressed data or 0 on error.
*/
U32 lzma_uncompress( U8 * dest, U32 dstSize, const U8 * src, U32 srcSize );

#endif // _LZMALIB_H_
