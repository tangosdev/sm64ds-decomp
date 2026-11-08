#ifndef NITRO_GX_GXCOMMON_H
#define NITRO_GX_GXCOMMON_H

#include "types.h"

/* NitroSDK's GXOamAttr (gxcommon.h): one 8-byte OAM record, seen as its four
 * attribute halfwords, as attr0 and attr1 in one word (attr01, which
 * G2_SetOBJMode rewrites), or as the attribute bitfields. In a record of an
 * OBJ affine matrix, attr3 holds one of its parameters (GXOamAffine). */
typedef union GXOamAttr {
    u32 attr01;
    struct {
        u16 attr0;
        u16 attr1;
        u16 attr2;
        u16 attr3;
    };
    struct {
        u32 y : 8;
        u32 rsMode : 2;
        u32 objMode : 2;
        u32 mosaic : 1;
        u32 colorMode : 1;
        u32 shape : 2;
        u32 x : 9;
        u32 rsParam : 5;
        u32 size : 2;
        u32 charNo : 10;
        u32 priority : 2;
        u32 cParam : 4;
        u32 _3 : 16;
    };
} GXOamAttr;

#endif
