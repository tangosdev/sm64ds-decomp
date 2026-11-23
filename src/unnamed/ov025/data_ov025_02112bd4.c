#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    void *f004;
} data_ov025_02112bd4_Layout;
typedef char data_ov025_02112bd4_size[sizeof(data_ov025_02112bd4_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    u32 f008;
    void *f00c;
} data_ov025_02112bdc_Layout;
typedef char data_ov025_02112bdc_size[sizeof(data_ov025_02112bdc_Layout) == 0x10 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    u32 f008;
    u32 f00c;
    void *f010;
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
} data_ov025_02112bec_Layout;
typedef char data_ov025_02112bec_size[sizeof(data_ov025_02112bec_Layout) == 0x3c ? 1 : -1];

extern data_ov025_02112bd4_Layout data_ov025_02112bd4;
extern data_ov025_02112bdc_Layout data_ov025_02112bdc;
extern data_ov025_02112bec_Layout data_ov025_02112bec;
extern u8 data_ov025_02112658[];
extern u8 data_ov025_02112654[];
extern u8 data_ov025_02112694[];
extern u8 data_ov025_0211265c[];
extern u8 data_ov025_02113404[];
extern u8 data_ov025_02112b5c[];
extern u8 data_ov025_02112bbc[];

data_ov025_02112bd4_Layout data_ov025_02112bd4 = {
    0x0000005b,
    &data_ov025_02112658[0],
};

data_ov025_02112bdc_Layout data_ov025_02112bdc = {
    &data_ov025_02112654[0],
    &data_ov025_02112694[0],
    0x00000002,
    &data_ov025_0211265c[0],
};

data_ov025_02112bec_Layout data_ov025_02112bec = {
    &data_ov025_02113404[0],
    &data_ov025_02112b5c[0],
    0x0716071b,
    0x071d071c,
    &data_ov025_02112bbc[0],
    0x05780002,
    0x0000000f,
    0x53504c43,
    0x00010008,
    0x01000fc4,
    0x000000ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
