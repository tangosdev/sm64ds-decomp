//cpp
// @symbol _ZN11dScMiniGm_c8BehaviorEv
/* recovered: named members + real C++ method */
/* dScMiniGm_c::Behavior() -- vtable slot 6. Runs the minigame-menu frame: a
 * touch anywhere plays the click, an active minigame (data_0209b300 == 1) takes
 * the whole frame, and otherwise three countdowns run in priority order --
 * unk_090 and unk_094 each flip the page direction when they expire, unk_098
 * closes the menu entirely. With none pending, unk_08c and unk_09c free-run as
 * 0..0x3f animation phases. */
#include "dScMiniGm_c.h"
#include "decl_Scene.h"
#include "decl_common.h"

extern "C" {
int func_02012790(int);
int RandomIntInternal(int *seed);
void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int);

extern unsigned char data_020a0e40;
extern unsigned short data_020a0e5a[];
extern unsigned char data_0209b300;
extern int data_0209e650;
extern unsigned char data_0209b304;
}

s32 dScMiniGm_c::Behavior()
{
    if ((data_020a0e5a[data_020a0e40 << 1] & 0xfff) != 0) {
        func_02012790(0xe);
    }
    if (data_0209b300 == 1) {
        func_ov005_020bff4c((char *)this);
        return 1;
    }
    RandomIntInternal(&data_0209d4b8);
    RandomIntInternal(&data_0209e650);

    if (unk_090 > 0) {
        unk_090 -= 1;
        if (unk_090 == 0) {
            data_0209b304 = 0;
            unk_054 = 1;
        }
    } else if (unk_094 > 0) {
        unk_094 -= 1;
        if (unk_094 == 0) {
            data_0209b304 = 1;
            unk_054 = 1;
        }
    } else if (unk_098 > 1) {
        unk_098 -= 1;
        if (unk_098 == 1) {
            _ZN8dScene_c20SetAndStopColorFaderEv();
            ExitMinigameMenu();
            _ZN5Sound22StopLoadedMusic_Layer1Ej(0x1e);
            unk_0ac = 1;
            return 1;
        }
    } else {
        unk_08c += 1;
        if (unk_08c >= 0x40) unk_08c = 0;
        unk_09c += 1;
        if (unk_09c >= 0x40) unk_09c = 0;
    }
    func_ov005_020c0878((char *)this);
    func_ov005_020c06cc((char *)this);
    func_ov005_020c0378((char *)this);
    func_ov005_020c0250((char *)this);
    func_ov005_020c0140((char *)this);
    return 1;
}
