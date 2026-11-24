#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef union {
    u32 w;
    void *p;
} Word;

extern Word data_ov038_021111c0[];
extern Word data_ov038_021111c4[];
extern Word data_ov038_021111c8[];
extern Word data_ov038_021111cc[];
extern Word data_ov038_021111d4[];
extern Word data_ov038_021111dc[];
extern Word data_ov038_021111e8[];
extern Word data_ov038_021111f8[];
extern Word data_ov038_02111208[];
extern Word data_ov038_021112e4[];

Word data_ov038_021111c8[1] = {{.w = 0x00000004}};

Word data_ov038_021111c4[1] = {{.w = 0x000007c8}};

Word data_ov038_021111c0[1] = {{.w = 0x00000c80}};

Word data_ov038_021111d4[2] = {{.w = 0x00000006}, {.p = data_ov038_02111208 + 27}};

Word data_ov038_021111cc[2] = {{.w = 0x00000002}, {.p = data_ov038_02111208}};

Word data_ov038_021111dc[3] = {{.p = data_ov038_021111cc}, {.w = 0x00000000}, {.w = 0x000a0000}};

Word data_ov038_021111f8[4] = {
    {.w = 0xffffff00}, {.w = 0x4e202710}, {.w = 0xc0700001}, {.w = 0x94700000}};

Word data_ov038_021111e8[4] = {
    {.w = 0x0000ff02}, {.w = 0x0000fed4}, {.w = 0x00000000}, {.w = 0x00000000}};

Word data_ov038_02111208[55] = {{.w = 0x00000200},
                                {.p = data_ov038_02111208 + 11},
                                {.w = 0x00004105},
                                {.p = data_ov038_021112e4},
                                {.p = data_ov038_02111208 + 19},
                                {.p = data_ov038_021111d4},
                                {.w = 0x07c707c9},
                                {.w = 0x07cb07ca},
                                {.p = data_ov038_021111dc},
                                {.w = 0x03200001},
                                {.w = 0x0000002f},
                                {.w = 0x00000031},
                                {.w = 0x0000f1f0},
                                {.w = 0x00000000},
                                {.w = 0x00f80000},
                                {.w = 0x00000031},
                                {.w = 0x0000f1f0},
                                {.w = 0x00000000},
                                {.w = 0x00170000},
                                {.w = 0x53504c43},
                                {.w = 0x00020008},
                                {.w = 0x00040fc0},
                                {.w = 0x000000ff},
                                {.w = 0x00040fe0},
                                {.w = 0x000000ff},
                                {.w = 0xffffffff},
                                {.w = 0xffffffff},
                                {.w = 0x00000104},
                                {.p = data_ov038_021111e8},
                                {.w = 0x0000010b},
                                {.p = data_ov038_021111c4},
                                {.w = 0x00000401},
                                {.p = data_ov038_02111208 + 39},
                                {.w = 0x00000208},
                                {.p = data_ov038_021111f8},
                                {.w = 0x0000010c},
                                {.p = data_ov038_021111c0},
                                {.w = 0x0000010e},
                                {.p = data_ov038_021111c8},
                                {.w = 0x00000000},
                                {.w = 0x0000fd44},
                                {.w = 0x00000000},
                                {.w = 0x00800000},
                                {.w = 0xfe0c0000},
                                {.w = 0x0000fd44},
                                {.w = 0x00000000},
                                {.w = 0x00800000},
                                {.w = 0xff380000},
                                {.w = 0xfe70fd44},
                                {.w = 0x00000000},
                                {.w = 0x00800000},
                                {.w = 0x015e0000},
                                {.w = 0xfe70fd44},
                                {.w = 0x00000000},
                                {.w = 0x00800000}};

Word data_ov038_021112e4[135] = {
    {.w = 0xf6a02226}, {.w = 0xf6a0ee6c}, {.w = 0xf6a02226}, {.w = 0x08fcee6c}, {.w = 0x09602226},
    {.w = 0x08fcee6c}, {.w = 0x09602226}, {.w = 0xf6a0ee6c}, {.w = 0x0000023f}, {.w = 0x0000ef66},
    {.w = 0x0000e225}, {.w = 0x01f4f1ed}, {.w = 0x0000e225}, {.w = 0xfe0cf1ed}, {.w = 0xfe0ce225},
    {.w = 0x0000f1ed}, {.w = 0x01f4e225}, {.w = 0x0000f1ed}, {.w = 0x015ee225}, {.w = 0x015ef1ed},
    {.w = 0xfea2e225}, {.w = 0x015ef1ed}, {.w = 0xfea2e225}, {.w = 0xfea2f1ed}, {.w = 0x015ee225},
    {.w = 0xfea2f1ed}, {.w = 0x08fc2226}, {.w = 0x0000f7cd}, {.w = 0xf7042226}, {.w = 0x0000f7cc},
    {.w = 0x00002226}, {.w = 0x0898f7c3}, {.w = 0x00002226}, {.w = 0xf704f7cc}, {.w = 0x0000e225},
    {.w = 0xf7ccf6e6}, {.w = 0x0000e225}, {.w = 0xf63cf6e6}, {.w = 0x0000e225}, {.w = 0xf704f8f8},
    {.w = 0x0000e225}, {.w = 0xf704f6a0}, {.w = 0x0000e225}, {.w = 0xf830f7cc}, {.w = 0x0000e225},
    {.w = 0xf7ccf8b3}, {.w = 0x0000e225}, {.w = 0xf63cf8bb}, {.w = 0x0000e225}, {.w = 0xf5d8f7cc},
    {.w = 0x0000e225}, {.w = 0x0960f6d3}, {.w = 0x0000e225}, {.w = 0x07d0f6d3}, {.w = 0x0000e225},
    {.w = 0x0898f8e6}, {.w = 0x0000e225}, {.w = 0x0898f68e}, {.w = 0x0000e225}, {.w = 0x09c4f7ba},
    {.w = 0x0000e225}, {.w = 0x0960f8a0}, {.w = 0x0000e225}, {.w = 0x07d0f8a8}, {.w = 0x0000e225},
    {.w = 0x076cf7ba}, {.w = 0xf830e225}, {.w = 0x0000f7cc}, {.w = 0xf5d8e225}, {.w = 0x0000f7cc},
    {.w = 0xf704e225}, {.w = 0x0000f8f8}, {.w = 0xf704e225}, {.w = 0x0000f6a0}, {.w = 0xf63ce225},
    {.w = 0x0000f6e3}, {.w = 0xf7cce225}, {.w = 0x0000f6de}, {.w = 0xf7cce225}, {.w = 0x0000f8b6},
    {.w = 0xf63ce225}, {.w = 0x0000f8ad}, {.w = 0x0a28e225}, {.w = 0x0000f7cd}, {.w = 0x07d0e225},
    {.w = 0x0000f7cd}, {.w = 0x08fce225}, {.w = 0x0000f8f8}, {.w = 0x08fce225}, {.w = 0x0000f6a1},
    {.w = 0x0834e225}, {.w = 0x0000f6e3}, {.w = 0x09c4e225}, {.w = 0x0000f6de}, {.w = 0x09c4e225},
    {.w = 0x0000f8b6}, {.w = 0x0834e225}, {.w = 0x0000f8ad}, {.w = 0x05530a36}, {.w = 0x02e0f0c7},
    {.w = 0x04c22a36}, {.w = 0x03b5f299}, {.w = 0x04f90a36}, {.w = 0xfa7af398}, {.w = 0x04684a36},
    {.w = 0xfb65f57f}, {.w = 0xf99e0a36}, {.w = 0xfbfef0d2}, {.w = 0xf90d0a36}, {.w = 0xfc9ff28f},
    {.w = 0xfbc72a36}, {.w = 0x062bf0a7}, {.w = 0xfb364a36}, {.w = 0x0700f265}, {.w = 0x05530a36},
    {.w = 0x02edf9ce}, {.w = 0x04c22a36}, {.w = 0x00bcfbbc}, {.w = 0xfbc72a36}, {.w = 0xfa39fab1},
    {.w = 0xfb364a36}, {.w = 0xfb0efc6e}, {.w = 0x04f90a36}, {.w = 0x0687f844}, {.w = 0x04684a36},
    {.w = 0x0772fa2c}, {.w = 0xfbc72a36}, {.w = 0x00f6f774}, {.w = 0xfb364a36}, {.w = 0xfed4f9b9},
    {.w = 0x00000000}, {.w = 0x00000000}, {.w = 0x00000000}, {.w = 0x00000000}, {.w = 0x00000000}};
