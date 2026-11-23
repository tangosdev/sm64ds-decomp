#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    u32 f004;
    u32 f008;
    void *f00c;
    u32 f010;
    u32 f014;
    void *f018;
    void *f01c;
    u32 f020;
    void *f024;
    u32 f028;
    u32 f02c;
    void *f030;
    u32 f034;
    u32 f038;
} data_ov012_02111cf0_Layout;
typedef char data_ov012_02111cf0_size[sizeof(data_ov012_02111cf0_Layout) == 0x3c ? 1 : -1];

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
    u32 f030;
    void *f034;
    u32 f038;
    void *f03c;
    u32 f040;
    void *f044;
} data_ov012_02111d2c_Layout;
typedef char data_ov012_02111d2c_size[sizeof(data_ov012_02111d2c_Layout) == 0x48 ? 1 : -1];

typedef struct {
    u32 w[20];
} data_ov012_02111d74_Layout;
typedef char data_ov012_02111d74_size[sizeof(data_ov012_02111d74_Layout) == 0x50 ? 1 : -1];

typedef struct {
    u32 w[21];
} data_ov012_02111dc4_Layout;
typedef char data_ov012_02111dc4_size[sizeof(data_ov012_02111dc4_Layout) == 0x54 ? 1 : -1];

typedef struct {
    u32 w[24];
} data_ov012_02111e18_Layout;
typedef char data_ov012_02111e18_size[sizeof(data_ov012_02111e18_Layout) == 0x60 ? 1 : -1];

typedef struct {
    u32 w[24];
} data_ov012_02111e78_Layout;
typedef char data_ov012_02111e78_size[sizeof(data_ov012_02111e78_Layout) == 0x60 ? 1 : -1];

typedef struct {
    u32 w[28];
} data_ov012_02111ed8_Layout;
typedef char data_ov012_02111ed8_size[sizeof(data_ov012_02111ed8_Layout) == 0x70 ? 1 : -1];

typedef struct {
    u32 w[30];
} data_ov012_02111f48_Layout;
typedef char data_ov012_02111f48_size[sizeof(data_ov012_02111f48_Layout) == 0x78 ? 1 : -1];

extern data_ov012_02111cf0_Layout data_ov012_02111cf0;
extern data_ov012_02111d2c_Layout data_ov012_02111d2c;
extern data_ov012_02111d74_Layout data_ov012_02111d74;
extern data_ov012_02111dc4_Layout data_ov012_02111dc4;
extern data_ov012_02111e18_Layout data_ov012_02111e18;
extern data_ov012_02111e78_Layout data_ov012_02111e78;
extern data_ov012_02111ed8_Layout data_ov012_02111ed8;
extern data_ov012_02111f48_Layout data_ov012_02111f48;
extern u8 data_ov012_02111bbc[];
extern u8 data_ov012_02111b94[];
extern u8 data_ov012_02111c3c[];
extern u8 data_ov012_02111bb4[];
extern u8 data_ov012_02111b9c[];
extern u8 data_ov012_02112138[];
extern u8 data_ov012_02111c70[];
extern u8 data_ov012_02111bdc[];
extern u8 data_ov012_02111bd0[];
extern u8 data_ov012_02111b80[];

#pragma define_section sName ".data.c0Rest"                                                        \
                             ".data.c0Rest"                                                        \
                             ".data.c0Rest" far_data
#pragma section sName begin

data_ov012_02111cf0_Layout data_ov012_02111cf0 = {
    0x00000000,
    0x00000000,
    0x012c0200,
    &data_ov012_02111bbc[0],
    0x00000000,
    0xff060101,
    &data_ov012_02111b94[0],
    &data_ov012_02111c3c[0],
    0xfe3e0202,
    &data_ov012_02111bb4[0],
    0x00000000,
    0xff060203,
    &data_ov012_02111b9c[0],
    0x00000000,
    0xfea20204,
};

data_ov012_02111d2c_Layout data_ov012_02111d2c = {
    0x00004c02, &data_ov012_02112138[0], 0x00000503, &data_ov012_02111c70[0],
    0x00000604, &data_ov012_02111dc4,    0x0000050b, &data_ov012_02111bdc[0],
    0x00000b01, &data_ov012_02111f48,    0x00000809, &data_ov012_02111e78,
    0x0000050c, &data_ov012_02111bd0[0], 0x0000010e, &data_ov012_02111b80[0],
    0x0000080a, &data_ov012_02111ed8,
};

data_ov012_02111d74_Layout data_ov012_02111d74 = {{
    0x1af400e4, 0xfce0fb82, 0xc0000000, 0xffff0000, 0x1af400e4, 0xff38fb82, 0xc0000000,
    0xffff0000, 0x1130012f, 0xfe3efb50, 0x00000000, 0xffff0000, 0x186a004b, 0xfb2bfbff,
    0x0000ffff, 0x070f0000, 0x0af000ed, 0xfe0cf66e, 0xc0000000, 0x11040000,
}};

data_ov012_02111dc4_Layout data_ov012_02111dc4 = {{
    0xfc18ff02, 0x03e8fcae, 0x00000000, 0xff020000, 0xfce00708, 0x0000079e, 0x00004000,
    0xf9f2ff02, 0xf47afc4a, 0x75550000, 0xff020000, 0xfc4af5a6, 0x0000f7fe, 0x0000f8e4,
    0x1db0ff02, 0xfe0cfb1e, 0xc0000000, 0xff020000, 0xfc4a0ea6, 0x0000f4de, 0x0000ce39,
}};

data_ov012_02111e78_Layout data_ov012_02111e78 = {{
    0xfbe6012d, 0xc000079d, 0x000a0100, 0xfbe6fb82, 0x00000384, 0x00060000, 0xfbe6fcae, 0x80000384,
    0x00060000, 0xfb1ef3e6, 0x00000577, 0x000f0200, 0xfd12fe70, 0x000000c8, 0x000f0300, 0xfb8202bc,
    0xc000fc63, 0x000f2300, 0xf9f21e14, 0xc000fdc1, 0x00103300, 0xfbe6044c, 0xc000f58d, 0x00022400,
}};

data_ov012_02111e18_Layout data_ov012_02111e18 = {{
    0x53504c43, 0x000b0008, 0x04000fc0, 0x000000ff, 0x00000fc0, 0x000000ff, 0x00040fc0, 0x000000ff,
    0x01040fc5, 0x000000ff, 0x01043fc5, 0x000000ff, 0x00040fc4, 0x000000ff, 0x00042fc4, 0x000000ff,
    0x00042fc0, 0x000000ff, 0x00040fe0, 0x000000ff, 0x01040fc4, 0x000000ff, 0x00044fc4, 0x000000ff,
}};

data_ov012_02111ed8_Layout data_ov012_02111ed8 = {{
    0xfbe6fbb4, 0x00000320, 0x0c020000, 0xfaba20ff, 0xf0f6fbb4, 0x00000000, 0x5504000e,
    0xf9f21de2, 0x0000fe0c, 0x0501c000, 0xf4de21ff, 0xfbd2fbe6, 0x80000000, 0x43060010,
    0xfa560974, 0xc000f768, 0x000d0000, 0x0d167707, 0x079efbe6, 0xc0000000, 0x77080012,
    0xfb5a0fd2, 0xc0000802, 0x00250000, 0xfc7c1109, 0x0320fbe6, 0x00000000, 0x20ff1202,
}};

data_ov012_02111f48_Layout data_ov012_02111f48 = {{
    0xfbe60000, 0x04e2fbe6, 0x00000000, 0x07000000, 0xfcae0000, 0x0514fbe6, 0x00000000, 0x00000000,
    0xfb820000, 0x0514fbe6, 0x00000000, 0x00000000, 0xfa560000, 0x0514fbe6, 0x00000000, 0x00000000,
    0xfaba0000, 0xf0f6fce0, 0x00000000, 0x05920000, 0x1c520000, 0xfe0cf9f2, 0xc0000000, 0x07230000,
    0xf4de0000, 0xfbd2fce0, 0x80000000, 0x059a0000, 0x09740000, 0xf5d8f9ac,
}};
