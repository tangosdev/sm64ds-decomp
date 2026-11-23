#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    void *f004;
} sSmartballSideSpritesAlt_Layout;
typedef char sSmartballSideSpritesAlt_size[sizeof(sSmartballSideSpritesAlt_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov006_02138a90_Layout;
typedef char data_ov006_02138a90_size[sizeof(data_ov006_02138a90_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov006_02138ab0_Layout;
typedef char data_ov006_02138ab0_size[sizeof(data_ov006_02138ab0_Layout) == 0x20 ? 1 : -1];

extern sSmartballSideSpritesAlt_Layout sSmartballSideSpritesAlt;
extern data_ov006_02138a90_Layout data_ov006_02138a90;
extern data_ov006_02138ab0_Layout data_ov006_02138ab0;

#pragma define_section sName ".data.gap32"                                                         \
                             ".data.gap32"                                                         \
                             ".data.gap32" far_data
#pragma section sName begin

sSmartballSideSpritesAlt_Layout sSmartballSideSpritesAlt = {
    &data_ov006_02138a90,
    &data_ov006_02138ab0,
};

data_ov006_02138a90_Layout data_ov006_02138a90 = {{
    0x81e800e8,
    0x0000300c,
    0x800880e8,
    0x00003010,
    0x81e84008,
    0x0000308c,
    0x40080008,
    0xffff3090,
}};

data_ov006_02138ab0_Layout data_ov006_02138ab0 = {{
    0x81e800e8,
    0x00003012,
    0x800880e8,
    0x00003016,
    0x81e84008,
    0x00003092,
    0x40080008,
    0xffff3096,
}};
