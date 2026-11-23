#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov012_021121b8_Layout;
typedef char data_ov012_021121b8_size[sizeof(data_ov012_021121b8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov012_021121c0_Layout;
typedef char data_ov012_021121c0_size[sizeof(data_ov012_021121c0_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov012_021121dc_Layout;
typedef char data_ov012_021121dc_size[sizeof(data_ov012_021121dc_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[24];
} data_ov012_021121f8_Layout;
typedef char data_ov012_021121f8_size[sizeof(data_ov012_021121f8_Layout) == 0x60 ? 1 : -1];

extern data_ov012_021121b8_Layout data_ov012_021121b8;
extern data_ov012_021121c0_Layout data_ov012_021121c0;
extern data_ov012_021121dc_Layout data_ov012_021121dc;
extern data_ov012_021121f8_Layout data_ov012_021121f8;

#pragma define_section sName ".data.c0Rest"                                                        \
                             ".data.c0Rest"                                                        \
                             ".data.c0Rest" far_data
#pragma section sName begin

data_ov012_021121b8_Layout data_ov012_021121b8 = {{
    0x01f4fb1e,
    0xfabaf1f0,
}};

data_ov012_021121c0_Layout data_ov012_021121c0 = {{
    0xf0c4faec,
    0xfaecfaba,
    0xfb1ef0c4,
    0xf95c0320,
    0x0320fb82,
    0xfb82f95c,
    0xfe70faec,
}};

data_ov012_021121dc_Layout data_ov012_021121dc = {{
    0xfaecfb50,
    0xfb50fe70,
    0x0064fd44,
    0xfce0fb50,
    0xfb50ff9c,
    0xff9cfa24,
    0xf830fb50,
}};

data_ov012_021121f8_Layout data_ov012_021121f8 = {{
    0xfb5001f4, 0xff9cf5d8, 0xf3e4fb50, 0xfb50faec, 0xf448f3e4, 0xf3e4fb50, 0xfb82f448, 0xf574fb50,
    0xfb50fb82, 0xfb50fed4, 0xfaecf4ac, 0xf4acfb50, 0xfb82faec, 0xfe0cf9c0, 0xf9c0fb50, 0xfb50ff38,
    0xff9cf894, 0xfc7cfb50, 0xfb50fe0c, 0xf9c0fa88, 0xfa88fb82, 0xfb82f8f8, 0xf18c0258, 0x0320fb1e,
}};
