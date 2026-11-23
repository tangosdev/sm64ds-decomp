#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov017_02111ab4_Layout;
typedef char data_ov017_02111ab4_size[sizeof(data_ov017_02111ab4_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov017_02111abc_Layout;
typedef char data_ov017_02111abc_size[sizeof(data_ov017_02111abc_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    void *f00c;
    u32 f010;
    void *f014;
    u32 f018;
    void *f01c;
} data_ov017_02111ad4_Layout;
typedef char data_ov017_02111ad4_size[sizeof(data_ov017_02111ad4_Layout) == 0x20 ? 1 : -1];

extern data_ov017_02111ab4_Layout data_ov017_02111ab4;
extern data_ov017_02111abc_Layout data_ov017_02111abc;
extern data_ov017_02111ad4_Layout data_ov017_02111ad4;
extern u8 data_ov017_02111a30[];
extern u8 data_ov017_02111a00[];
extern u8 data_ov017_02111b2c[];
extern u8 data_ov017_02111a40[];

#pragma define_section sName ".data.ksRest"                                                        \
                             ".data.ksRest"                                                        \
                             ".data.ksRest" far_data
#pragma section sName begin

data_ov017_02111ab4_Layout data_ov017_02111ab4 = {{
    0x53504c43,
    0x00020008,
}};

data_ov017_02111abc_Layout data_ov017_02111abc = {{
    0x00030fc0,
    0x000000ff,
    0x00030fc5,
    0x000000ff,
    0xffffffff,
    0xffffffff,
}};

data_ov017_02111ad4_Layout data_ov017_02111ad4 = {
    0x00000104, &data_ov017_02111a30[0], 0x0000010b, &data_ov017_02111a00[0],
    0x00000401, &data_ov017_02111b2c[0], 0x00000208, &data_ov017_02111a40[0],
};
