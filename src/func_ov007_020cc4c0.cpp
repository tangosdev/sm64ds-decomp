//cpp
// @symbol func_ov007_020cc4c0
// recovered name: dScDSMT_c_InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScDSMT_c::InitResources - recovered from vtable slot identity */
struct FaderBrightness;

struct GX {
    static void DispOn();
    static void DisableAllBanks();
};
struct Scene {
    void SetFaders(FaderBrightness *fb);
};
struct Sound {
    static void LoadInitialGroup(int);
};

extern "C" void Enable3dEngines(void);
extern "C" void func_ov007_020b7090(int a, int b, int c, int d, int e);

extern unsigned char data_0209f1e0;
extern int data_0209d4a8;
extern int data_0208ee44;

extern "C" int func_ov007_020cc4c0(char *self)
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
    *(unsigned short *)0x4000304 = (*(unsigned short *)0x4000304 & 0xfffffdf1) | 0x20e;
    GX::DispOn();
    *(unsigned int *)0x4001000 |= 0x10000;
    Enable3dEngines();
    Initialise3dGraphics(0);
    data_0209d4a8 = (int)(self + 0x50);
    ((Scene *)(self + 0x54))->SetFaders((FaderBrightness *)(self + 0x50));
    data_0208ee44 = 1;
    GX::DisableAllBanks();
    data_ov007_02103260 = -1;
    func_ov007_020b7138(&data_ov007_02103290, data_0209b340);
    func_ov007_020b7090(0, 0, 0, 0, 0);
    Sound::LoadInitialGroup(2);
    return 1;
}
