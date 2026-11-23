#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov012_021120f8_Layout;
typedef char data_ov012_021120f8_size[sizeof(data_ov012_021120f8_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[5];
} data_ov012_02112104_Layout;
typedef char data_ov012_02112104_size[sizeof(data_ov012_02112104_Layout) == 0x14 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov012_02112118_Layout;
typedef char data_ov012_02112118_size[sizeof(data_ov012_02112118_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov012_02112138_Layout;
typedef char data_ov012_02112138_size[sizeof(data_ov012_02112138_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov012_02112158_Layout;
typedef char data_ov012_02112158_size[sizeof(data_ov012_02112158_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov012_02112178_Layout;
typedef char data_ov012_02112178_size[sizeof(data_ov012_02112178_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov012_02112198_Layout;
typedef char data_ov012_02112198_size[sizeof(data_ov012_02112198_Layout) == 0x20 ? 1 : -1];

extern data_ov012_021120f8_Layout data_ov012_021120f8;
extern data_ov012_02112104_Layout data_ov012_02112104;
extern data_ov012_02112118_Layout data_ov012_02112118;
extern data_ov012_02112138_Layout data_ov012_02112138;
extern data_ov012_02112158_Layout data_ov012_02112158;
extern data_ov012_02112178_Layout data_ov012_02112178;
extern data_ov012_02112198_Layout data_ov012_02112198;

#pragma define_section sName ".data.c0Rest"                                                        \
                             ".data.c0Rest"                                                        \
                             ".data.c0Rest" far_data
#pragma section sName begin

data_ov012_021120f8_Layout data_ov012_021120f8 = {{
    0xff380109,
    0xfd9efcae,
    0x80000000,
}};

data_ov012_02112104_Layout data_ov012_02112104 = {{
    0xffff0000,
    0xfa2400ed,
    0xf4acfb82,
    0x00000000,
    0x04010000,
}};

data_ov012_02112138_Layout data_ov012_02112138 = {{
    0xfb82f894,
    0xfa88f9c0,
    0xf9c0fb7e,
    0xfb50fa88,
    0xf510f448,
    0xf448fb50,
    0xfb82f510,
    0xf4acfaec,
}};

data_ov012_02112158_Layout data_ov012_02112158 = {{
    0xfaecfb82,
    0xfb50f4ac,
    0xfed4f3e4,
    0xf3e4fb50,
    0xfb50fed4,
    0xf8f8fa88,
    0xfa88fb82,
    0xfb82f8f8,
}};

data_ov012_02112118_Layout data_ov012_02112118 = {{
    0xfaec00ed,
    0xf4acfb82,
    0x00000000,
    0x03020000,
    0xf1f000ed,
    0xfaecfaba,
    0xe0000000,
    0x22030000,
}};

data_ov012_02112178_Layout data_ov012_02112178 = {{
    0xf12802bc,
    0x02bcfb1e,
    0xfabaf128,
    0xf18cfaec,
    0xfaecfaba,
    0xfb1ef18c,
    0xf8940258,
    0x0258fb82,
}};

data_ov012_02112198_Layout data_ov012_02112198 = {{
    0xfb50f574,
    0xfa24f4ac,
    0xf4acfb50,
    0xfb82fa24,
    0xf894f9c0,
    0xf9c0fb82,
    0xfb82f894,
    0xf1f001f4,
}};
