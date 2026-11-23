#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov052_021121b8_Layout;
typedef char data_ov052_021121b8_size[sizeof(data_ov052_021121b8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov052_021121c0_Layout;
typedef char data_ov052_021121c0_size[sizeof(data_ov052_021121c0_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov052_021121dc_Layout;
typedef char data_ov052_021121dc_size[sizeof(data_ov052_021121dc_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[24];
} data_ov052_021121f8_Layout;
typedef char data_ov052_021121f8_size[sizeof(data_ov052_021121f8_Layout) == 0x60 ? 1 : -1];

extern data_ov052_021121b8_Layout data_ov052_021121b8;
extern data_ov052_021121c0_Layout data_ov052_021121c0;
extern data_ov052_021121dc_Layout data_ov052_021121dc;
extern data_ov052_021121f8_Layout data_ov052_021121f8;

data_ov052_021121b8_Layout data_ov052_021121b8 = {{
    0xe9bc0fa0,
    0x00000000,
}};

data_ov052_021121c0_Layout data_ov052_021121c0 = {{
    0xffff0000,
    0x01c2000f,
    0x0abe0fa0,
    0x00000000,
    0xffff0000,
    0xf704000f,
    0xf2860fa0,
}};

data_ov052_021121dc_Layout data_ov052_021121dc = {{
    0x00000000,
    0xffff0000,
    0xff06001f,
    0xfa56109a,
    0x00000000,
    0x05000000,
    0x01900024,
}};

data_ov052_021121f8_Layout data_ov052_021121f8 = {{
    0x04b01068, 0x00000000, 0xffff0000, 0x012c000f, 0xf0c40fa0, 0x00000000, 0xffff0000, 0xfe0c000f,
    0xf0600fa0, 0x00000000, 0xffff0000, 0x00c8000f, 0xed400fa0, 0x00000000, 0xffff0000, 0xfa88000f,
    0xe9580fa0, 0x00000000, 0xffff0000, 0xe34a011c, 0xf3801004, 0x00000000, 0xffff0000, 0xf8620084,
}};
