//cpp
/* Runtime asset handles resolved through the overlay 0 file table. */
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx_load.h"
#include "nitro/mi/memory.h"
namespace s20adc4c { int Mg_GetWorkBuffer(void); }
using s20adc4c::Mg_GetWorkBuffer;

enum LoadFont_AssetId {
    ASSET_D_2D_MARIO_3DFONT_NCL_BIN = 0x0237
};

extern "C" void *LoadFile(int handle);
extern "C" int LoadFileAt(int handle, int dest);
extern "C" void Deallocate(void *ptr);

extern "C" void DecompressLZ16(void *dst, void *src);

extern unsigned char data_0209d698;
extern int data_0209d6f8;
extern unsigned char data_0209d660;
extern short data_0209d6d4;

extern "C" void LoadFont(unsigned char arg);
extern "C" void LoadFont(unsigned char arg)
{
    void *file;
    void *r4;
    data_0209d698 = arg;
    r4 = LoadFile(0x980e);
    if (data_0209d698 == 2) {
        LoadFileAt(0x980e, Mg_GetWorkBuffer());
        data_0209d6f8 = Mg_GetWorkBuffer();
        file = LoadFile(ASSET_D_2D_MARIO_3DFONT_NCL_BIN);
        GX_LoadBGPltt((char *)file + 2, 2, 0x20);
        Deallocate(file);
    } else if (data_0209d698 == 1) {
        MI_CpuCopy32(r4, (int *)G2S_GetBG1CharPtr(), 0x4000);
        file = LoadFile(ASSET_D_2D_MARIO_3DFONT_NCL_BIN);
        GXS_LoadBGPltt(file, 0x40, 0x20);
        Deallocate(file);
    } else {
        MI_CpuCopy32(r4, (int *)G2_GetBG3CharPtr(), 0x4000);
        file = LoadFile(0x980d);
        DecompressLZ16(file, (void *)((int)G2_GetBG3CharPtr() + 0x8000));
        Deallocate(file);
    }
    Deallocate(r4);
    data_0209d660 = 0;
    data_0209d6d4 = -1;
}
