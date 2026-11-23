#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    u32 f00c;
    u32 f010;
    u32 f014;
    u32 f018;
} data_ov006_0212e5a8_Layout;
typedef char data_ov006_0212e5a8_size[sizeof(data_ov006_0212e5a8_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[11];
} data_ov006_0212e5c4_Layout;
typedef char data_ov006_0212e5c4_size[sizeof(data_ov006_0212e5c4_Layout) == 0x2c ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    u32 f00c;
    u32 f010;
    u32 f014;
    u32 f018;
    u32 f01c;
    u32 f020;
    u32 f024;
    u32 f028;
    u32 f02c;
    u32 f030;
    u32 f034;
    u32 f038;
} data_ov006_0212e5f0_Layout;
typedef char data_ov006_0212e5f0_size[sizeof(data_ov006_0212e5f0_Layout) == 0x3c ? 1 : -1];

typedef struct {
    u32 w[120];
} data_ov006_0212e62c_Layout;
typedef char data_ov006_0212e62c_size[sizeof(data_ov006_0212e62c_Layout) == 0x1e0 ? 1 : -1];

extern data_ov006_0212e5a8_Layout data_ov006_0212e5a8;
extern data_ov006_0212e5c4_Layout data_ov006_0212e5c4;
extern data_ov006_0212e5f0_Layout data_ov006_0212e5f0;
extern data_ov006_0212e62c_Layout data_ov006_0212e62c;
extern u8 data_ov006_0213c75c[];
extern u8 data_ov006_0213c6bc[];

#pragma define_section sName ".rodata.gap2"                                                        \
                             ".rodata.gap2"                                                        \
                             ".rodata.gap2" far_data
#pragma section sName begin

data_ov006_0212e5a8_Layout data_ov006_0212e5a8 = {
    0x0000ffff, &data_ov006_0213c75c[0], 0x00000001, 0x00000001, 0x00000001, 0x00000078, 0x00000001,
};

data_ov006_0212e5c4_Layout data_ov006_0212e5c4 = {{
    0x0703001f,
    0x14110d0a,
    0x1f1e1b18,
    0x1c1d1e1f,
    0x18191a1b,
    0x14151617,
    0x10111213,
    0x0c0d0e0f,
    0x08090a0b,
    0x04050607,
    0x00000203,
}};

data_ov006_0212e5f0_Layout data_ov006_0212e5f0 = {
    0x0000ffff, &data_ov006_0213c6bc[0],
    0x00000001, 0x00000001,
    0x00000001, 0x00000001,
    0x00000001, 0x00000001,
    0x00010001, 0x00010001,
    0x00010001, 0x00010001,
    0x00010001, 0x00010001,
    0x00020101,
};

data_ov006_0212e62c_Layout data_ov006_0212e62c = {{
    0x00000000, 0xffffffe6, 0xffffffc4, 0xffffffa2, 0xffffff80, 0xffffff5e, 0xffffff3b, 0xffffff19,
    0xfffffef7, 0xfffffed5, 0xfffffeb3, 0xfffffe90, 0xfffffe6e, 0xfffffe4c, 0xfffffe2a, 0xfffffe07,
    0xfffffde5, 0xfffffdc3, 0xfffffda1, 0xfffffd7f, 0xfffffd5c, 0xfffffd3a, 0xfffffd18, 0xfffffcf6,
    0xfffffcd4, 0xfffffcb1, 0xfffffc8f, 0xfffffc6d, 0xfffffc4b, 0xfffffc29, 0xfffffc06, 0xfffffbe4,
    0xfffffbc2, 0xfffffba0, 0xfffffb7e, 0xfffffb5b, 0xfffffb39, 0xfffffb17, 0xfffffaf5, 0xfffffad3,
    0xfffffab0, 0xfffffa8e, 0xfffffa6c, 0xfffffa4a, 0xfffffa28, 0xfffffa05, 0xfffff9e3, 0xfffff9c1,
    0xfffff99f, 0xfffff97d, 0xfffff95a, 0xfffff938, 0xfffff916, 0xfffff8f4, 0xfffff8d2, 0xfffff8af,
    0xfffff88d, 0xfffff86b, 0xfffff849, 0xfffff826, 0xfffff804, 0xfffff7e2, 0xfffff7c0, 0xfffff79e,
    0xfffff77b, 0xfffff759, 0xfffff737, 0xfffff715, 0xfffff6f3, 0xfffff6d0, 0xfffff6ae, 0xfffff68c,
    0xfffff66a, 0xfffff648, 0xfffff625, 0xfffff603, 0xfffff5e1, 0xfffff5bf, 0xfffff59d, 0xfffff57a,
    0xfffff558, 0xfffff536, 0xfffff514, 0xfffff4f2, 0xfffff4cf, 0xfffff4ad, 0xfffff48b, 0xfffff469,
    0xfffff447, 0xfffff424, 0xfffff402, 0xfffff3e0, 0xfffff3be, 0xfffff39c, 0xfffff379, 0xfffff357,
    0xfffff335, 0xfffff313, 0xfffff2f1, 0xfffff2ce, 0xfffff2ac, 0xfffff28a, 0xfffff268, 0xfffff245,
    0xfffff223, 0xfffff201, 0xfffff1df, 0xfffff1bd, 0xfffff19a, 0xfffff178, 0xfffff156, 0xfffff134,
    0xfffff112, 0xfffff0ef, 0xfffff0cd, 0xfffff0ab, 0xfffff089, 0xfffff067, 0xfffff044, 0xfffff022,
}};

#pragma section sName end
