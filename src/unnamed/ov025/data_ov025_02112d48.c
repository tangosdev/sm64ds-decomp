#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[8];
} data_ov025_02112d48_Layout;
typedef char data_ov025_02112d48_size[sizeof(data_ov025_02112d48_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov025_02112d68_Layout;
typedef char data_ov025_02112d68_size[sizeof(data_ov025_02112d68_Layout) == 0x30 ? 1 : -1];

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
} data_ov025_02112d98_Layout;
typedef char data_ov025_02112d98_size[sizeof(data_ov025_02112d98_Layout) == 0x40 ? 1 : -1];

typedef struct {
    u32 w[18];
} data_ov025_02112dd8_Layout;
typedef char data_ov025_02112dd8_size[sizeof(data_ov025_02112dd8_Layout) == 0x48 ? 1 : -1];

typedef struct {
    u32 w[20];
} data_ov025_02112e20_Layout;
typedef char data_ov025_02112e20_size[sizeof(data_ov025_02112e20_Layout) == 0x50 ? 1 : -1];

typedef struct {
    u32 w[32];
} data_ov025_02112e70_Layout;
typedef char data_ov025_02112e70_size[sizeof(data_ov025_02112e70_Layout) == 0x80 ? 1 : -1];

typedef struct {
    u32 w[63];
} data_ov025_02112ef0_Layout;
typedef char data_ov025_02112ef0_size[sizeof(data_ov025_02112ef0_Layout) == 0xfc ? 1 : -1];

extern data_ov025_02112d48_Layout data_ov025_02112d48;
extern data_ov025_02112d68_Layout data_ov025_02112d68;
extern data_ov025_02112d98_Layout data_ov025_02112d98;
extern data_ov025_02112dd8_Layout data_ov025_02112dd8;
extern data_ov025_02112e20_Layout data_ov025_02112e20;
extern data_ov025_02112e70_Layout data_ov025_02112e70;
extern data_ov025_02112ef0_Layout data_ov025_02112ef0;
extern u8 data_ov025_02112b6c[];
extern u8 data_ov025_02112b80[];
extern u8 data_ov025_02112b64[];
extern u8 data_ov025_02112b40[];

#pragma define_section sName ".data.dkkTail"                                                       \
                             ".data.dkkTail"                                                       \
                             ".data.dkkTail" far_data
#pragma section sName begin

data_ov025_02112d48_Layout data_ov025_02112d48 = {{
    0x53504c43,
    0x00010008,
    0x01000fc4,
    0x000000ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
}};

data_ov025_02112d68_Layout data_ov025_02112d68 = {{
    0x0374cc3d,
    0x09040546,
    0x00fa0c3e,
    0xfcca1113,
    0xfefb0c3e,
    0xfdb40b69,
    0x00fa0c3e,
    0xfdc40f1f,
    0xf8940c3e,
    0xfd7704e2,
    0xf89e0c3e,
    0x08d904e2,
}};

data_ov025_02112d98_Layout data_ov025_02112d98 = {
    0x00002a02, &data_ov025_02112ef0,    0x00001503, &data_ov025_02112e70,
    0x00000504, &data_ov025_02112dd8,    0x0000040b, &data_ov025_02112b6c[0],
    0x00000501, &data_ov025_02112e20,    0x00000109, &data_ov025_02112b80[0],
    0x0000040c, &data_ov025_02112b64[0], 0x0000010e, &data_ov025_02112b40[0],
};

data_ov025_02112dd8_Layout data_ov025_02112dd8 = {{
    0x0000ff01,
    0x00fa1388,
    0x00000000,
    0xff020000,
    0x0064ff7a,
    0x00001954,
    0x000071c7,
    0x0000ff02,
    0x035213ba,
    0x80000000,
    0x00040000,
    0xf1be0000,
    0x0000fd76,
    0x00008000,
    0xfffdff02,
    0x008e01f7,
    0x80000000,
    0x00000000,
}};

data_ov025_02112e20_Layout data_ov025_02112e20 = {{
    0x00000000, 0x177001a4, 0x80000000, 0x02080000, 0xff380000, 0x170c0000, 0x80000000,
    0x00080000, 0x00000000, 0x170c0000, 0x80000000, 0x00080000, 0x00c80000, 0x170c0000,
    0x80000000, 0x00080000, 0x00000000, 0x00fa170c, 0x00000000, 0x02100000,
}};

data_ov025_02112e70_Layout data_ov025_02112e70 = {{
    0xff020000, 0x00027f02, 0x7f02ff02, 0xff020004, 0x00067f02, 0x7f01ff02, 0xff020008, 0x000a7f01,
    0x7f01ff02, 0xff02000c, 0x000e7f01, 0x7f01ff02, 0xff020010, 0x00127f01, 0x7f01ff02, 0xff020014,
    0x00167f01, 0x7f01ff02, 0xff020018, 0x001a7f01, 0x7f01ff02, 0xff02001c, 0x001e7f01, 0x7f01ff02,
    0xff020020, 0x00227f01, 0x7f01ff02, 0xff020024, 0x00267f01, 0x7f01ff02, 0xff020028, 0x00007f01,
}};

data_ov025_02112ef0_Layout data_ov025_02112ef0 = {{
    0x04b00032, 0xf894fd76, 0xfd7604b0, 0x04b0f894, 0xf894fda8, 0x086604b0, 0x04b0f894, 0x03e808fc,
    0x08fc04b0, 0xff8df95c, 0xf95c03e8, 0x0672fef7, 0xff89f6a0, 0xf8f805aa, 0x06d6feee, 0xff8ff6a0,
    0xf8f80898, 0x073afeee, 0xff8df95c, 0xf95c0a28, 0x079efef7, 0xff8ffc18, 0xf9c00898, 0x073afeee,
    0xff89fc18, 0xf9c005aa, 0x06d6feee, 0xff570000, 0x0000ffce, 0x0190feef, 0xff57fe0c, 0xff9c00fa,
    0x01c2fef0, 0xff59fe0c, 0xff6a0320, 0x0258fefc, 0xff5b0000, 0x0000044c, 0x028afef3, 0xff630226,
    0x00960320, 0x0258fefc, 0xff5701f4, 0x006400fa, 0x01c2fef0, 0xff8206a4, 0x06a4041a, 0x0672fef7,
    0xff7f041a, 0x060e05aa, 0x06a4fefe, 0xff7f041a, 0x060e0866, 0x076cfefe, 0xff8206a4, 0x06a409f6,
    0x079efef7, 0xff85092e, 0x073a0898, 0x076cfefe, 0xff7f092e, 0x073a05aa, 0x06a4fefe,
}};
