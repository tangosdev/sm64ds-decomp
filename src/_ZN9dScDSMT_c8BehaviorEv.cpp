//cpp
// @symbol _ZN9dScDSMT_c8BehaviorEv
/* recovered: named members + real C++ method */
/* dScDSMT_c::Behavior() -- vtable slot 6. Pumps the download-play state
 * machine and dispatches on its return code: 3..5 load one of three saved
 * files, 6 pulls in the minigame overlays and fades out, 7 opens the minigame
 * menu, 2 fades back. Ends by handing its own `fader` member to dScene_c. */
#include "dScDSMT_c.h"
#include "decl_Heap.h"
#include "decl_Scene.h"
#include "decl_common.h"

extern "C" {
unsigned short DecIfAbove0_Short(unsigned short *p);
int func_ov007_020b7090(u16 a0, u16 a1, u16 a2, u16 a3, int arg4);
int _ZN4Heap10SetDefaultEv(int heap);
void _ZN8dScene_c14StartSceneFadeEjjt(unsigned int a, unsigned int b, u16 c);
void StartMinigameMenu(u8 returnToRecRoom);

extern char *data_0209b33c;
extern char data_0209caa0;
}

s32 dScDSMT_c::Behavior()
{
    int result;

    if (data_ov007_02103260 >= 0 && DecIfAbove0_Short(&data_ov007_02104c28) == 0) {
        _ZN5Sound22LoadAndSetMusic_Layer1Ei(data_ov007_02103260);
        data_ov007_02103260 = -1;
    }

    func_0203da9c();
    {
        u16 *p = func_0203dabc();
        int arg4;
        func_0203da9c();
        arg4 = func_0203dae4();
        result = func_ov007_020b7090(p[0], p[1], p[2], p[3], arg4);
    }

    if ((unsigned int)(result - 3) <= 2) {
        int idx = 0;
        if (result == 4) {
            idx = 1;
        } else if (result == 5) {
            idx = 2;
        }
        func_02013c84(idx, data_0209b33c + idx * 0x44, -1, &data_0209caa0);
        StartFile(1, 0);
    } else if (result == 6) {
        if (data_0209d524 == 0) {
            int h1 = func_0201a458();
            int h2 = _ZN4Heap10SetDefaultEv(h1);
            LoadArchive(1);
            LoadTextNarcs();
            _ZN4Heap6RescueEv(h1);
            _ZN4Heap10SetDefaultEv(h2);
            LoadOverlay((int)&overlay_64);
            LoadOverlay((int)&overlay_66);
        }
        _ZN8dScene_c14StartSceneFadeEjjt(6, 0, 0x7fff);
    } else if (result == 7) {
        StartMinigameMenu(0);
    } else if (result == 2) {
        _ZN8dScene_c14StartSceneFadeEjjt(1, 0, 0x7fff);
    }

    dScene_c::SetFaders(&fader);
    return 1;
}
