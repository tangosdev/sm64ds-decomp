#ifndef NITRO_GX_G3_H
#define NITRO_GX_G3_H

/* NitroSDK's geometry command opcodes (nitro/gx/g3.h): the word written to
 * the geometry FIFO (REG_GXFIFO) ahead of a command's parameters. */

#define G3OP_NOP 0x00

#define G3OP_MTX_MODE 0x10
#define G3OP_MTX_PUSH 0x11
#define G3OP_MTX_POP 0x12
#define G3OP_MTX_STORE 0x13
#define G3OP_MTX_RESTORE 0x14
#define G3OP_MTX_IDENTITY 0x15
#define G3OP_MTX_LOAD_4x4 0x16
#define G3OP_MTX_LOAD_4x3 0x17
#define G3OP_MTX_MULT_4x4 0x18
#define G3OP_MTX_MULT_4x3 0x19
#define G3OP_MTX_MULT_3x3 0x1a
#define G3OP_MTX_SCALE 0x1b
#define G3OP_MTX_TRANS 0x1c

#define G3OP_COLOR 0x20
#define G3OP_NORMAL 0x21
#define G3OP_TEXCOORD 0x22
#define G3OP_VTX_16 0x23
#define G3OP_VTX_10 0x24
#define G3OP_VTX_XY 0x25
#define G3OP_VTX_XZ 0x26
#define G3OP_VTX_YZ 0x27
#define G3OP_VTX_DIFF 0x28
#define G3OP_POLYGON_ATTR 0x29
#define G3OP_TEXIMAGE_PARAM 0x2a
#define G3OP_TEXPLTT_BASE 0x2b

#define G3OP_DIF_AMB 0x30
#define G3OP_SPE_EMI 0x31
#define G3OP_LIGHT_VECTOR 0x32
#define G3OP_LIGHT_COLOR 0x33
#define G3OP_SHININESS 0x34

#define G3OP_BEGIN 0x40
#define G3OP_END 0x41

#define G3OP_SWAP_BUFFERS 0x50

#define G3OP_VIEWPORT 0x60

#define G3OP_BOX_TEST 0x70
#define G3OP_POS_TEST 0x71
#define G3OP_VEC_TEST 0x72

#endif /* NITRO_GX_G3_H */
