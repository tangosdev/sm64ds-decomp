#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211cc44_Layout;
typedef char data_ov075_0211cc44_size[sizeof(data_ov075_0211cc44_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211cc4c_Layout;
typedef char data_ov075_0211cc4c_size[sizeof(data_ov075_0211cc4c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211cc54_Layout;
typedef char data_ov075_0211cc54_size[sizeof(data_ov075_0211cc54_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211cc5c_Layout;
typedef char data_ov075_0211cc5c_size[sizeof(data_ov075_0211cc5c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[10];
} data_ov075_0211cc64_Layout;
typedef char data_ov075_0211cc64_size[sizeof(data_ov075_0211cc64_Layout) == 0x28 ? 1 : -1];

typedef struct {
    u32 w[10];
} data_ov075_0211cc8c_Layout;
typedef char data_ov075_0211cc8c_size[sizeof(data_ov075_0211cc8c_Layout) == 0x28 ? 1 : -1];

typedef struct {
    u32 w[10];
} data_ov075_0211ccb4_Layout;
typedef char data_ov075_0211ccb4_size[sizeof(data_ov075_0211ccb4_Layout) == 0x28 ? 1 : -1];

typedef struct {
    u32 w[10];
} data_ov075_0211ccdc_Layout;
typedef char data_ov075_0211ccdc_size[sizeof(data_ov075_0211ccdc_Layout) == 0x28 ? 1 : -1];

typedef struct {
    u32 w[10];
} data_ov075_0211cd04_Layout;
typedef char data_ov075_0211cd04_size[sizeof(data_ov075_0211cd04_Layout) == 0x28 ? 1 : -1];

typedef struct {
    u32 w[10];
} data_ov075_0211cd2c_Layout;
typedef char data_ov075_0211cd2c_size[sizeof(data_ov075_0211cd2c_Layout) == 0x28 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    void *f008;
    u32 f00c;
    void *f010;
    u32 f014;
    void *f018;
    u32 f01c;
    void *f020;
    u32 f024;
} data_ov075_0211cd54_Layout;
typedef char data_ov075_0211cd54_size[sizeof(data_ov075_0211cd54_Layout) == 0x28 ? 1 : -1];

extern data_ov075_0211cc44_Layout data_ov075_0211cc44;
extern data_ov075_0211cc4c_Layout data_ov075_0211cc4c;
extern data_ov075_0211cc54_Layout data_ov075_0211cc54;
extern data_ov075_0211cc5c_Layout data_ov075_0211cc5c;
extern data_ov075_0211cc64_Layout data_ov075_0211cc64;
extern data_ov075_0211cc8c_Layout data_ov075_0211cc8c;
extern data_ov075_0211ccb4_Layout data_ov075_0211ccb4;
extern data_ov075_0211ccdc_Layout data_ov075_0211ccdc;
extern data_ov075_0211cd04_Layout data_ov075_0211cd04;
extern data_ov075_0211cd2c_Layout data_ov075_0211cd2c;
extern data_ov075_0211cd54_Layout data_ov075_0211cd54;
extern u8 data_ov075_0211caac[];
extern u8 data_ov075_0211cc24[];
extern u8 data_ov075_0211cfb4[];
extern u8 data_ov075_0211cfe4[];
extern u8 data_ov075_0211cdd4[];
extern u8 data_ov075_0211ce04[];
extern u8 data_ov075_0211ce34[];
extern u8 data_ov075_0211ce64[];

#pragma define_section sName ".data.entry_sprite_gap9"                                             \
                             ".data.entry_sprite_gap9"                                             \
                             ".data.entry_sprite_gap9" far_data
#pragma section sName begin

data_ov075_0211cc54_Layout data_ov075_0211cc54 = {
    &data_ov075_0211cfb4[0],
    0x00000006,
};

data_ov075_0211cc5c_Layout data_ov075_0211cc5c = {
    &data_ov075_0211cfe4[0],
    0x00000006,
};

data_ov075_0211cc44_Layout data_ov075_0211cc44 = {
    &data_ov075_0211caac[0],
    0x00000003,
};

data_ov075_0211cc4c_Layout data_ov075_0211cc4c = {
    &data_ov075_0211cc24[0],
    0x00000004,
};

data_ov075_0211cc8c_Layout data_ov075_0211cc8c = {{
    0x81b100d0,
    0x00001394,
    0x81d100d0,
    0x00001398,
    0x81f100d0,
    0x0000138c,
    0x803100d0,
    0x0000138c,
    0x801100d0,
    0xffff139c,
}};

data_ov075_0211cc64_Layout data_ov075_0211cc64 = {{
    0x81ba00d0,
    0x00000380,
    0x81d600d0,
    0x00000384,
    0x81f200d0,
    0x00000388,
    0x801200d0,
    0x0000038c,
    0x802e00d0,
    0xffff0390,
}};

data_ov075_0211ccdc_Layout data_ov075_0211ccdc = {{
    0xc1d800e8,
    0x0000a180,
    0x401880e8,
    0x0000a188,
    0x40188008,
    0x0000a208,
    0x81f040d8,
    0x0000a143,
    0x401000d8,
    0xffffa147,
}};

data_ov075_0211cd04_Layout data_ov075_0211cd04 = {{
    0x81f000d8,
    0x0000a012,
    0x801080d8,
    0x0000a016,
    0x40188008,
    0x0000a208,
    0xc1d800e8,
    0x0000a180,
    0x401880e8,
    0xffffa188,
}};

data_ov075_0211ccb4_Layout data_ov075_0211ccb4 = {{
    0x81b000d0,
    0x00001380,
    0x81d000d0,
    0x00001310,
    0x81f000d0,
    0x00001384,
    0x801000d0,
    0x00001388,
    0x803000d0,
    0xffff1310,
}};

data_ov075_0211cd2c_Layout data_ov075_0211cd2c = {{
    0x81f000d8,
    0x0000a00c,
    0x801080d8,
    0x0000a010,
    0xc1d800e8,
    0x0000a180,
    0x401880e8,
    0x0000a188,
    0x40188008,
    0xffffa208,
}};

data_ov075_0211cd54_Layout data_ov075_0211cd54 = {
    &data_ov075_0211cdd4[0], 0x0000001e, &data_ov075_0211ce04[0], 0x00000003,
    &data_ov075_0211ce34[0], 0x00000003, &data_ov075_0211ce64[0], 0x00000003,
    &data_ov075_0211ccdc,    0x00000028,
};
