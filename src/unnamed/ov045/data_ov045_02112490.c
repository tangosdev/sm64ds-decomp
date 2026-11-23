#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} data_ov045_02112490_Layout;
typedef char data_ov045_02112490_size[sizeof(data_ov045_02112490_Layout) == 0x4 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov045_02112494_Layout;
typedef char data_ov045_02112494_size[sizeof(data_ov045_02112494_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov045_0211249c_Layout;
typedef char data_ov045_0211249c_size[sizeof(data_ov045_0211249c_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    void *f00c;
} data_ov045_021124ac_Layout;
typedef char data_ov045_021124ac_size[sizeof(data_ov045_021124ac_Layout) == 0x10 ? 1 : -1];

extern data_ov045_02112490_Layout data_ov045_02112490;
extern data_ov045_02112494_Layout data_ov045_02112494;
extern data_ov045_0211249c_Layout data_ov045_0211249c;
extern data_ov045_021124ac_Layout data_ov045_021124ac;
extern u8 data_ov045_02112470[];
extern u8 data_ov045_021124bc[];
extern u8 data_ov045_02112940[];
extern u8 data_ov045_02112780[];

#pragma define_section sName ".data.w12460"                                                        \
                             ".data.w12460"                                                        \
                             ".data.w12460" far_data
#pragma section sName begin

data_ov045_02112490_Layout data_ov045_02112490 = {
    &data_ov045_02112470[0],
};

data_ov045_02112494_Layout data_ov045_02112494 = {
    &data_ov045_021124bc[0],
    0x14e60000,
};

data_ov045_0211249c_Layout data_ov045_0211249c = {{
    0x0f0a19af,
    0xc00000f5,
    0x00260000,
    0x00002204,
}};

data_ov045_021124ac_Layout data_ov045_021124ac = {
    0x00003700,
    &data_ov045_02112940[0],
    0x00003805,
    &data_ov045_02112780[0],
};
