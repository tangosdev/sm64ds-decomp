#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    void *f004;
    void *f008;
    void *f00c;
    u32 f010;
    void *f014;
} sPresenterLoseLoopTexAnim_Layout;
typedef char
    sPresenterLoseLoopTexAnim_size[sizeof(sPresenterLoseLoopTexAnim_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    void *f008;
    void *f00c;
    u32 f010;
    void *f014;
} sPresenterSmileTexAnim_Layout;
typedef char sPresenterSmileTexAnim_size[sizeof(sPresenterSmileTexAnim_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    void *f008;
    void *f00c;
    u32 f010;
    void *f014;
} sPresenterLoseTexAnim_Layout;
typedef char sPresenterLoseTexAnim_size[sizeof(sPresenterLoseTexAnim_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    void *f008;
    void *f00c;
    u32 f010;
    void *f014;
} sPresenterIdleTexAnim_Layout;
typedef char sPresenterIdleTexAnim_size[sizeof(sPresenterIdleTexAnim_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    void *f008;
    void *f00c;
    u32 f010;
    void *f014;
} sPresenterMissTexAnim_Layout;
typedef char sPresenterMissTexAnim_size[sizeof(sPresenterMissTexAnim_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    void *f008;
    void *f00c;
    u32 f010;
    void *f014;
} sPresenterTextureAnim_Layout;
typedef char sPresenterTextureAnim_size[sizeof(sPresenterTextureAnim_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    void *f008;
    void *f00c;
    u32 f010;
    void *f014;
} sPresenterCheerTexAnim_Layout;
typedef char sPresenterCheerTexAnim_size[sizeof(sPresenterCheerTexAnim_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    void *f008;
    void *f00c;
    u32 f010;
    void *f014;
} sPresenterMatchTexAnim_Layout;
typedef char sPresenterMatchTexAnim_size[sizeof(sPresenterMatchTexAnim_Layout) == 0x18 ? 1 : -1];

extern sPresenterLoseLoopTexAnim_Layout sPresenterLoseLoopTexAnim;
extern sPresenterSmileTexAnim_Layout sPresenterSmileTexAnim;
extern sPresenterLoseTexAnim_Layout sPresenterLoseTexAnim;
extern sPresenterIdleTexAnim_Layout sPresenterIdleTexAnim;
extern sPresenterMissTexAnim_Layout sPresenterMissTexAnim;
extern sPresenterTextureAnim_Layout sPresenterTextureAnim;
extern sPresenterCheerTexAnim_Layout sPresenterCheerTexAnim;
extern sPresenterMatchTexAnim_Layout sPresenterMatchTexAnim;
extern u8 data_ov006_0212b8e8[];
extern u8 data_ov006_0212b8bc[];
extern u8 data_ov006_0212bc74[];
extern u8 data_ov006_0212b940[];
extern u8 data_ov006_0212b8e4[];
extern u8 data_ov006_0212b8b4[];
extern u8 data_ov006_0212b8f4[];
extern u8 data_ov006_0212b908[];
extern u8 data_ov006_0212b8cc[];
extern u8 data_ov006_0212b8c4[];
extern u8 data_ov006_0212bddc[];
extern u8 data_ov006_0212b978[];
extern u8 data_ov006_0212b8e0[];
extern u8 data_ov006_0212b8ac[];
extern u8 data_ov006_0212c3cc[];
extern u8 data_ov006_0212b9b0[];
extern u8 data_ov006_0212b8d4[];
extern u8 data_ov006_0212b8b8[];
extern u8 data_ov006_0212bb58[];
extern u8 data_ov006_0212ba20[];
extern u8 data_ov006_0212b8dc[];
extern u8 data_ov006_0212b8b0[];
extern u8 data_ov006_0212bac8[];
extern u8 data_ov006_0212b9e8[];
extern u8 data_ov006_0212b8d8[];
extern u8 data_ov006_0212b8c8[];
extern u8 data_ov006_0212b8ec[];
extern u8 data_ov006_0212ba58[];
extern u8 data_ov006_0212b8d0[];
extern u8 data_ov006_0212b8c0[];
extern u8 data_ov006_0212c02c[];
extern u8 data_ov006_0212ba90[];

#pragma define_section sName ".data.gap45c"                                                        \
                             ".data.gap45c"                                                        \
                             ".data.gap45c" far_data
#pragma section sName begin

sPresenterCheerTexAnim_Layout sPresenterCheerTexAnim = {
    0x00000019, &data_ov006_0212b8d8[0], &data_ov006_0212b8c8[0], &data_ov006_0212b8ec[0],
    0x00000002, &data_ov006_0212ba58[0],
};

sPresenterTextureAnim_Layout sPresenterTextureAnim = {
    0x0000000c, &data_ov006_0212b8dc[0], &data_ov006_0212b8b0[0], &data_ov006_0212bac8[0],
    0x00000002, &data_ov006_0212b9e8[0],
};

sPresenterMissTexAnim_Layout sPresenterMissTexAnim = {
    0x00000046, &data_ov006_0212b8d4[0], &data_ov006_0212b8b8[0], &data_ov006_0212bb58[0],
    0x00000002, &data_ov006_0212ba20[0],
};

sPresenterIdleTexAnim_Layout sPresenterIdleTexAnim = {
    0x00000082, &data_ov006_0212b8e0[0], &data_ov006_0212b8ac[0], &data_ov006_0212c3cc[0],
    0x00000002, &data_ov006_0212b9b0[0],
};

sPresenterLoseTexAnim_Layout sPresenterLoseTexAnim = {
    0x00000025, &data_ov006_0212b8cc[0], &data_ov006_0212b8c4[0], &data_ov006_0212bddc[0],
    0x00000002, &data_ov006_0212b978[0],
};

sPresenterSmileTexAnim_Layout sPresenterSmileTexAnim = {
    0x00000007, &data_ov006_0212b8e4[0], &data_ov006_0212b8b4[0], &data_ov006_0212b8f4[0],
    0x00000002, &data_ov006_0212b908[0],
};

sPresenterLoseLoopTexAnim_Layout sPresenterLoseLoopTexAnim = {
    0x0000001e, &data_ov006_0212b8e8[0], &data_ov006_0212b8bc[0], &data_ov006_0212bc74[0],
    0x00000002, &data_ov006_0212b940[0],
};

sPresenterMatchTexAnim_Layout sPresenterMatchTexAnim = {
    0x0000003a, &data_ov006_0212b8d0[0], &data_ov006_0212b8c0[0], &data_ov006_0212c02c[0],
    0x00000002, &data_ov006_0212ba90[0],
};
