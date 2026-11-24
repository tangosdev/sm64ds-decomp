#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef union {
    u32 w;
    void *p;
} Word;

extern Word data_ov017_02111a30[];
extern Word data_ov017_02111a00[];
extern Word data_ov017_02111a40[];
extern Word data_ov017_02111a04[];
extern Word data_ov017_02111ab4[];
extern Word data_ov017_02111ad4[];
extern Word data_ov017_02111afc[];
extern Word data_ov017_02111b2c[];
extern Word data_ov017_02111b6c[];

#pragma define_section sRun ".data.ksRest"                                                         \
                            ".data.ksRest"                                                         \
                            ".data.ksRest" far_data
#pragma section sRun begin

Word data_ov017_02111ab4[8] = {{.w = 0x53504c43}, {.w = 0x00020008}, {.w = 0x00030fc0},
                               {.w = 0x000000ff}, {.w = 0x00030fc5}, {.w = 0x000000ff},
                               {.w = 0xffffffff}, {.w = 0xffffffff}};

Word data_ov017_02111ad4[10] = {
    {.w = 0x00000104}, {.p = data_ov017_02111a30}, {.w = 0x0000010b}, {.p = data_ov017_02111a00},
    {.w = 0x00000401}, {.p = data_ov017_02111b2c}, {.w = 0x00000208}, {.p = data_ov017_02111a40},
    {.w = 0x0000010e}, {.p = data_ov017_02111a04}};

Word data_ov017_02111afc[12] = {{.w = 0x0000001f}, {.w = 0x0b5405c6}, {.w = 0x00000000},
                                {.w = 0x01010000}, {.w = 0x00000124}, {.w = 0xf862fea9},
                                {.w = 0x00000000}, {.w = 0xff010000}, {.w = 0x000000b9},
                                {.w = 0x00000809}, {.w = 0x00000000}, {.w = 0xffff0000}};

Word data_ov017_02111b2c[16] = {
    {.w = 0x03840000}, {.w = 0xfa880374}, {.w = 0x80000000}, {.w = 0x00800000},
    {.w = 0x028a0000}, {.w = 0xfa880374}, {.w = 0x80000000}, {.w = 0x00800000},
    {.w = 0x047e0000}, {.w = 0xfa880374}, {.w = 0x80000000}, {.w = 0x00800000},
    {.w = 0x05460000}, {.w = 0xfa880374}, {.w = 0x80000000}, {.w = 0x00800000}};

Word data_ov017_02111b6c[18] = {
    {.w = 0x0000423f}, {.w = 0x0b5405c6}, {.w = 0x012ce225}, {.w = 0xf574feac}, {.w = 0xff06e225},
    {.w = 0x02bcffba}, {.w = 0x0226e225}, {.w = 0xf5d8feac}, {.w = 0x02eee225}, {.w = 0x0a5a039d},
    {.w = 0x0320e225}, {.w = 0xf63cfeac}, {.w = 0x0352e225}, {.w = 0x03b6ffdd}, {.w = 0x00fae225},
    {.w = 0x060e013b}, {.w = 0xff06e225}, {.w = 0x076c02a8}};

#pragma section sRun end
