#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov006_021389f8_Layout;
typedef char data_ov006_021389f8_size[sizeof(data_ov006_021389f8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02138a00_Layout;
typedef char data_ov006_02138a00_size[sizeof(data_ov006_02138a00_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
} sSmartballPropellerSprites_Layout;
typedef char
    sSmartballPropellerSprites_size[sizeof(sSmartballPropellerSprites_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02138a10_Layout;
typedef char data_ov006_02138a10_size[sizeof(data_ov006_02138a10_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02138a18_Layout;
typedef char data_ov006_02138a18_size[sizeof(data_ov006_02138a18_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
} sSmartballSpringSprites_Layout;
typedef char sSmartballSpringSprites_size[sizeof(sSmartballSpringSprites_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02138a28_Layout;
typedef char data_ov006_02138a28_size[sizeof(data_ov006_02138a28_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02138a30_Layout;
typedef char data_ov006_02138a30_size[sizeof(data_ov006_02138a30_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
} sSmartballSpringSpritesAlt_Layout;
typedef char
    sSmartballSpringSpritesAlt_size[sizeof(sSmartballSpringSpritesAlt_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
} sSmartballSideSprites_Layout;
typedef char sSmartballSideSprites_size[sizeof(sSmartballSideSprites_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov006_02138a48_Layout;
typedef char data_ov006_02138a48_size[sizeof(data_ov006_02138a48_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov006_02138a68_Layout;
typedef char data_ov006_02138a68_size[sizeof(data_ov006_02138a68_Layout) == 0x20 ? 1 : -1];

extern data_ov006_021389f8_Layout data_ov006_021389f8;
extern data_ov006_02138a00_Layout data_ov006_02138a00;
extern sSmartballPropellerSprites_Layout sSmartballPropellerSprites;
extern data_ov006_02138a10_Layout data_ov006_02138a10;
extern data_ov006_02138a18_Layout data_ov006_02138a18;
extern sSmartballSpringSprites_Layout sSmartballSpringSprites;
extern data_ov006_02138a28_Layout data_ov006_02138a28;
extern data_ov006_02138a30_Layout data_ov006_02138a30;
extern sSmartballSpringSpritesAlt_Layout sSmartballSpringSpritesAlt;
extern sSmartballSideSprites_Layout sSmartballSideSprites;
extern data_ov006_02138a48_Layout data_ov006_02138a48;
extern data_ov006_02138a68_Layout data_ov006_02138a68;

#pragma define_section sName ".data.gap32"                                                         \
                             ".data.gap32"                                                         \
                             ".data.gap32" far_data
#pragma section sName begin

data_ov006_021389f8_Layout data_ov006_021389f8 = {{
    0xc1e000e0,
    0xffff1110,
}};

sSmartballSideSprites_Layout sSmartballSideSprites = {
    &data_ov006_02138a48,
    &data_ov006_02138a68,
};

sSmartballSpringSpritesAlt_Layout sSmartballSpringSpritesAlt = {
    &data_ov006_02138a30,
    &data_ov006_02138a28,
};

data_ov006_02138a30_Layout data_ov006_02138a30 = {{
    0xc1f080e0,
    0xffff2018,
}};

data_ov006_02138a28_Layout data_ov006_02138a28 = {{
    0xc1f080e0,
    0xffff101c,
}};

sSmartballSpringSprites_Layout sSmartballSpringSprites = {
    &data_ov006_02138a18,
    &data_ov006_02138a10,
};

data_ov006_02138a18_Layout data_ov006_02138a18 = {{
    0xc1f080e0,
    0xffff2018,
}};

data_ov006_02138a10_Layout data_ov006_02138a10 = {{
    0xc1f080e0,
    0xffff101c,
}};

sSmartballPropellerSprites_Layout sSmartballPropellerSprites = {
    &data_ov006_02138a00,
    &data_ov006_021389f8,
};

data_ov006_02138a00_Layout data_ov006_02138a00 = {{
    0xc1e000e0,
    0xffff2108,
}};

data_ov006_02138a48_Layout data_ov006_02138a48 = {{
    0x81e800e8,
    0x0000300c,
    0x800880e8,
    0x00003010,
    0x81e84008,
    0x0000308c,
    0x40080008,
    0xffff3090,
}};

data_ov006_02138a68_Layout data_ov006_02138a68 = {{
    0x81e800e8,
    0x00003012,
    0x800880e8,
    0x00003016,
    0x81e84008,
    0x00003092,
    0x40080008,
    0xffff3096,
}};
