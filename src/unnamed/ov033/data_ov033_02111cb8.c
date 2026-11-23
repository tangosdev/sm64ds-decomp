#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    void *f00c;
} data_ov033_02111cb8_Layout;
typedef char data_ov033_02111cb8_size[sizeof(data_ov033_02111cb8_Layout) == 0x10 ? 1 : -1];

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
} data_ov033_02111cc8_Layout;
typedef char data_ov033_02111cc8_size[sizeof(data_ov033_02111cc8_Layout) == 0x40 ? 1 : -1];

typedef struct {
    u32 w[25];
} data_ov033_02111d08_Layout;
typedef char data_ov033_02111d08_size[sizeof(data_ov033_02111d08_Layout) == 0x64 ? 1 : -1];

typedef struct {
    u32 w[26];
} data_ov033_02111d6c_Layout;
typedef char data_ov033_02111d6c_size[sizeof(data_ov033_02111d6c_Layout) == 0x68 ? 1 : -1];

typedef struct {
    u32 w[28];
} data_ov033_02111dd4_Layout;
typedef char data_ov033_02111dd4_size[sizeof(data_ov033_02111dd4_Layout) == 0x70 ? 1 : -1];

typedef struct {
    u32 w[40];
} data_ov033_02111e44_Layout;
typedef char data_ov033_02111e44_size[sizeof(data_ov033_02111e44_Layout) == 0xa0 ? 1 : -1];

typedef struct {
    u32 w[46];
} data_ov033_02111ee4_Layout;
typedef char data_ov033_02111ee4_size[sizeof(data_ov033_02111ee4_Layout) == 0xb8 ? 1 : -1];

extern data_ov033_02111cb8_Layout data_ov033_02111cb8;
extern data_ov033_02111cc8_Layout data_ov033_02111cc8;
extern data_ov033_02111d08_Layout data_ov033_02111d08;
extern data_ov033_02111d6c_Layout data_ov033_02111d6c;
extern data_ov033_02111dd4_Layout data_ov033_02111dd4;
extern data_ov033_02111e44_Layout data_ov033_02111e44;
extern data_ov033_02111ee4_Layout data_ov033_02111ee4;
extern u8 data_ov033_02111b9c[];
extern u8 data_ov033_02111ae0[];
extern u8 data_ov033_02111af4[];
extern u8 data_ov033_02111b04[];
extern u8 data_ov033_02111ae4[];
extern u8 data_ov033_02111ae8[];
extern u8 data_ov033_02111c3c[];

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

data_ov033_02111cb8_Layout data_ov033_02111cb8 = {
    0x00001102,
    &data_ov033_02111d6c,
    0x00000303,
    &data_ov033_02111b9c[0],
};

data_ov033_02111cc8_Layout data_ov033_02111cc8 = {
    0x00000704, &data_ov033_02111d08,    0x0000010b, &data_ov033_02111ae0[0],
    0x00000701, &data_ov033_02111dd4,    0x00000106, &data_ov033_02111af4[0],
    0x00000107, &data_ov033_02111b04[0], 0x0000010c, &data_ov033_02111ae4[0],
    0x0000010e, &data_ov033_02111ae8[0], 0x0000030a, &data_ov033_02111c3c[0],
};

data_ov033_02111d08_Layout data_ov033_02111d08 = {{
    0x0000ff00, 0xffce0514, 0x00000000, 0xff020000, 0xfdd9f510, 0x00000898, 0x00004000,
    0x0000ff07, 0xffcefbe6, 0x00000000, 0xff020000, 0xfe70f768, 0x0000f8f8, 0x00002aaa,
    0x09c4ff02, 0xf8f8ff6a, 0xd1c80000, 0xff020000, 0x012cfa24, 0x0000fa88, 0x0000238e,
    0xfc18ff02, 0xf63cfed4, 0xeaab0000, 0x00000000,
}};

data_ov033_02111d6c_Layout data_ov033_02111d6c = {{
    0x0050fab5, 0xfa79fd87, 0xfd52001c, 0x0034f9c0, 0xfa88fa88, 0xfa240003, 0xff9afbd9,
    0xfe12fa6a, 0xfa6afe94, 0xfe2000d1, 0x032dfa6a, 0xfa6afda8, 0xfd9903b3, 0x0000fa6a,
    0x03840384, 0x0384fc68, 0x0000ff6a, 0xee6cfe5c, 0x04b00514, 0xfc18fe0c, 0x02bc00c8,
    0x00c8fc18, 0x000005dc, 0x064000c8, 0x00c80190, 0x000002bc,
}};

data_ov033_02111dd4_Layout data_ov033_02111dd4 = {{
    0xf7cc0000, 0x076c033e, 0x5c710000, 0x05080000, 0xf8f80000, 0x076c033e, 0x5c710000,
    0x05080000, 0xf7cc0000, 0x0898033e, 0x5c710000, 0x05080000, 0xf8f80000, 0x0898033e,
    0x5c710000, 0x05080000, 0xf9340000, 0xf90cfda8, 0x00000000, 0x06980000, 0xfc200000,
    0xfb7300a0, 0xe38f0000, 0x06a80000, 0x076c0000, 0xf95cfe0c, 0x238e0000, 0x06a00000,
}};

data_ov033_02111e44_Layout data_ov033_02111e44 = {{
    0x53504c43, 0x00100008, 0x00240fc0, 0x000000ff, 0x01000fc0, 0x000000ff, 0x00000fe3, 0x000000ff,
    0x00040008, 0x000000ff, 0x00744004, 0x000000ff, 0x00740004, 0x000000ff, 0x00040004, 0x000000ff,
    0x00042004, 0x000000ff, 0x00730002, 0x000000ff, 0x00744002, 0x000000ff, 0x00030002, 0x000000ff,
    0x00040002, 0x000000ff, 0x00050005, 0x000000ff, 0x00030fc5, 0x000000ff, 0x00040005, 0x000000ff,
    0x00000005, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
}};

data_ov033_02111ee4_Layout data_ov033_02111ee4 = {{
    0xfe88e225, 0x017cfe1b, 0xffc0e225, 0x017cfe1d, 0xfa79e225, 0xffb4ff6a, 0xfc18e225, 0x0384ff6a,
    0xff78e225, 0xfa4efe20, 0x003ce225, 0xfa4efe20, 0x067ce225, 0xfc18fe13, 0x067ce225, 0xfcaefe00,
    0x067ce225, 0xfd44fded, 0x067ce225, 0xfddafdda, 0x067ce225, 0xfe70fdc8, 0x012ce225, 0xf02efd12,
    0x058cfe29, 0x0294ff6a, 0xf92a5e37, 0x073afcae, 0x00c80a36, 0x0834fbfa, 0x00dee225, 0x031dff74,
    0xfef2e225, 0x00820285, 0xff88e225, 0x00820285, 0x001ee225, 0x00820285, 0x00b4e225, 0x00820285,
    0x014ae225, 0x00820285, 0xf84efe29, 0x0118fcaa, 0xf84ee225, 0x0118ff06,
}};
