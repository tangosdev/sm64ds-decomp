#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef union {
    u32 w;
    void *p;
} Word;

extern Word data_ov012_02111bbc[];
extern Word data_ov012_02111b94[];
extern Word data_ov012_02111c3c[];
extern Word data_ov012_02111bb4[];
extern Word data_ov012_02111b9c[];
extern Word data_ov012_02111c70[];
extern Word data_ov012_02111bdc[];
extern Word data_ov012_02111bd0[];
extern Word data_ov012_02111b80[];
extern Word data_ov012_02111cf0[];
extern Word data_ov012_02111d2c[];
extern Word data_ov012_02111d74[];
extern Word data_ov012_02111dc4[];
extern Word data_ov012_02111e18[];
extern Word data_ov012_02111e78[];
extern Word data_ov012_02111ed8[];
extern Word data_ov012_02111f48[];
extern Word data_ov012_02111ff8[];
extern Word data_ov012_02112138[];

#pragma define_section sRun ".data.c0Rest"                                                         \
                            ".data.c0Rest"                                                         \
                            ".data.c0Rest" far_data
#pragma section sRun begin

Word data_ov012_02111cf0[15] = {
    {.w = 0x00000000},          {.w = 0x00000000},          {.w = 0x012c0200},
    {.p = data_ov012_02111bbc}, {.w = 0x00000000},          {.w = 0xff060101},
    {.p = data_ov012_02111b94}, {.p = data_ov012_02111c3c}, {.w = 0xfe3e0202},
    {.p = data_ov012_02111bb4}, {.w = 0x00000000},          {.w = 0xff060203},
    {.p = data_ov012_02111b9c}, {.w = 0x00000000},          {.w = 0xfea20204}};

Word data_ov012_02111d2c[18] = {
    {.w = 0x00004c02}, {.p = data_ov012_02112138}, {.w = 0x00000503}, {.p = data_ov012_02111c70},
    {.w = 0x00000604}, {.p = data_ov012_02111dc4}, {.w = 0x0000050b}, {.p = data_ov012_02111bdc},
    {.w = 0x00000b01}, {.p = data_ov012_02111f48}, {.w = 0x00000809}, {.p = data_ov012_02111e78},
    {.w = 0x0000050c}, {.p = data_ov012_02111bd0}, {.w = 0x0000010e}, {.p = data_ov012_02111b80},
    {.w = 0x0000080a}, {.p = data_ov012_02111ed8}};

Word data_ov012_02111d74[20] = {
    {.w = 0x1af400e4}, {.w = 0xfce0fb82}, {.w = 0xc0000000}, {.w = 0xffff0000}, {.w = 0x1af400e4},
    {.w = 0xff38fb82}, {.w = 0xc0000000}, {.w = 0xffff0000}, {.w = 0x1130012f}, {.w = 0xfe3efb50},
    {.w = 0x00000000}, {.w = 0xffff0000}, {.w = 0x186a004b}, {.w = 0xfb2bfbff}, {.w = 0x0000ffff},
    {.w = 0x070f0000}, {.w = 0x0af000ed}, {.w = 0xfe0cf66e}, {.w = 0xc0000000}, {.w = 0x11040000}};

Word data_ov012_02111dc4[21] = {
    {.w = 0xfc18ff02}, {.w = 0x03e8fcae}, {.w = 0x00000000}, {.w = 0xff020000}, {.w = 0xfce00708},
    {.w = 0x0000079e}, {.w = 0x00004000}, {.w = 0xf9f2ff02}, {.w = 0xf47afc4a}, {.w = 0x75550000},
    {.w = 0xff020000}, {.w = 0xfc4af5a6}, {.w = 0x0000f7fe}, {.w = 0x0000f8e4}, {.w = 0x1db0ff02},
    {.w = 0xfe0cfb1e}, {.w = 0xc0000000}, {.w = 0xff020000}, {.w = 0xfc4a0ea6}, {.w = 0x0000f4de},
    {.w = 0x0000ce39}};

Word data_ov012_02111e18[24] = {
    {.w = 0x53504c43}, {.w = 0x000b0008}, {.w = 0x04000fc0}, {.w = 0x000000ff}, {.w = 0x00000fc0},
    {.w = 0x000000ff}, {.w = 0x00040fc0}, {.w = 0x000000ff}, {.w = 0x01040fc5}, {.w = 0x000000ff},
    {.w = 0x01043fc5}, {.w = 0x000000ff}, {.w = 0x00040fc4}, {.w = 0x000000ff}, {.w = 0x00042fc4},
    {.w = 0x000000ff}, {.w = 0x00042fc0}, {.w = 0x000000ff}, {.w = 0x00040fe0}, {.w = 0x000000ff},
    {.w = 0x01040fc4}, {.w = 0x000000ff}, {.w = 0x00044fc4}, {.w = 0x000000ff}};

Word data_ov012_02111e78[24] = {
    {.w = 0xfbe6012d}, {.w = 0xc000079d}, {.w = 0x000a0100}, {.w = 0xfbe6fb82}, {.w = 0x00000384},
    {.w = 0x00060000}, {.w = 0xfbe6fcae}, {.w = 0x80000384}, {.w = 0x00060000}, {.w = 0xfb1ef3e6},
    {.w = 0x00000577}, {.w = 0x000f0200}, {.w = 0xfd12fe70}, {.w = 0x000000c8}, {.w = 0x000f0300},
    {.w = 0xfb8202bc}, {.w = 0xc000fc63}, {.w = 0x000f2300}, {.w = 0xf9f21e14}, {.w = 0xc000fdc1},
    {.w = 0x00103300}, {.w = 0xfbe6044c}, {.w = 0xc000f58d}, {.w = 0x00022400}};

Word data_ov012_02111ed8[28] = {
    {.w = 0xfbe6fbb4}, {.w = 0x00000320}, {.w = 0x0c020000}, {.w = 0xfaba20ff}, {.w = 0xf0f6fbb4},
    {.w = 0x00000000}, {.w = 0x5504000e}, {.w = 0xf9f21de2}, {.w = 0x0000fe0c}, {.w = 0x0501c000},
    {.w = 0xf4de21ff}, {.w = 0xfbd2fbe6}, {.w = 0x80000000}, {.w = 0x43060010}, {.w = 0xfa560974},
    {.w = 0xc000f768}, {.w = 0x000d0000}, {.w = 0x0d167707}, {.w = 0x079efbe6}, {.w = 0xc0000000},
    {.w = 0x77080012}, {.w = 0xfb5a0fd2}, {.w = 0xc0000802}, {.w = 0x00250000}, {.w = 0xfc7c1109},
    {.w = 0x0320fbe6}, {.w = 0x00000000}, {.w = 0x20ff1202}};

Word data_ov012_02111f48[44] = {
    {.w = 0xfbe60000}, {.w = 0x04e2fbe6}, {.w = 0x00000000}, {.w = 0x07000000}, {.w = 0xfcae0000},
    {.w = 0x0514fbe6}, {.w = 0x00000000}, {.w = 0x00000000}, {.w = 0xfb820000}, {.w = 0x0514fbe6},
    {.w = 0x00000000}, {.w = 0x00000000}, {.w = 0xfa560000}, {.w = 0x0514fbe6}, {.w = 0x00000000},
    {.w = 0x00000000}, {.w = 0xfaba0000}, {.w = 0xf0f6fce0}, {.w = 0x00000000}, {.w = 0x05920000},
    {.w = 0x1c520000}, {.w = 0xfe0cf9f2}, {.w = 0xc0000000}, {.w = 0x07230000}, {.w = 0xf4de0000},
    {.w = 0xfbd2fce0}, {.w = 0x80000000}, {.w = 0x059a0000}, {.w = 0x09740000}, {.w = 0xf5d8f9ac},
    {.w = 0x40000000}, {.w = 0x042c0000}, {.w = 0x08fc0000}, {.w = 0x079efe0c}, {.w = 0x40000000},
    {.w = 0x02090000}, {.w = 0x0fd20000}, {.w = 0x079efb82}, {.w = 0xc0000000}, {.w = 0x04090000},
    {.w = 0xfc7c0000}, {.w = 0x04e2fbe6}, {.w = 0x00000000}, {.w = 0x07000000}};

Word data_ov012_02111ff8[80] = {
    {.w = 0xfa8f00ed}, {.w = 0xf9c2fb82}, {.w = 0x00000000}, {.w = 0x35000000}, {.w = 0xfaba002a},
    {.w = 0xf0f6fbb4}, {.w = 0x00000000}, {.w = 0x05550000}, {.w = 0xf4de002a}, {.w = 0xfbd2fb82},
    {.w = 0x80000000}, {.w = 0x06550000}, {.w = 0xf0c40050}, {.w = 0xfaecfaba}, {.w = 0x2000ffff},
    {.w = 0x0af30000}, {.w = 0xf15a0108}, {.w = 0xfa92fc4a}, {.w = 0x00000000}, {.w = 0xffff0000},
    {.w = 0xf4ac0108}, {.w = 0x019afcae}, {.w = 0x00000000}, {.w = 0xffff0000}, {.w = 0xf83a0108},
    {.w = 0xff9cfd12}, {.w = 0x40000000}, {.w = 0xffff0000}, {.w = 0xf83a0108}, {.w = 0xfd44fd12},
    {.w = 0x40000000}, {.w = 0xffff0000}, {.w = 0xf83a0108}, {.w = 0xfaecfd12}, {.w = 0x40000000},
    {.w = 0xffff0000}, {.w = 0xf9b60108}, {.w = 0x01f4fd12}, {.w = 0xc0000000}, {.w = 0xffff0000},
    {.w = 0xf9b60108}, {.w = 0xff9cfd12}, {.w = 0xc0000000}, {.w = 0xffff0000}, {.w = 0xf9b60108},
    {.w = 0xfd44fd12}, {.w = 0xc0000000}, {.w = 0xffff0000}, {.w = 0xf8a40108}, {.w = 0xf236fd12},
    {.w = 0x20000000}, {.w = 0xffff0000}, {.w = 0xfcd10108}, {.w = 0xf236fd12}, {.w = 0xe0000000},
    {.w = 0xffff0000}, {.w = 0xf3580109}, {.w = 0x04b0fcae}, {.w = 0x40000000}, {.w = 0xffff0000},
    {.w = 0xf5060109}, {.w = 0x04b0fcae}, {.w = 0xc0000000}, {.w = 0xffff0000}, {.w = 0xff380109},
    {.w = 0xfd9efcae}, {.w = 0x80000000}, {.w = 0xffff0000}, {.w = 0xfa2400ed}, {.w = 0xf4acfb82},
    {.w = 0x00000000}, {.w = 0x04010000}, {.w = 0xfaec00ed}, {.w = 0xf4acfb82}, {.w = 0x00000000},
    {.w = 0x03020000}, {.w = 0xf1f000ed}, {.w = 0xfaecfaba}, {.w = 0xe0000000}, {.w = 0x22030000}};

Word data_ov012_02112138[114] = {
    {.w = 0xfb82f894}, {.w = 0xfa88f9c0}, {.w = 0xf9c0fb7e}, {.w = 0xfb50fa88}, {.w = 0xf510f448},
    {.w = 0xf448fb50}, {.w = 0xfb82f510}, {.w = 0xf4acfaec}, {.w = 0xfaecfb82}, {.w = 0xfb50f4ac},
    {.w = 0xfed4f3e4}, {.w = 0xf3e4fb50}, {.w = 0xfb50fed4}, {.w = 0xf8f8fa88}, {.w = 0xfa88fb82},
    {.w = 0xfb82f8f8}, {.w = 0xf12802bc}, {.w = 0x02bcfb1e}, {.w = 0xfabaf128}, {.w = 0xf18cfaec},
    {.w = 0xfaecfaba}, {.w = 0xfb1ef18c}, {.w = 0xf8940258}, {.w = 0x0258fb82}, {.w = 0xfb50f574},
    {.w = 0xfa24f4ac}, {.w = 0xf4acfb50}, {.w = 0xfb82fa24}, {.w = 0xf894f9c0}, {.w = 0xf9c0fb82},
    {.w = 0xfb82f894}, {.w = 0xf1f001f4}, {.w = 0x01f4fb1e}, {.w = 0xfabaf1f0}, {.w = 0xf0c4faec},
    {.w = 0xfaecfaba}, {.w = 0xfb1ef0c4}, {.w = 0xf95c0320}, {.w = 0x0320fb82}, {.w = 0xfb82f95c},
    {.w = 0xfe70faec}, {.w = 0xfaecfb50}, {.w = 0xfb50fe70}, {.w = 0x0064fd44}, {.w = 0xfce0fb50},
    {.w = 0xfb50ff9c}, {.w = 0xff9cfa24}, {.w = 0xf830fb50}, {.w = 0xfb5001f4}, {.w = 0xff9cf5d8},
    {.w = 0xf3e4fb50}, {.w = 0xfb50faec}, {.w = 0xf448f3e4}, {.w = 0xf3e4fb50}, {.w = 0xfb82f448},
    {.w = 0xf574fb50}, {.w = 0xfb50fb82}, {.w = 0xfb50fed4}, {.w = 0xfaecf4ac}, {.w = 0xf4acfb50},
    {.w = 0xfb82faec}, {.w = 0xfe0cf9c0}, {.w = 0xf9c0fb50}, {.w = 0xfb50ff38}, {.w = 0xff9cf894},
    {.w = 0xfc7cfb50}, {.w = 0xfb50fe0c}, {.w = 0xf9c0fa88}, {.w = 0xfa88fb82}, {.w = 0xfb82f8f8},
    {.w = 0xf18c0258}, {.w = 0x0320fb1e}, {.w = 0xfaecf0c4}, {.w = 0xf18cfda8}, {.w = 0xfc7cfac2},
    {.w = 0xfb1ef254}, {.w = 0xf8f802bc}, {.w = 0x0190fb82}, {.w = 0xfb82f8f8}, {.w = 0xfa88fa88},
    {.w = 0xf95cfb82}, {.w = 0xfb50fa88}, {.w = 0xf95cf510}, {.w = 0xf4acfb50}, {.w = 0xfb50f5d8},
    {.w = 0xf510f510}, {.w = 0xfa24fb82}, {.w = 0xfb5cf448}, {.w = 0xf510f830}, {.w = 0xf380fb50},
    {.w = 0xfb50fa88}, {.w = 0xfe70f448}, {.w = 0xf380fb50}, {.w = 0xfb5000c8}, {.w = 0xfed4f5d8},
    {.w = 0xf768fb50}, {.w = 0xfb1ef0c4}, {.w = 0xf1f002bc}, {.w = 0xfaecfaba}, {.w = 0xfb1ef1f0},
    {.w = 0xf5d801f4}, {.w = 0x02bcfb34}, {.w = 0xfb1ef448}, {.w = 0xf2b804b0}, {.w = 0x02bcfb1e},
    {.w = 0xf66e0d48}, {.w = 0x0af0fd44}, {.w = 0xfe0cf66e}, {.w = 0xf66e0898}, {.w = 0x0898fce0},
    {.w = 0xfed4f66e}, {.w = 0xf66e0b54}, {.w = 0x0d48ff38}, {.w = 0xfed4f66e}};

#pragma section sRun end
