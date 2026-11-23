#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov033_02112294_Layout;
typedef char data_ov033_02112294_size[sizeof(data_ov033_02112294_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[33];
} data_ov033_0211229c_Layout;
typedef char data_ov033_0211229c_size[sizeof(data_ov033_0211229c_Layout) == 0x84 ? 1 : -1];

extern data_ov033_02112294_Layout data_ov033_02112294;
extern data_ov033_0211229c_Layout data_ov033_0211229c;

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

data_ov033_02112294_Layout data_ov033_02112294 = {{
    0x00000000,
    0xffff0000,
}};

data_ov033_0211229c_Layout data_ov033_0211229c = {{
    0x076c0010, 0x0640fd12, 0x00000000, 0xffff0000, 0x044c0010, 0x0320ff6a, 0x00000000,
    0xffff0000, 0x05dc0010, 0x044cfe3e, 0x00000000, 0xffff0000, 0xf8f80139, 0xfaecfda8,
    0x00000000, 0xffff0000, 0xf8f800fe, 0x0190fcaa, 0x00000000, 0x10040000, 0x06ce00fe,
    0xf8befe3e, 0x00000000, 0x12040000, 0xfaec00fe, 0xfd7600d2, 0x00000000, 0x11040000,
    0xf8f800fe, 0x0190fcaa, 0x00000000, 0x00040000, 0x06ce00fe,
}};
