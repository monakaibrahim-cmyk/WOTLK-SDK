#pragma once

#include <cstdint>

struct FT_FaceRec_;
typedef struct FT_FaceRec_* FT_Face;

struct FT_LibraryRec_;
typedef struct FT_LibraryRec_* FT_Library;

struct FT_Vector {
    long x;
    long y;
};

using FT_Byte = unsigned char;
using FT_Int32 = signed int;
using FT_UInt = unsigned int;
using FT_Long = signed long;
using FT_ULong = unsigned long;

#ifndef FT_LOAD_DEFAULT
#define FT_LOAD_DEFAULT 0x0
#define FT_LOAD_NO_HINTING (1L << 1)
#define FT_LOAD_PEDANTIC (1L << 7)
#define FT_LOAD_LINEAR_DESIGN (1L << 13)
#define FT_LOAD_NO_BITMAP (1L << 3)
#define FT_LOAD_CROP_BITMAP (1L << 6)
#endif
