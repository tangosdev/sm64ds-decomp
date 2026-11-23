#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} sSmartballTimerSpriteAlt_Layout;
typedef char sSmartballTimerSpriteAlt_size[sizeof(sSmartballTimerSpriteAlt_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov006_02138bf8_Layout;
typedef char data_ov006_02138bf8_size[sizeof(data_ov006_02138bf8_Layout) == 0x20 ? 1 : -1];

extern sSmartballTimerSpriteAlt_Layout sSmartballTimerSpriteAlt;
extern data_ov006_02138bf8_Layout data_ov006_02138bf8;

#pragma define_section sName ".data.gap32"                                                         \
                             ".data.gap32"                                                         \
                             ".data.gap32" far_data
#pragma section sName begin

sSmartballTimerSpriteAlt_Layout sSmartballTimerSpriteAlt = {
    &data_ov006_02138bf8,
};

data_ov006_02138bf8_Layout data_ov006_02138bf8 = {{
    0x81d440f8,
    0x000070cd,
    0x81f440f8,
    0x000070d1,
    0x401400f8,
    0x000070d5,
    0x002480f8,
    0xffff70d7,
}};
