//cpp
// @symbol _ZN9dScDSMT_c13InitResourcesEv
#include "dScDSMT_c.h"
#include "decl_common.h"

namespace GX {
void DispOn();
void DisableAllBanks();
}

namespace Sound {
void LoadInitialGroup(int group);
}

extern "C" {
void Enable3dEngines();
void func_ov007_020b7090(int a, int b, int c, int d, int e);
}

extern u8 data_0209f1e0;
extern int data_0209d4a8;
extern int data_0208ee44;

s32 dScDSMT_c::InitResources()
{
    if (data_0209f1e0 != 0)
        data_0209b340[1] = 1;

    func_02023544();
    UnloadArchives();
    func_02017e94((int)&overlay_100);
    func_02017e94((int)&overlay_102);

    if (data_0209d524 != 0)
        func_0201a428();

    data_0209b340[0x27] = data_0209d6fc;
    *(volatile u16 *)0x4000304 =
        (*(volatile u16 *)0x4000304 & 0xfffffdf1) | 0x20e;
    GX::DispOn();
    *(volatile u32 *)0x4001000 |= 0x10000;
    Enable3dEngines();
    ::Initialise3dGraphics(0);
    data_0209d4a8 = (int)&unk_050;
    dScene_c::SetFaders(&fader);
    data_0208ee44 = 1;
    GX::DisableAllBanks();
    data_ov007_02103260 = -1;
    func_ov007_020b7138(&data_ov007_02103290, data_0209b340);
    func_ov007_020b7090(0, 0, 0, 0, 0);
    Sound::LoadInitialGroup(2);
    return 1;
}
