#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    void *f004;
    void *f008;
    void *f00c;
    void *f010;
    void *f014;
    void *f018;
    void *f01c;
    void *f020;
    void *f024;
    void *f028;
    u32 f02c;
    void *f030;
} data_ov030_02115bc8_Layout;
typedef char data_ov030_02115bc8_size[sizeof(data_ov030_02115bc8_Layout) == 0x34 ? 1 : -1];

extern data_ov030_02115bc8_Layout data_ov030_02115bc8;
extern u8 data_ov030_02115cf0[];
extern u8 gMonkeyWait4AnimationFile[];
extern u8 gMonkeyWait2AnimationFile[];
extern u8 gMonkeyWait3AnimationFile[];
extern u8 _ZTI7daMky_c[];

data_ov030_02115bc8_Layout data_ov030_02115bc8 = {
    &data_ov030_02115cf0[0],
    &gMonkeyWait4AnimationFile[0],
    &data_ov030_02115cf0[0],
    &gMonkeyWait2AnimationFile[0],
    &gMonkeyWait2AnimationFile[0],
    &gMonkeyWait2AnimationFile[0],
    &gMonkeyWait2AnimationFile[0],
    &gMonkeyWait3AnimationFile[0],
    &gMonkeyWait4AnimationFile[0],
    &gMonkeyWait3AnimationFile[0],
    &gMonkeyWait4AnimationFile[0],
    0x00000000,
    &_ZTI7daMky_c[0],
};
