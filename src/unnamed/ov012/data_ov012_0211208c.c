#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[6];
} data_ov012_0211208c_Layout;
typedef char data_ov012_0211208c_size[sizeof(data_ov012_0211208c_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov012_021120a4_Layout;
typedef char data_ov012_021120a4_size[sizeof(data_ov012_021120a4_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov012_021120bc_Layout;
typedef char data_ov012_021120bc_size[sizeof(data_ov012_021120bc_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov012_021120d8_Layout;
typedef char data_ov012_021120d8_size[sizeof(data_ov012_021120d8_Layout) == 0x20 ? 1 : -1];

extern data_ov012_0211208c_Layout data_ov012_0211208c;
extern data_ov012_021120a4_Layout data_ov012_021120a4;
extern data_ov012_021120bc_Layout data_ov012_021120bc;
extern data_ov012_021120d8_Layout data_ov012_021120d8;

#pragma define_section sName ".data.c0Rest"                                                        \
                             ".data.c0Rest"                                                        \
                             ".data.c0Rest" far_data
#pragma section sName begin

data_ov012_0211208c_Layout data_ov012_0211208c = {{
    0x01f4fd12,
    0xc0000000,
    0xffff0000,
    0xf9b60108,
    0xff9cfd12,
    0xc0000000,
}};

data_ov012_021120a4_Layout data_ov012_021120a4 = {{
    0xffff0000,
    0xf9b60108,
    0xfd44fd12,
    0xc0000000,
    0xffff0000,
    0xf8a40108,
}};

data_ov012_021120bc_Layout data_ov012_021120bc = {{
    0xf236fd12,
    0x20000000,
    0xffff0000,
    0xfcd10108,
    0xf236fd12,
    0xe0000000,
    0xffff0000,
}};

data_ov012_021120d8_Layout data_ov012_021120d8 = {{
    0xf3580109,
    0x04b0fcae,
    0x40000000,
    0xffff0000,
    0xf5060109,
    0x04b0fcae,
    0xc0000000,
    0xffff0000,
}};
