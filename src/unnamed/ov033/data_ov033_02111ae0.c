#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov033_02111ae0_Layout;
typedef char data_ov033_02111ae0_size[sizeof(data_ov033_02111ae0_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov033_02111ae4_Layout;
typedef char data_ov033_02111ae4_size[sizeof(data_ov033_02111ae4_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov033_02111ae8_Layout;
typedef char data_ov033_02111ae8_size[sizeof(data_ov033_02111ae8_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov033_02111aec_Layout;
typedef char data_ov033_02111aec_size[sizeof(data_ov033_02111aec_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov033_02111af4_Layout;
typedef char data_ov033_02111af4_size[sizeof(data_ov033_02111af4_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov033_02111afc_Layout;
typedef char data_ov033_02111afc_size[sizeof(data_ov033_02111afc_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov033_02111b04_Layout;
typedef char data_ov033_02111b04_size[sizeof(data_ov033_02111b04_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov033_02111b0c_Layout;
typedef char data_ov033_02111b0c_size[sizeof(data_ov033_02111b0c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov033_02111b14_Layout;
typedef char data_ov033_02111b14_size[sizeof(data_ov033_02111b14_Layout) == 0xc ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    u32 f008;
} data_ov033_02111b20_Layout;
typedef char data_ov033_02111b20_size[sizeof(data_ov033_02111b20_Layout) == 0xc ? 1 : -1];

extern data_ov033_02111ae0_Layout data_ov033_02111ae0;
extern data_ov033_02111ae4_Layout data_ov033_02111ae4;
extern data_ov033_02111ae8_Layout data_ov033_02111ae8;
extern data_ov033_02111aec_Layout data_ov033_02111aec;
extern data_ov033_02111af4_Layout data_ov033_02111af4;
extern data_ov033_02111afc_Layout data_ov033_02111afc;
extern data_ov033_02111b04_Layout data_ov033_02111b04;
extern data_ov033_02111b0c_Layout data_ov033_02111b0c;
extern data_ov033_02111b14_Layout data_ov033_02111b14;
extern data_ov033_02111b20_Layout data_ov033_02111b20;
extern u8 data_ov033_02111cb8[];
extern u8 data_ov033_02111c68[];
extern u8 data_ov033_02111bb0[];

#pragma define_section sName ".data.gap0"                                                          \
                             ".data.gap0"                                                          \
                             ".data.gap0" far_data
#pragma section sName begin

data_ov033_02111ae8_Layout data_ov033_02111ae8 = {{
    0x03033334,
}};

data_ov033_02111ae4_Layout data_ov033_02111ae4 = {{
    0x00000960,
}};

data_ov033_02111ae0_Layout data_ov033_02111ae0 = {{
    0x000007e9,
}};

data_ov033_02111afc_Layout data_ov033_02111afc = {
    0x0000000a,
    &data_ov033_02111c68[0],
};

data_ov033_02111af4_Layout data_ov033_02111af4 = {{
    0xfd12012c,
    0x0000effc,
}};

data_ov033_02111b04_Layout data_ov033_02111b04 = {{
    0xfda8f9c0,
    0x0000f830,
}};

data_ov033_02111b0c_Layout data_ov033_02111b0c = {{
    0x0000483f,
    0xeed0fd44,
}};

data_ov033_02111aec_Layout data_ov033_02111aec = {
    0x0000000a,
    &data_ov033_02111cb8[0],
};

data_ov033_02111b14_Layout data_ov033_02111b14 = {{
    0x65746177,
    0x616d5f72,
    0x00000074,
}};

data_ov033_02111b20_Layout data_ov033_02111b20 = {
    &data_ov033_02111afc,
    &data_ov033_02111bb0[0],
    0x04b00000,
};
