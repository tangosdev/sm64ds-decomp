#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef union {
    u32 w;
    void *p;
} Word;

extern Word data_ov018_02112ea8[];
extern Word gPenguinAngryTextureSequenceFile[];
extern Word gPenguinWait1TextureSequenceFile[];
extern Word gPenguinWait1AnimationFile[];
extern Word gPenguinWalkAnimationFile[];
extern const Word data_ov018_02112a74[];
extern const Word data_ov018_02112a78[];
extern const Word data_ov018_02112a7c[];
extern const Word data_ov018_02112a98[];

const Word data_ov018_02112a74[1] = {{.w = 0x00000000}};

const Word data_ov018_02112a78[1] = {{.w = 0x00001000}};

const Word data_ov018_02112a7c[7] = {
    {.w = 0x0000ffff}, {.p = data_ov018_02112ea8}, {.w = 0x00000001}, {.w = 0x00000001},
    {.w = 0x00000001}, {.w = 0x0000005b},          {.w = 0x0000005b}};

const Word data_ov018_02112a98[95] = {{.w = 0x00000009},
                                      {.w = 0x00000036},
                                      {.w = 0x00000064},
                                      {.w = 0x00000091},
                                      {.w = 0x000000bf},
                                      {.w = 0x000000ec},
                                      {.w = 0x0000011a},
                                      {.w = 0x00000147},
                                      {.w = 0x00000174},
                                      {.w = 0x000001a2},
                                      {.w = 0x000001cf},
                                      {.w = 0x000001fd},
                                      {.w = 0x0000022a},
                                      {.w = 0x00000257},
                                      {.w = 0x00000285},
                                      {.w = 0x000002b2},
                                      {.w = 0x000002e0},
                                      {.w = 0x0000030d},
                                      {.w = 0x0000033a},
                                      {.w = 0x00000368},
                                      {.w = 0x00000395},
                                      {.w = 0x000003c3},
                                      {.w = 0x000003f0},
                                      {.w = 0x0000041e},
                                      {.w = 0x0000044b},
                                      {.w = 0x00000478},
                                      {.w = 0x000004a6},
                                      {.w = 0x000004d3},
                                      {.w = 0x00000501},
                                      {.w = 0x0000052e},
                                      {.w = 0x0000055b},
                                      {.w = 0x00000589},
                                      {.w = 0x000005b6},
                                      {.w = 0x000005e4},
                                      {.w = 0x00000611},
                                      {.w = 0x0000063e},
                                      {.w = 0x0000066c},
                                      {.w = 0x00000699},
                                      {.w = 0x000006c7},
                                      {.w = 0x000006f4},
                                      {.w = 0x00000721},
                                      {.w = 0x0000074f},
                                      {.w = 0x0000077c},
                                      {.w = 0x000007aa},
                                      {.w = 0x000007d7},
                                      {.w = 0x00000805},
                                      {.w = 0x00000832},
                                      {.w = 0x0000085f},
                                      {.w = 0x0000088d},
                                      {.w = 0x000008ba},
                                      {.w = 0x000008e8},
                                      {.w = 0x00000915},
                                      {.w = 0x00000942},
                                      {.w = 0x00000970},
                                      {.w = 0x0000099d},
                                      {.w = 0x000009cb},
                                      {.w = 0x000009f8},
                                      {.w = 0x00000a25},
                                      {.w = 0x00000a53},
                                      {.w = 0x00000a80},
                                      {.w = 0x00000aae},
                                      {.w = 0x00000adb},
                                      {.w = 0x00000b09},
                                      {.w = 0x00000b36},
                                      {.w = 0x00000b63},
                                      {.w = 0x00000b91},
                                      {.w = 0x00000bbe},
                                      {.w = 0x00000bec},
                                      {.w = 0x00000c19},
                                      {.w = 0x00000c46},
                                      {.w = 0x00000c74},
                                      {.w = 0x00000ca1},
                                      {.w = 0x00000ccf},
                                      {.w = 0x00000cfc},
                                      {.w = 0x00000d29},
                                      {.w = 0x00000d57},
                                      {.w = 0x00000d84},
                                      {.w = 0x00000db2},
                                      {.w = 0x00000ddf},
                                      {.w = 0x00000e0c},
                                      {.w = 0x00000e3a},
                                      {.w = 0x00000e67},
                                      {.w = 0x00000e95},
                                      {.w = 0x00000ec2},
                                      {.w = 0x00000ef0},
                                      {.w = 0x00000f1d},
                                      {.w = 0x00000f4a},
                                      {.w = 0x00000f78},
                                      {.w = 0x00000fa5},
                                      {.w = 0x00000fd3},
                                      {.w = 0x00001000},
                                      {.p = gPenguinAngryTextureSequenceFile},
                                      {.p = gPenguinWait1TextureSequenceFile},
                                      {.p = gPenguinWait1AnimationFile},
                                      {.p = gPenguinWalkAnimationFile}};
