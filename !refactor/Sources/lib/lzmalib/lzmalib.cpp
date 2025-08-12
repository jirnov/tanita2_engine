#include <precompiled.h>
#include "lzmalib.h"


extern "C"
{
    #include "lzma/LzmaEnc.h"
    #include "lzma/LzmaDec.h"
}


namespace 
{
    static void *SzAlloc(void *p, size_t size) { p = p; return malloc(size); }
    static void SzFree(void *p, void *address) { p = p; free(address); }
    static ISzAlloc g_Alloc = { SzAlloc, SzFree };
}


#define LZMA_HEADER_SIZE (LZMA_PROPS_SIZE + 1)


U32 lzma_compress( U8 * dest,  U32 dstSize, const U8 * src, U32 srcSize )
{
    U32 origDstSize = dstSize;

    CLzmaEncProps props;
    LzmaEncProps_Init(&props);
    props.level = 9;
    props.dictSize = 1 << 24;
    SizeT outPropsSize = LZMA_PROPS_SIZE;

    SRes result = LzmaEncode(dest + LZMA_HEADER_SIZE, (SizeT *) &dstSize, 
                             src, srcSize, 
                             &props, (Byte *)dest + 1, &outPropsSize, 
                             0, NULL, &g_Alloc, &g_Alloc);

    dstSize += LZMA_HEADER_SIZE;

    if( result == SZ_OK && dstSize <= origDstSize)
        return dstSize;

    return 0;
}


U32 lzma_uncompress( U8 * dest, U32 dstSize, const U8 * src, U32 srcSize )
{
    ELzmaStatus status;

    srcSize -= LZMA_PROPS_SIZE;

    SRes result = LzmaDecode(dest, (SizeT *) &dstSize, 
                             src + LZMA_HEADER_SIZE, (SizeT *) &srcSize, 
                             src + 1, LZMA_PROPS_SIZE, LZMA_FINISH_ANY, &status, &g_Alloc);

    if( result == SZ_OK )
        return dstSize;

    return 0;
}
