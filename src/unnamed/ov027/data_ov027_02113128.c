#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[11];
} data_ov027_02113128_Layout;
typedef char data_ov027_02113128_size[sizeof(data_ov027_02113128_Layout) == 0x2c ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov027_02113154_Layout;
typedef char data_ov027_02113154_size[sizeof(data_ov027_02113154_Layout) == 0x30 ? 1 : -1];

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
} data_ov027_02113184_Layout;
typedef char data_ov027_02113184_size[sizeof(data_ov027_02113184_Layout) == 0x48 ? 1 : -1];

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
} data_ov027_021131cc_Layout;
typedef char data_ov027_021131cc_size[sizeof(data_ov027_021131cc_Layout) == 0x48 ? 1 : -1];

extern data_ov027_02113128_Layout data_ov027_02113128;
extern data_ov027_02113154_Layout data_ov027_02113154;
extern data_ov027_02113184_Layout data_ov027_02113184;
extern data_ov027_021131cc_Layout data_ov027_021131cc;
extern u8 data_ov027_0211300c[];
extern u8 data_ov027_02112fe8[];
extern u8 data_ov027_02112fe0[];
extern u8 data_ov027_02113214[];
extern u8 data_ov027_02113074[];
extern u8 data_ov027_021130a4[];
extern u8 data_ov027_02112fe4[];
extern u8 data_ov027_02113024[];
extern u8 data_ov027_02113434[];
extern u8 data_ov027_02113344[];
extern u8 data_ov027_02113044[];
extern u8 data_ov027_02113034[];
extern u8 data_ov027_02113054[];
extern u8 data_ov027_02113064[];
extern u8 data_ov027_02113084[];
extern u8 data_ov027_02113094[];

#pragma define_section sName ".data.gap0"                                                          \
                             ".data.gap0"                                                          \
                             ".data.gap0" far_data
#pragma section sName begin

data_ov027_02113128_Layout data_ov027_02113128 = {{
    0x1806ff02,
    0x02580514,
    0xc16d0000,
    0xff020000,
    0x0960fed4,
    0x00000514,
    0x000033e9,
    0x0000ff07,
    0x00001356,
    0x00000000,
    0x00000000,
}};

data_ov027_02113154_Layout data_ov027_02113154 = {{
    0x10cc0a3e,
    0xecdc0541,
    0x012c0a3e,
    0xee6c041a,
    0xe7640a3e,
    0x12c0067e,
    0xeed00a3e,
    0xec78054b,
    0xe5d40a3e,
    0xea8408fc,
    0x1194ca3d,
    0xff3804e2,
}};

data_ov027_02113184_Layout data_ov027_02113184 = {
    0x00000202, &data_ov027_0211300c[0], 0x00000103, &data_ov027_02112fe8[0],
    0x00000304, &data_ov027_02113128,    0x0000010b, &data_ov027_02112fe0[0],
    0x00000501, &data_ov027_02113214[0], 0x00000206, &data_ov027_02113074[0],
    0x00000207, &data_ov027_021130a4[0], 0x0000010e, &data_ov027_02112fe4[0],
    0x0000010a, &data_ov027_02113024[0],
};

data_ov027_021131cc_Layout data_ov027_021131cc = {
    0x00003900, &data_ov027_02113434[0], 0x00001e05, &data_ov027_02113344[0],
    0x00000140, &data_ov027_02113044[0], 0x00000160, &data_ov027_02113034[0],
    0x00000180, &data_ov027_02113054[0], 0x000001a0, &data_ov027_02113064[0],
    0x000006a5, &data_ov027_02113154,    0x000001c0, &data_ov027_02113084[0],
    0x000001e0, &data_ov027_02113094[0],
};
