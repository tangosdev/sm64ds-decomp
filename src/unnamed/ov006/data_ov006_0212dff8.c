#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov006_0212dff8_Layout;
typedef char data_ov006_0212dff8_size[sizeof(data_ov006_0212dff8_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov006_0212dffc_Layout;
typedef char data_ov006_0212dffc_size[sizeof(data_ov006_0212dffc_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov006_0212e000_Layout;
typedef char data_ov006_0212e000_size[sizeof(data_ov006_0212e000_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov006_0212e004_Layout;
typedef char data_ov006_0212e004_size[sizeof(data_ov006_0212e004_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov006_0212e008_Layout;
typedef char data_ov006_0212e008_size[sizeof(data_ov006_0212e008_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov006_0212e00c_Layout;
typedef char data_ov006_0212e00c_size[sizeof(data_ov006_0212e00c_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov006_0212e010_Layout;
typedef char data_ov006_0212e010_size[sizeof(data_ov006_0212e010_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov006_0212e014_Layout;
typedef char data_ov006_0212e014_size[sizeof(data_ov006_0212e014_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov006_0212e018_Layout;
typedef char data_ov006_0212e018_size[sizeof(data_ov006_0212e018_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov006_0212e01c_Layout;
typedef char data_ov006_0212e01c_size[sizeof(data_ov006_0212e01c_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov006_0212e020_Layout;
typedef char data_ov006_0212e020_size[sizeof(data_ov006_0212e020_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} sTrmpRowBaseHeights_Layout;
typedef char sTrmpRowBaseHeights_size[sizeof(sTrmpRowBaseHeights_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[3];
} sTrmp2MarioScale_Layout;
typedef char sTrmp2MarioScale_size[sizeof(sTrmp2MarioScale_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[3];
} sTrmp3DMarioScale_Layout;
typedef char sTrmp3DMarioScale_size[sizeof(sTrmp3DMarioScale_Layout) == 0xc ? 1 : -1];

extern data_ov006_0212dff8_Layout data_ov006_0212dff8;
extern data_ov006_0212dffc_Layout data_ov006_0212dffc;
extern data_ov006_0212e000_Layout data_ov006_0212e000;
extern data_ov006_0212e004_Layout data_ov006_0212e004;
extern data_ov006_0212e008_Layout data_ov006_0212e008;
extern data_ov006_0212e00c_Layout data_ov006_0212e00c;
extern data_ov006_0212e010_Layout data_ov006_0212e010;
extern data_ov006_0212e014_Layout data_ov006_0212e014;
extern data_ov006_0212e018_Layout data_ov006_0212e018;
extern data_ov006_0212e01c_Layout data_ov006_0212e01c;
extern data_ov006_0212e020_Layout data_ov006_0212e020;
extern sTrmpRowBaseHeights_Layout sTrmpRowBaseHeights;
extern sTrmp2MarioScale_Layout sTrmp2MarioScale;
extern sTrmp3DMarioScale_Layout sTrmp3DMarioScale;

#pragma define_section sName ".rodata.before_coin_tables"                                          \
                             ".rodata.before_coin_tables"                                          \
                             ".rodata.before_coin_tables" far_data
#pragma section sName begin

data_ov006_0212dff8_Layout data_ov006_0212dff8 = {{
    0x00006000,
}};

data_ov006_0212dffc_Layout data_ov006_0212dffc = {{
    0x00000c00,
}};

data_ov006_0212e020_Layout data_ov006_0212e020 = {{
    0x000000e0,
}};

data_ov006_0212e01c_Layout data_ov006_0212e01c = {{
    0x00000180,
}};

data_ov006_0212e018_Layout data_ov006_0212e018 = {{
    0x00000e00,
}};

data_ov006_0212e014_Layout data_ov006_0212e014 = {{
    0x00005800,
}};

data_ov006_0212e010_Layout data_ov006_0212e010 = {{
    0xffffe400,
}};

data_ov006_0212e00c_Layout data_ov006_0212e00c = {{
    0x000000c0,
}};

data_ov006_0212e008_Layout data_ov006_0212e008 = {{
    0x00004800,
}};

data_ov006_0212e004_Layout data_ov006_0212e004 = {{
    0x00005400,
}};

data_ov006_0212e000_Layout data_ov006_0212e000 = {{
    0xffffdc00,
}};

sTrmpRowBaseHeights_Layout sTrmpRowBaseHeights = {{
    0x00028000,
    0xfff68000,
}};

sTrmp2MarioScale_Layout sTrmp2MarioScale = {{
    0x00002000,
    0x00002000,
    0x00002000,
}};

sTrmp3DMarioScale_Layout sTrmp3DMarioScale = {{
    0x00002000,
    0x00002000,
    0x00002000,
}};
