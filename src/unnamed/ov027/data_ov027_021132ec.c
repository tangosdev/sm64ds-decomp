#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[22];
} data_ov027_021132ec_Layout;
typedef char data_ov027_021132ec_size[sizeof(data_ov027_021132ec_Layout) == 0x58 ? 1 : -1];

typedef struct {
    u32 w[25];
} data_ov027_02113344_Layout;
typedef char data_ov027_02113344_size[sizeof(data_ov027_02113344_Layout) == 0x64 ? 1 : -1];

typedef struct {
    u32 w[35];
} data_ov027_021133a8_Layout;
typedef char data_ov027_021133a8_size[sizeof(data_ov027_021133a8_Layout) == 0x8c ? 1 : -1];

typedef struct {
    u32 w[57];
} data_ov027_02113434_Layout;
typedef char data_ov027_02113434_size[sizeof(data_ov027_02113434_Layout) == 0xe4 ? 1 : -1];

extern data_ov027_021132ec_Layout data_ov027_021132ec;
extern data_ov027_02113344_Layout data_ov027_02113344;
extern data_ov027_021133a8_Layout data_ov027_021133a8;
extern data_ov027_02113434_Layout data_ov027_02113434;

#pragma define_section sName ".data.gap0"                                                          \
                             ".data.gap0"                                                          \
                             ".data.gap0" far_data
#pragma section sName begin

data_ov027_021132ec_Layout data_ov027_021132ec = {{
    0x00742fc5, 0x000000ff, 0x00742fc4, 0x000000ff, 0x0003cfc1, 0x000000ff, 0x0003cfc4, 0x000000ff,
    0x0003cfc5, 0x000000ff, 0x0003cfc7, 0x000000ff, 0x00a43fc6, 0x000000ff, 0x00043fc6, 0x000000ff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
}};

data_ov027_02113344_Layout data_ov027_02113344 = {{
    0x09c4e225, 0x11f803e8, 0x0b54e225, 0x0e7403e8, 0x0e74063d, 0x125c079e, 0xfa88e225,
    0x03840436, 0xfb50e225, 0x03e80550, 0xfc18e225, 0x044c066a, 0xfc7ce225, 0x04b006fd,
    0xfd12e225, 0x04e2073d, 0xfda8e225, 0x0514077d, 0xfe3ee225, 0x054607b0, 0xfed4e225,
    0x057807e0, 0x04e2e225, 0x017c086e, 0x06a4e225,
}};

data_ov027_021133a8_Layout data_ov027_021133a8 = {{
    0xfd7609c4, 0x06d6e225, 0x00320c1c, 0x0000e225, 0x07080cb2, 0xfed4e225, 0x06400d5b,
    0x159d5e29, 0xf34103e8, 0x14945e29, 0xeb3c0405, 0x0de75e29, 0xe98903e8, 0x07515e29,
    0xedd803e8, 0x06525e29, 0xf23f05dc, 0xf19f5e29, 0xfb6f03e8, 0xf5875e29, 0xfdc703e8,
    0x00005e29, 0x0000128e, 0x06bc5e29, 0xfc680abe, 0x00fa443f, 0xee9e058c, 0xee08483f,
    0x16a3044c, 0x02bc023d, 0x028a10fe, 0x01baec3d, 0x05340924, 0x0651e225, 0xf23f0898,
}};

data_ov027_02113434_Layout data_ov027_02113434 = {{
    0x06a4009f, 0x01c20cb2, 0xdc720000, 0xffff0000, 0x03ba0074, 0x07e903e8, 0x00000000, 0xffff0000,
    0x08340095, 0x15e003e8, 0x00000000, 0xffff0000, 0x05dc0103, 0x0ce403e8, 0x40000000, 0xffff0000,
    0x01f40103, 0x151803e8, 0x40000000, 0xffff0000, 0x03e80103, 0x113003e8, 0x40000000, 0xffff0000,
    0x0dac004c, 0x01f403c4, 0x4000ffff, 0x07420000, 0x0c660039, 0x04f50353, 0x00000000, 0x02ff0000,
    0x14db0039, 0xfc0903e8, 0x00000000, 0x01ff0000, 0x11170032, 0x04710320, 0x80000000, 0x0002c000,
    0x0ce40103, 0xf5100639, 0x00000000, 0xffff0000, 0x0f3c00bd, 0xf44802f9, 0x00000000, 0x00ff0000,
    0x0f8c0099, 0xf4de0347, 0x00000000, 0x00030000, 0x00c80122, 0xee6c0528, 0x00000000, 0xff020000,
    0x0dac0103,
}};
