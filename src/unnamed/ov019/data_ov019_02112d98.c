#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[7];
} data_ov019_02112d98_Layout;
typedef char data_ov019_02112d98_size[sizeof(data_ov019_02112d98_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    void *f00c;
    u32 f010;
    void *f014;
    u32 f018;
    void *f01c;
    u32 f020;
    void *f024;
    u32 f028;
    void *f02c;
    u32 f030;
    void *f034;
    u32 f038;
    void *f03c;
    u32 f040;
    void *f044;
    u32 f048;
    void *f04c;
    u32 f050;
    void *f054;
    u32 f058;
    void *f05c;
    u32 f060;
    void *f064;
    u32 f068;
    void *f06c;
} data_ov019_02112db4_Layout;
typedef char data_ov019_02112db4_size[sizeof(data_ov019_02112db4_Layout) == 0x70 ? 1 : -1];

extern data_ov019_02112d98_Layout data_ov019_02112d98;
extern data_ov019_02112db4_Layout data_ov019_02112db4;
extern u8 data_ov019_02112d04[];
extern u8 data_ov019_02113028[];
extern u8 data_ov019_02112c08[];
extern u8 data_ov019_02112bc8[];
extern u8 data_ov019_02112c18[];
extern u8 data_ov019_02112b90[];
extern u8 data_ov019_02112c28[];
extern u8 data_ov019_02112bc0[];
extern u8 data_ov019_02112c38[];
extern u8 data_ov019_02112bb0[];
extern u8 data_ov019_02112c48[];
extern u8 data_ov019_02112b98[];
extern u8 data_ov019_02112c58[];
extern u8 data_ov019_02112bb8[];

data_ov019_02112d98_Layout data_ov019_02112d98 = {{
    0x000000ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
}};

data_ov019_02112db4_Layout data_ov019_02112db4 = {
    0x00000500, &data_ov019_02112d04[0], 0x00004905, &data_ov019_02113028[0],
    0x00000140, &data_ov019_02112c08[0], 0x00000145, &data_ov019_02112bc8[0],
    0x00000160, &data_ov019_02112c18[0], 0x00000165, &data_ov019_02112b90[0],
    0x00000180, &data_ov019_02112c28[0], 0x00000185, &data_ov019_02112bc0[0],
    0x000001a0, &data_ov019_02112c38[0], 0x000001a5, &data_ov019_02112bb0[0],
    0x000001c0, &data_ov019_02112c48[0], 0x000001c5, &data_ov019_02112b98[0],
    0x000001e0, &data_ov019_02112c58[0], 0x000001e5, &data_ov019_02112bb8[0],
};
