#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov012_02112434_Layout;
typedef char data_ov012_02112434_size[sizeof(data_ov012_02112434_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 f000;
    u32 f004;
    u32 f008;
    u32 f00c;
    void *f010;
} data_ov012_02112438_Layout;
typedef char data_ov012_02112438_size[sizeof(data_ov012_02112438_Layout) == 0x14 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
    u32 f00c;
    u32 f010;
    u32 f014;
} data_ov012_0211244c_Layout;
typedef char data_ov012_0211244c_size[sizeof(data_ov012_0211244c_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov012_02112464_Layout;
typedef char data_ov012_02112464_size[sizeof(data_ov012_02112464_Layout) == 0x30 ? 1 : -1];

extern data_ov012_02112434_Layout data_ov012_02112434;
extern data_ov012_02112438_Layout data_ov012_02112438;
extern data_ov012_0211244c_Layout data_ov012_0211244c;
extern data_ov012_02112464_Layout data_ov012_02112464;
extern u8 _ZN14daObjC0Water_cD1Ev[];
extern u8 _ZN14daObjC0Water_cD0Ev[];

#pragma define_section sName ".data.c0Rest"                                                        \
                             ".data.c0Rest"                                                        \
                             ".data.c0Rest" far_data
#pragma section sName begin

data_ov012_02112434_Layout data_ov012_02112434 = {{
    0x02010f6c,
}};

data_ov012_02112438_Layout data_ov012_02112438 = {
    0x02043ac0, 0x0204357c, 0x0204349c, 0x02043494, &_ZN14daObjC0Water_cD1Ev[0],
};

data_ov012_0211244c_Layout data_ov012_0211244c = {
    &_ZN14daObjC0Water_cD0Ev[0], 0x02010160, 0x02010154, 0x0201014c, 0x02010148, 0x02010144,
};

data_ov012_02112464_Layout data_ov012_02112464 = {{
    0x02010140,
    0x0201013c,
    0x02010138,
    0x02010134,
    0x02010130,
    0x0201012c,
    0x02010124,
    0x020100dc,
    0x020ee55c,
    0x00000000,
    0x00000000,
    0x00000000,
}};
