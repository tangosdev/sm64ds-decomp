#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[9];
} data_ov033_02111f9c_Layout;
typedef char data_ov033_02111f9c_size[sizeof(data_ov033_02111f9c_Layout) == 0x24 ? 1 : -1];

typedef struct {
    u32 w[51];
} data_ov033_02111fc0_Layout;
typedef char data_ov033_02111fc0_size[sizeof(data_ov033_02111fc0_Layout) == 0xcc ? 1 : -1];

extern data_ov033_02111f9c_Layout data_ov033_02111f9c;
extern data_ov033_02111fc0_Layout data_ov033_02111fc0;

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

data_ov033_02111f9c_Layout data_ov033_02111f9c = {{
    0x06e0001f,
    0xff07fea2,
    0x00000000,
    0xffff0000,
    0xf93400ba,
    0xf90cfda8,
    0x00000000,
    0xffff0000,
    0xfc2000ba,
}};

data_ov033_02111fc0_Layout data_ov033_02111fc0 = {{
    0xfb73007d, 0x10000000, 0xffff0000, 0x076c00ba, 0xf95cfe3e, 0x00000000, 0xffff0000, 0x07120014,
    0x0514fd12, 0x00000001, 0x0b220000, 0xfab5000d, 0xfd870050, 0xc0000000, 0x00200000, 0xfda80016,
    0x044cff6a, 0x00000000, 0x00000000, 0xfa9c002b, 0xf95cfd12, 0xc0000000, 0xff020000, 0x00000076,
    0xfe400483, 0x40000000, 0xffff0000, 0x00000077, 0x00000000, 0x00000000, 0xffff0000, 0xfbb40019,
    0xf95cfc4a, 0x00000000, 0xff000000, 0xfbb40019, 0xf894fc4a, 0x00000000, 0xff000000, 0xfc7c0019,
    0xf894fc4a, 0x00000000, 0xff000000, 0xfc7c0019, 0xf95cfc4a, 0x00000000, 0xff000000, 0xfd440019,
    0xf894fc4a, 0x00000000, 0xff000000,
}};
