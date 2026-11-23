#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov018_021130cc_Layout;
typedef char data_ov018_021130cc_size[sizeof(data_ov018_021130cc_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[20];
} data_ov018_021130d0_Layout;
typedef char data_ov018_021130d0_size[sizeof(data_ov018_021130d0_Layout) == 0x50 ? 1 : -1];

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
} data_ov018_02113120_Layout;
typedef char data_ov018_02113120_size[sizeof(data_ov018_02113120_Layout) == 0x50 ? 1 : -1];

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
} data_ov018_02113170_Layout;
typedef char data_ov018_02113170_size[sizeof(data_ov018_02113170_Layout) == 0x58 ? 1 : -1];

typedef struct {
    u32 w[24];
} data_ov018_021131c8_Layout;
typedef char data_ov018_021131c8_size[sizeof(data_ov018_021131c8_Layout) == 0x60 ? 1 : -1];

extern data_ov018_021130cc_Layout data_ov018_021130cc;
extern data_ov018_021130d0_Layout data_ov018_021130d0;
extern data_ov018_02113120_Layout data_ov018_02113120;
extern data_ov018_02113170_Layout data_ov018_02113170;
extern data_ov018_021131c8_Layout data_ov018_021131c8;
extern u8 data_ov018_02112f14[];
extern u8 data_ov018_02113058[];
extern u8 data_ov018_02112e80[];
extern u8 data_ov018_02112edc[];
extern u8 data_ov018_02112eec[];
extern u8 data_ov018_02112ec0[];
extern u8 data_ov018_02112e84[];
extern u8 data_ov018_02112ecc[];
extern u8 data_ov018_02113588[];
extern u8 data_ov018_02113390[];
extern u8 data_ov018_02112fc8[];
extern u8 data_ov018_02112e88[];
extern u8 data_ov018_02113090[];
extern u8 data_ov018_02112f68[];
extern u8 data_ov018_02112ff8[];
extern u8 data_ov018_02113028[];
extern u8 data_ov018_02112e98[];
extern u8 data_ov018_02112f88[];
extern u8 data_ov018_02112fa8[];

data_ov018_021130cc_Layout data_ov018_021130cc = {{
    0xf2f00000,
}};

data_ov018_02113120_Layout data_ov018_02113120 = {
    0x00001c02, &data_ov018_021131c8,    0x00000403, &data_ov018_02112f14[0],
    0x00000404, &data_ov018_02113058[0], 0x0000010b, &data_ov018_02112e80[0],
    0x00000501, &data_ov018_021130d0,    0x00000206, &data_ov018_02112edc[0],
    0x00000207, &data_ov018_02112eec[0], 0x00000109, &data_ov018_02112ec0[0],
    0x0000010e, &data_ov018_02112e84[0], 0x0000010a, &data_ov018_02112ecc[0],
};

data_ov018_021130d0_Layout data_ov018_021130d0 = {{
    0xf9c00000, 0xf63c1068, 0x40000000, 0x05080000, 0xf9c00000, 0xf70409c4, 0x40000000,
    0x00080000, 0xf9c00000, 0xf7cc09c4, 0x40000000, 0x00080000, 0xf9c00000, 0xf89409c4,
    0x40000000, 0x00080000, 0x0a280000, 0x10ccee6c, 0x20000000, 0x00180000,
}};

data_ov018_02113170_Layout data_ov018_02113170 = {
    0x00003000, &data_ov018_02113588[0], 0x00003f05, &data_ov018_02113390[0],
    0x00000320, &data_ov018_02112fc8[0], 0x00000125, &data_ov018_02112e88[0],
    0x00000440, &data_ov018_02113090[0], 0x00000260, &data_ov018_02112f68[0],
    0x00000380, &data_ov018_02112ff8[0], 0x000003a0, &data_ov018_02113028[0],
    0x000001a5, &data_ov018_02112e98[0], 0x000002c0, &data_ov018_02112f88[0],
    0x000002e0, &data_ov018_02112fa8[0],
};

data_ov018_021131c8_Layout data_ov018_021131c8 = {{
    0xf9c1faba, 0xf3b21180, 0x0f32fa1f, 0xef020226, 0xff3f1881, 0x1881f042, 0xf25efb57, 0xf76f1881,
    0x1881f538, 0xf7ccf44f, 0x09c41881, 0xfbe60a96, 0x07cb0898, 0x0a5a00fa, 0x05460658, 0x05940708,
    0x003208ca, 0x089804b0, 0x040cf31c, 0xf060047e, 0xfc4a0320, 0x030df254, 0xf47af768, 0xf60a02eb,
}};
