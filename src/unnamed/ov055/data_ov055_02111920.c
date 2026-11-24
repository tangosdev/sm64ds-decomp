#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef union {
    u32 w;
    void *p;
} Word;

extern Word data_ov055_02111920[];
extern Word data_ov055_02111924[];
extern Word data_ov055_02111928[];
extern Word data_ov055_02111930[];
extern Word data_ov055_02111938[];
extern Word data_ov055_02111940[];
extern Word data_ov055_0211194c[];
extern Word data_ov055_02111958[];
extern Word data_ov055_02111968[];
extern Word data_ov055_02111978[];

#pragma define_section sRun ".data.lgPre"                                                          \
                            ".data.lgPre"                                                          \
                            ".data.lgPre" far_data
#pragma section sRun begin

Word data_ov055_02111920[1] = {{.w = 0x0000071f}};

Word data_ov055_02111924[1] = {{.w = 0x00000a28}};

Word data_ov055_02111930[2] = {{.w = 0x00000002}, {.p = data_ov055_02111958}};

Word data_ov055_02111938[2] = {{.w = 0x00000006}, {.p = data_ov055_02111978 + 34}};

Word data_ov055_02111928[2] = {{.w = 0xff020000}, {.w = 0x00000000}};

Word data_ov055_02111940[3] = {{.p = data_ov055_02111930}, {.w = 0x00000000}, {.w = 0x07080300}};

Word data_ov055_0211194c[3] = {{.w = 0x012cf704}, {.w = 0xf704fc81}, {.w = 0x037f012c}};

Word data_ov055_02111958[4] = {{.w = 0x00000300},
                               {.p = data_ov055_02111978 + 22},
                               {.w = 0x00000c05},
                               {.p = data_ov055_02111978 + 46}};

Word data_ov055_02111968[4] = {
    {.w = 0xfa240000}, {.w = 0x000006a4}, {.w = 0x40000000}, {.w = 0x03000000}};

Word data_ov055_02111978[70] = {{.w = 0xfb50ff02},          {.w = 0x0064060e},
                                {.w = 0xaaaa0000},          {.w = 0x00060000},
                                {.w = 0x0140f704},          {.w = 0x0000fd44},
                                {.w = 0x00004000},          {.p = data_ov055_02111978 + 14},
                                {.p = data_ov055_02111938}, {.w = 0x071e0720},
                                {.w = 0x07220721},          {.p = data_ov055_02111940},
                                {.w = 0x02260001},          {.w = 0x0000000f},
                                {.w = 0x53504c43},          {.w = 0x00030008},
                                {.w = 0x00030fc4},          {.w = 0x000000ff},
                                {.w = 0x00038044},          {.w = 0x000000ff},
                                {.w = 0x00000fc0},          {.w = 0x000000ff},
                                {.w = 0x01900044},          {.w = 0x03840000},
                                {.w = 0xc0000000},          {.w = 0xff030000},
                                {.w = 0x02540142},          {.w = 0x01a60000},
                                {.w = 0xc0000000},          {.w = 0xffff0000},
                                {.w = 0xf8f800f4},          {.w = 0x00000190},
                                {.w = 0x40000000},          {.w = 0x00000000},
                                {.w = 0x00000202},          {.p = data_ov055_0211194c},
                                {.w = 0x00000103},          {.p = data_ov055_02111928},
                                {.w = 0x00000204},          {.p = data_ov055_02111978},
                                {.w = 0x0000010b},          {.p = data_ov055_02111920},
                                {.w = 0x00000101},          {.p = data_ov055_02111968},
                                {.w = 0x0000010c},          {.p = data_ov055_02111924},
                                {.w = 0xfaece225},          {.w = 0xfaba0000},
                                {.w = 0xfbb4e225},          {.w = 0xfaba0000},
                                {.w = 0xfaece225},          {.w = 0x05460000},
                                {.w = 0xfbb4e225},          {.w = 0x05460000},
                                {.w = 0xfc7ce225},          {.w = 0xfaba0000},
                                {.w = 0xfd44e225},          {.w = 0xfaba0000},
                                {.w = 0xfe0ce225},          {.w = 0xfaba0000},
                                {.w = 0xfc7ce225},          {.w = 0x05460000},
                                {.w = 0xfd44e225},          {.w = 0x05460000},
                                {.w = 0xfe0ce225},          {.w = 0x05460000},
                                {.w = 0xfed4e225},          {.w = 0xfaba0000},
                                {.w = 0xfed4e225},          {.w = 0x05460000}};

#pragma section sRun end
