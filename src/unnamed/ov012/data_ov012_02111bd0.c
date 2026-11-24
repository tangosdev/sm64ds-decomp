#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef union {
    u32 w;
    void *p;
} Word;

extern Word data_ov012_02111cb0[];
extern Word data_ov012_02111bd0[];
extern Word data_ov012_02111bdc[];
extern Word data_ov012_02111be8[];
extern Word data_ov012_02111bf4[];
extern Word data_ov012_02111c04[];
extern Word data_ov012_02111c14[];

#pragma define_section sRun ".data.w11bd0"                                                         \
                            ".data.w11bd0"                                                         \
                            ".data.w11bd0" far_data
#pragma section sRun begin

Word data_ov012_02111bd0[3] = {{.w = 0x08980898}, {.w = 0x08980898}, {.w = 0x00000898}};

Word data_ov012_02111bdc[3] = {{.w = 0x06ff06fe}, {.w = 0x07010700}, {.w = 0x00000702}};

Word data_ov012_02111be8[3] = {{.w = 0x5f74616d}, {.w = 0x65746177}, {.w = 0x00000072}};

Word data_ov012_02111bf4[4] = {
    {.w = 0x0772443f}, {.w = 0xf7a2faec}, {.w = 0x0777e43c}, {.w = 0xf501fb70}};

Word data_ov012_02111c04[4] = {
    {.w = 0x00000200}, {.p = data_ov012_02111cb0}, {.w = 0x00000205}, {.p = data_ov012_02111bf4}};

Word data_ov012_02111c14[4] = {
    {.w = 0x0d16002a}, {.w = 0x079efbe6}, {.w = 0xc0000000}, {.w = 0x27770000}};

#pragma section sRun end
