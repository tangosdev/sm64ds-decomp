#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    void *f004;
} data_ov055_02111958_Layout;
typedef char data_ov055_02111958_size[sizeof(data_ov055_02111958_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov055_02111960_Layout;
typedef char data_ov055_02111960_size[sizeof(data_ov055_02111960_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov055_02111968_Layout;
typedef char data_ov055_02111968_size[sizeof(data_ov055_02111968_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov055_02111978_Layout;
typedef char data_ov055_02111978_size[sizeof(data_ov055_02111978_Layout) == 0x1c ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    u32 f008;
    u32 f00c;
    void *f010;
    u32 f014;
    u32 f018;
} data_ov055_02111994_Layout;
typedef char data_ov055_02111994_size[sizeof(data_ov055_02111994_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov055_021119b0_Layout;
typedef char data_ov055_021119b0_size[sizeof(data_ov055_021119b0_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov055_021119d0_Layout;
typedef char data_ov055_021119d0_size[sizeof(data_ov055_021119d0_Layout) == 0x30 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    void *f00c;
    u32 f010;
    void *f014;
    u32 f018;
    void *f01c;
    u32 f020;
    void *f024;
    u32 f028;
    void *f02c;
} data_ov055_02111a00_Layout;
typedef char data_ov055_02111a00_size[sizeof(data_ov055_02111a00_Layout) == 0x30 ? 1 : -1];

extern data_ov055_02111958_Layout data_ov055_02111958;
extern data_ov055_02111960_Layout data_ov055_02111960;
extern data_ov055_02111968_Layout data_ov055_02111968;
extern data_ov055_02111978_Layout data_ov055_02111978;
extern data_ov055_02111994_Layout data_ov055_02111994;
extern data_ov055_021119b0_Layout data_ov055_021119b0;
extern data_ov055_021119d0_Layout data_ov055_021119d0;
extern data_ov055_02111a00_Layout data_ov055_02111a00;
extern u8 data_ov055_02111a30[];
extern u8 data_ov055_02111938[];
extern u8 data_ov055_02111940[];
extern u8 data_ov055_0211194c[];
extern u8 data_ov055_02111928[];
extern u8 data_ov055_02111920[];
extern u8 data_ov055_02111924[];

#pragma define_section sName ".data.lgPre"                                                         \
                             ".data.lgPre"                                                         \
                             ".data.lgPre" far_data
#pragma section sName begin

data_ov055_02111960_Layout data_ov055_02111960 = {
    0x00000c05,
    &data_ov055_02111a30[0],
};

data_ov055_02111958_Layout data_ov055_02111958 = {
    0x00000300,
    &data_ov055_021119d0,
};

data_ov055_02111968_Layout data_ov055_02111968 = {{
    0xfa240000,
    0x000006a4,
    0x40000000,
    0x03000000,
}};

data_ov055_02111978_Layout data_ov055_02111978 = {{
    0xfb50ff02,
    0x0064060e,
    0xaaaa0000,
    0x00060000,
    0x0140f704,
    0x0000fd44,
    0x00004000,
}};

data_ov055_02111994_Layout data_ov055_02111994 = {
    &data_ov055_021119b0, &data_ov055_02111938[0], 0x071e0720,
    0x07220721,           &data_ov055_02111940[0], 0x02260001,
    0x0000000f,
};

data_ov055_021119b0_Layout data_ov055_021119b0 = {{
    0x53504c43,
    0x00030008,
    0x00030fc4,
    0x000000ff,
    0x00038044,
    0x000000ff,
    0x00000fc0,
    0x000000ff,
}};

data_ov055_021119d0_Layout data_ov055_021119d0 = {{
    0x01900044,
    0x03840000,
    0xc0000000,
    0xff030000,
    0x02540142,
    0x01a60000,
    0xc0000000,
    0xffff0000,
    0xf8f800f4,
    0x00000190,
    0x40000000,
    0x00000000,
}};

data_ov055_02111a00_Layout data_ov055_02111a00 = {
    0x00000202, &data_ov055_0211194c[0], 0x00000103, &data_ov055_02111928[0],
    0x00000204, &data_ov055_02111978,    0x0000010b, &data_ov055_02111920[0],
    0x00000101, &data_ov055_02111968,    0x0000010c, &data_ov055_02111924[0],
};
