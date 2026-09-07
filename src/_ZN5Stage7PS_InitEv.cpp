//cpp
// @symbol _ZN5Stage7PS_InitEv
/* recovered: real C++ method -- named members, local shadow class
 *
 * Called as a bare `PS_Init()` from Stage::Behavior (src/_ZN5Stage8BehaviorEv.cpp),
 * with no `this` placed in r0 at the call site -- static, matching every other
 * member of the pause-screen/menu family Stage.h already declares static.
 *
 * NOT declared in the real Stage.h on purpose. include/Stage.h documents why:
 * there is an untracked second file for this same symbol, src/_ZN5Stage7PS_InitEv.cpp,
 * that delinks.txt never named and nothing has ever compiled, whose private
 * `struct G2x` mangles SetBlendBrightness's middle parameter as `int` instead of
 * the real `...EPVtts` and resolves to nothing. This file avoids that landmine by
 * not declaring a G2x type at all -- SetBlendBrightness is reached through its
 * already-mangled extern symbol directly, exactly as the .c file it replaces did.
 */
#include "types.h"

class Stage {
public:
    static void PS_Init();
};

extern "C" {
extern u8 data_0209d454;
extern u8 data_0209d45c;
extern u8 data_0209f1ec;
extern u8 data_0209f210;
extern u8 data_0209f218;
extern u8 data_0209f22c;
extern u8 data_0209f230;
extern u8 data_0209f238;
extern u8 data_0209f23c;
extern u8 data_0209f240;
extern u8 data_0209f244;
extern u8 data_0209f248;
extern u8 data_0209f280;
extern u8 data_0209f29c;
extern u8 data_0209f2c8;
extern u8 data_0209f2cc;
extern u8 data_0209f2d8;
extern u8 data_0209f2e0;
extern u8 data_0209f2f0;
extern s8 data_0209f2f8;
extern u16 data_0209f300;
extern s32 data_0209fc68;

extern int SublevelToLevel(int i);
extern void _ZN5Sound10PauseMusicEv(void);
extern void _ZN3G2x18SetBlendBrightnessEPVtts(volatile u16 *p, int a, int b);
}

void Stage::PS_Init()
{
    int sl;
    u8 r7;
    u8 r8;
    u8 sb;
    int r5;
    int r4;

    sb = data_0209d454;
    if (sb & 4) {
        data_0209d454 = sb & ~4;
        sb = data_0209d454;
        *(volatile s32 *)0x04001000 =
            (*(volatile s32 *)0x04001000 & ~0x1f00) | (sb << 8);
    }

    r7 = data_0209f218;
    r8 = data_0209d45c;
    sl = 0;

    data_0209f248 = 0;
    data_0209f2e0 = 0;
    data_0209f2c8 = 0;
    data_0209f230 = 0;
    data_0209f2f0 = 0;
    data_0209f210 = 0;
    data_0209f22c = 0;
    data_0209f240 = 0;
    data_0209f238 = 0;
    data_0209f2cc = 0;
    data_0209f300 = 0xf;
    data_0209f23c = 0;
    data_0209f244 = 0;
    data_0209f29c = 0;
    data_0209f280 = 0;

    r5 = r8 ^ 0x10;
    r4 = sb ^ 0x10;

    if (r7 != 0) {
        data_0209f1ec = 7;
    } else {
        if (data_0209f2d8 == 1)
            sl = 1;
        if (sl != 0 && data_0209fc68 == 0) {
            data_0209f1ec = 4;
        } else {
            if (SublevelToLevel(data_0209f2f8) == 0x1d)
                data_0209f1ec = 0;
            else
                data_0209f1ec = 2;
        }
    }

    _ZN5Sound10PauseMusicEv();
    _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16 *)0x04000050, r5 | 0x20, -7);
    _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16 *)0x04001050, r4 | 0x20, -7);
}
