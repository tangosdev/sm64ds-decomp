#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} sSmartballTimerSprite_Layout;
typedef char sSmartballTimerSprite_size[sizeof(sSmartballTimerSprite_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov006_02138bd4_Layout;
typedef char data_ov006_02138bd4_size[sizeof(data_ov006_02138bd4_Layout) == 0x20 ? 1 : -1];

extern sSmartballTimerSprite_Layout sSmartballTimerSprite;
extern data_ov006_02138bd4_Layout data_ov006_02138bd4;

#pragma define_section sName ".data.gap32"                                                         \
                             ".data.gap32"                                                         \
                             ".data.gap32" far_data
#pragma section sName begin

sSmartballTimerSprite_Layout sSmartballTimerSprite = {
    &data_ov006_02138bd4,
};

data_ov006_02138bd4_Layout data_ov006_02138bd4 = {{
    0x81d440f8,
    0x000070cd,
    0x81f440f8,
    0x000070d1,
    0x401400f8,
    0x000070d5,
    0x002480f8,
    0xffff70d7,
}};
