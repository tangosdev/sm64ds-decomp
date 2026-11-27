#include "CLPS.h"
#include "StageMap.h"
#include "types.h"

/* Definition order is what makes the size-sort ties match address order. */

extern ObjTable data_ov034_0211408c;
extern ObjTable data_ov034_02114094;
extern ObjSubTable data_ov034_021140e0[3];
extern LevelRec data_ov034_021140f8;
extern ObjSubTable data_ov034_02114114[7];
extern StandardEntry data_ov034_0211414c[5];
extern StandardEntry data_ov034_0211419c[6];
extern CLPS_Block128 data_ov034_021141fc;
extern SimpleEntry data_ov034_0211427c[24];

u16 data_ov034_02114080[2] = {0x07e2, 0x07e3};
ObjTable data_ov034_02114094 = {7, {0, 0}, &data_ov034_02114114};
u32 data_ov034_02114084 = 0x03033334;
s16 data_ov034_02114088[2] = {0x0ce4, 0x0ce4};
FogEntry data_ov034_0211409c = {1, {0, 0, 0}, 0x36b0, 0x7e2c};
ObjTable data_ov034_0211408c = {3, {0, 0}, &data_ov034_021140e0};
Area data_ov034_021140a4 = {&data_ov034_0211408c, 0, 1, {0x01, 0x48, 0x0d}};

ViewEntry data_ov034_021140b0[1] = {
    {2, 0xff, 0x03e8, 0x0496, 0x06a4, {0, 0}, (s16)0xc000, 0},
};
ExitEntry data_ov034_021140c0[1] = {
    {0x01f4, 0x03e8, 0x0884, 0, (s16)0x8000, {0x18, 0x04, 0xff, 0x11}},
};

StandardEntry data_ov034_021140d0[1] = {
    {0x0014, 0x02ee, 0x03e8, 0x053c, 3, (s16)0xc000, 0, 0xffff},
};

ObjSubTable data_ov034_021140e0[3] = {
    {0, 6, {0, 0}, &data_ov034_0211419c},
    {5, 24, {0, 0}, &data_ov034_0211427c},
    {0xa0, 1, {0, 0}, &data_ov034_021140d0},
};

LevelRec data_ov034_021140f8 = {
    (struct CLPS_Block *)&data_ov034_021141fc,
    &data_ov034_02114094,
    0x07e5,
    0x07e1,
    0x07e6,
    0x07e7,
    &data_ov034_021140a4,
    1,
    1,
    0x01f4,
    0x0000000f,
};

ObjSubTable data_ov034_02114114[7] = {
    {4, 1, {0, 0}, &data_ov034_021140b0},  {11, 2, {0, 0}, &data_ov034_02114080},
    {1, 5, {0, 0}, &data_ov034_0211414c},  {8, 1, {0, 0}, &data_ov034_0211409c},
    {12, 2, {0, 0}, &data_ov034_02114088}, {14, 1, {0, 0}, &data_ov034_02114084},
    {10, 1, {0, 0}, &data_ov034_021140c0},
};

StandardEntry data_ov034_0211414c[5] = {
    {0, 0x01f4, 0x03e8, 0x076c, 0, (s16)0x8000, 0, 0},
    {0, 0x01f4, 0x03e8, 0x06a4, 0, (s16)0x8000, 0, 0},
    {0, 0x01f4, 0x03e8, 0x05dc, 0, (s16)0x8000, 0, 0},
    {0, 0x01f4, 0x03e8, 0x0514, 0, (s16)0x8000, 0, 0},
    {0, 0, 0x0b86, 0, 0, 0, 0, 0x0278},
};

StandardEntry data_ov034_0211419c[6] = {
    {0x0031, (s16)0xf934, 0x076c, (s16)0xfe70, 0, 0, 0, 0x00f3},
    {0x0118, 0x018c, 0x0708, 0, 0, 0, 0, 0xff06},
    {0x0131, (s16)0xfbb4, 0x0514, 0x0546, 0, 0, 0, 0xff00},
    {0x00a1, (s16)0xfe70, 0x0578, 0x0352, 0, 0, 0, 0xffff},
    {0x001e, (s16)0xf8f8, 0x060e, 0x0708, 0, 0, 0, 0xff06},
    {0x002c, (s16)0xf9f2, 0x0514, 0x0708, 0, 0, 0, 0x3c07},
};

CLPS_Block128 data_ov034_021141fc = {0x53504c43,
                                     8,
                                     15,
                                     {{0x00030fc4, 0x000000ff},
                                      {0x00240fc0, 0x000000ff},
                                      {0x001c0fcc, 0x000000ff},
                                      {0x01000fc4, 0x000000ff},
                                      {0x00000fc0, 0x000000ff},
                                      {0x01030fc4, 0x000000ff},
                                      {0x01002fc4, 0x000000ff},
                                      {0x00040fc4, 0x000000ff},
                                      {0x00030fc2, 0x000000ff},
                                      {0x00030fcc, 0x000000ff},
                                      {0x00032fc2, 0x000000ff},
                                      {0x01042fcc, 0x000000ff},
                                      {0x01040fcc, 0x000000ff},
                                      {0x00052fc2, 0x000000ff},
                                      {0x01052fc4, 0x000000ff}}};

SimpleEntry data_ov034_0211427c[24] = {
    {0xa226, 0x0384, 0x03e8, 0x03e8},
    {0xa226, 0x05e3, 0x03e8, 0x00b0},
    {0xa226, 0x0334, 0x03e8, (s16)0xfc8e},
    {0xa226, (s16)0xfdef, 0x02ee, (s16)0xfdef},
    {0xa226, (s16)0xff30, 0x03e8, (s16)0xfabd},
    {0xa226, (s16)0xf8d0, 0x05dc, (s16)0xfbb4},
    {0xa226, (s16)0xf90c, 0x03e8, 0x0110},
    {0xa226, (s16)0xfe84, 0x0514, 0x044c},
    {0x4c3f, 0, 0x073a, 0},
    {0xe225, (s16)0xfc7c, 0x074f, (s16)0xfc7c},
    {0xe225, (s16)0xfc18, 0x0766, (s16)0xfc18},
    {0xe225, (s16)0xfbb4, 0x077d, (s16)0xfbb4},
    {0xe225, (s16)0xfb50, 0x0794, (s16)0xfb50},
    {0xe225, (s16)0xfaec, 0x07ab, (s16)0xfaec},
    {0xe225, 0x0384, 0x074f, 0x0384},
    {0xe225, 0x03e8, 0x0766, 0x03e8},
    {0xe225, 0x044c, 0x077d, 0x044c},
    {0xe225, 0x04b0, 0x0794, 0x04b0},
    {0xe225, 0x0514, 0x07ab, 0x0514},
    {0x0a3f, (s16)0xf8f8, 0x0226, (s16)0xfa56},
    {0xe1ff, 0, 0x06f2, 0},
    {0x0227, (s16)0xfcae, 0x0514, 0x04ce},
    {0x0227, (s16)0xfcae, 0x0514, 0x05fa},
    {0xce3f, (s16)0xf8b7, 0x062c, (s16)0xf8d0},
};
