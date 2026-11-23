#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c458_Layout;
typedef char data_ov098_0213c458_size[sizeof(data_ov098_0213c458_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c460_Layout;
typedef char data_ov098_0213c460_size[sizeof(data_ov098_0213c460_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c468_Layout;
typedef char data_ov098_0213c468_size[sizeof(data_ov098_0213c468_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c470_Layout;
typedef char data_ov098_0213c470_size[sizeof(data_ov098_0213c470_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c478_Layout;
typedef char data_ov098_0213c478_size[sizeof(data_ov098_0213c478_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c480_Layout;
typedef char data_ov098_0213c480_size[sizeof(data_ov098_0213c480_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c488_Layout;
typedef char data_ov098_0213c488_size[sizeof(data_ov098_0213c488_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c490_Layout;
typedef char data_ov098_0213c490_size[sizeof(data_ov098_0213c490_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c498_Layout;
typedef char data_ov098_0213c498_size[sizeof(data_ov098_0213c498_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c4a0_Layout;
typedef char data_ov098_0213c4a0_size[sizeof(data_ov098_0213c4a0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c4a8_Layout;
typedef char data_ov098_0213c4a8_size[sizeof(data_ov098_0213c4a8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c4b0_Layout;
typedef char data_ov098_0213c4b0_size[sizeof(data_ov098_0213c4b0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c4b8_Layout;
typedef char data_ov098_0213c4b8_size[sizeof(data_ov098_0213c4b8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c4c0_Layout;
typedef char data_ov098_0213c4c0_size[sizeof(data_ov098_0213c4c0_Layout) == 0x8 ? 1 : -1];

extern data_ov098_0213c458_Layout data_ov098_0213c458;
extern data_ov098_0213c460_Layout data_ov098_0213c460;
extern data_ov098_0213c468_Layout data_ov098_0213c468;
extern data_ov098_0213c470_Layout data_ov098_0213c470;
extern data_ov098_0213c478_Layout data_ov098_0213c478;
extern data_ov098_0213c480_Layout data_ov098_0213c480;
extern data_ov098_0213c488_Layout data_ov098_0213c488;
extern data_ov098_0213c490_Layout data_ov098_0213c490;
extern data_ov098_0213c498_Layout data_ov098_0213c498;
extern data_ov098_0213c4a0_Layout data_ov098_0213c4a0;
extern data_ov098_0213c4a8_Layout data_ov098_0213c4a8;
extern data_ov098_0213c4b0_Layout data_ov098_0213c4b0;
extern data_ov098_0213c4b8_Layout data_ov098_0213c4b8;
extern data_ov098_0213c4c0_Layout data_ov098_0213c4c0;
extern u8 _ZN13daObjBlockS_c12EnterInMouthEv[];
extern u8 _ZN13daObjBlockS_c13UpdateInMouthEv[];
extern u8 _ZN13daObjBlockS_c13UpdateCarriedEv[];
extern u8 _ZN13daObjBlockS_c12UpdateThrownEv[];
extern u8 _ZN13daObjBlockS_c14UpdateBouncingEv[];
extern u8 _ZN13daObjBlockS_c13UpdateSpitOutEv[];
extern u8 _ZN13daObjBlockS_c10UpdateIdleEv[];
extern u8 _ZN13daObjBlockS_c12EnterCarriedEv[];
extern u8 _ZN13daObjBlockS_c11EnterThrownEv[];
extern u8 _ZN13daObjBlockS_c13EnterBouncingEv[];
extern u8 _ZN13daObjBlockS_c9EnterIdleEv[];
extern u8 _ZN13daObjBlockS_c12EnterSpitOutEv[];
extern u8 _ZN13daObjBlockS_c12UpdateBrokenEv[];
extern u8 _ZN13daObjBlockS_c11EnterBrokenEv[];

#pragma define_section sName ".data.blockHead"                                                     \
                             ".data.blockHead"                                                     \
                             ".data.blockHead" far_data
#pragma section sName begin

data_ov098_0213c4b8_Layout data_ov098_0213c4b8 = {
    &_ZN13daObjBlockS_c12UpdateBrokenEv[0],
    0x00000000,
};

data_ov098_0213c4b0_Layout data_ov098_0213c4b0 = {
    &_ZN13daObjBlockS_c12EnterSpitOutEv[0],
    0x00000000,
};

data_ov098_0213c4a8_Layout data_ov098_0213c4a8 = {
    &_ZN13daObjBlockS_c9EnterIdleEv[0],
    0x00000000,
};

data_ov098_0213c4a0_Layout data_ov098_0213c4a0 = {
    &_ZN13daObjBlockS_c13EnterBouncingEv[0],
    0x00000000,
};

data_ov098_0213c498_Layout data_ov098_0213c498 = {
    &_ZN13daObjBlockS_c11EnterThrownEv[0],
    0x00000000,
};

data_ov098_0213c490_Layout data_ov098_0213c490 = {
    &_ZN13daObjBlockS_c12EnterCarriedEv[0],
    0x00000000,
};

data_ov098_0213c488_Layout data_ov098_0213c488 = {
    &_ZN13daObjBlockS_c10UpdateIdleEv[0],
    0x00000000,
};

data_ov098_0213c480_Layout data_ov098_0213c480 = {
    &_ZN13daObjBlockS_c13UpdateSpitOutEv[0],
    0x00000000,
};

data_ov098_0213c478_Layout data_ov098_0213c478 = {
    &_ZN13daObjBlockS_c14UpdateBouncingEv[0],
    0x00000000,
};

data_ov098_0213c470_Layout data_ov098_0213c470 = {
    &_ZN13daObjBlockS_c12UpdateThrownEv[0],
    0x00000000,
};

data_ov098_0213c468_Layout data_ov098_0213c468 = {
    &_ZN13daObjBlockS_c13UpdateCarriedEv[0],
    0x00000000,
};

data_ov098_0213c460_Layout data_ov098_0213c460 = {
    &_ZN13daObjBlockS_c13UpdateInMouthEv[0],
    0x00000000,
};

data_ov098_0213c458_Layout data_ov098_0213c458 = {
    &_ZN13daObjBlockS_c12EnterInMouthEv[0],
    0x00000000,
};

data_ov098_0213c4c0_Layout data_ov098_0213c4c0 = {
    &_ZN13daObjBlockS_c11EnterBrokenEv[0],
    0x00000000,
};
