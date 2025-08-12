/* T2 Game Engine
 * Copyright (c) Trickster Games, 2007
 */

#ifndef __BASE_TYPES_H__
#define __BASE_TYPES_H__

#include <d3dx9.h>

// Type definitions used in engine
typedef long             S32;
typedef unsigned long    U32;
typedef short            S16;
typedef unsigned short   U16;
typedef char             S8;
typedef unsigned char    U8;

typedef float            F32;
typedef double           F64;

// Position in scene space
typedef D3DXVECTOR2 vec2;
// Matrix
typedef D3DXMATRIX mat4x4;

#endif // __BASE_TYPES_H__
