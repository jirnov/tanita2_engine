/* RLE compression/decompression library
 * Copyright (c) Trickster Games, 2007
 */

#include <precompiled.h>
#include "rlelib.h"
#include <string.h>

U32 gProbability[256];

// Compress data with run-length encoding (RLE)
U32 rle_compress( U8 * dst, U32 dstSize, const U8 * src, U32 srcSize )
{
    // Calculating probabilities for characters
    memset(gProbability, 0, 1024);
    const U8 * ptr = src,
             * ptrEnd = ptr + srcSize;
    while (ptr != ptrEnd)
    {
        if (gProbability[*ptr] != 0xffffffff)
            gProbability[*ptr]++;
        ptr++;
    }
    
    // Calculating magic
    U32 magicProb = gProbability[0];
    U8 magic = 0;
    for (int i = 1; i < 256; ++i)
        if (magicProb > gProbability[i])
        {
            magic = (U8)i;
            magicProb = gProbability[i];
        }
    const U8 RLE_MAGIC = magic;
    U32 packedSize = 0;
    
    // Writing magic at buffer start
    packedSize++;
    if (packedSize > dstSize)
        return 0;
    *dst++ = RLE_MAGIC;
    
    // Compressing
    U8 * srcEnd = (U8 *)src + srcSize;
    while (src != srcEnd)
    {
        if (*src++ == RLE_MAGIC)
        {
            packedSize += 2;
            if (packedSize > dstSize)
                return 0;
            *dst++ = RLE_MAGIC;
            *dst++ = RLE_MAGIC;
            continue;
        }
        
        U8 value = *(--src);
        U16 repeatCount = 0;

        while (*src == value)
        {
            *src++;
            if (++repeatCount == 0x80FF || src == srcEnd)
                break;
        }
        if (repeatCount > 3)
        {
            packedSize += 3;
            if (repeatCount >= 0x80)
                packedSize++;
            if (packedSize > dstSize)
                return 0;
            
            *dst++ = RLE_MAGIC;
            *dst++ = value;
            if (repeatCount >= 0x80)
            {
                *dst++ = (U8)(repeatCount >> 8);
                *dst++ = (U8)(repeatCount & 0x00FF);
            }
            else
                *dst++ = (U8)repeatCount | (0x80);
            continue;
        }
        
        packedSize += repeatCount;
        if (packedSize > dstSize)
            return 0;

        while (repeatCount-- > 0)
            *dst++ = value;
        
    }
    return packedSize;
}

// Uncompress data compressed with run-length encoding (RLE)
U32 rle_uncompress( U8 * dst, U32 dstSize, const U8 * src, U32 srcSize )
{
    const U8 RLE_MAGIC = *src++;
    srcSize--;
    U32 unpackedSize = 0;
    U8 * srcEnd = (U8 *)src + srcSize;
    
    while (src < srcEnd)
    {
        U8 value = *src++;
        if (value == RLE_MAGIC)
        {
            U32 repeatCount = (U8)*src++;
            
            if ((U8)repeatCount == value)
            {
                if (++unpackedSize > dstSize)
                    return 0;
                *dst++ = value;
            }
            else
            {
                value = (U8)repeatCount;
                repeatCount = (U8)*src++;
                if (repeatCount > 0x80)
                    repeatCount &= 0x7f;
                else
                    repeatCount = (repeatCount << 8) | (*src++);
                
                unpackedSize += repeatCount;
                if (unpackedSize > dstSize)
                    return 0;
                
                __asm
                {
                    xor  eax,  eax
                    mov  al,   value
                    mov  ecx,  repeatCount
                    mov  edi,  dst
                    cld
                    rep stosb
                    mov  dst,  edi
                };
            }
        }
        else
        {
            if (++unpackedSize > dstSize)
                return 0;
            *dst++ = value;
        }
    }
    return unpackedSize;
}
