//cpp
/* Runtime asset handles resolved through the overlay 0 file table. */

namespace G2S {
unsigned int GetBG1CharPtr(void);
}
enum LoadFont_AssetId {
    ASSET_D_2D_MARIO_3DFONT_NCL_BIN = 0x0237
};

extern "C" int LoadFile(int handle);
extern "C" int Mg_GetWorkBuffer(void);
extern "C" int LoadFileAt(int handle, int dest);
extern "C" void Deallocate(void *ptr);

extern "C" int func_02054d88(void);
extern "C" void MultiCopy_Int(int *src, int *dst, int len);
extern "C" void DecompressLZ16(void *dst, void *src);

struct GX  { static void LoadBGPltt(void const *p, unsigned a, unsigned b); };
struct GXS { static void LoadBGPltt(void const *p, unsigned a, unsigned b); };

extern unsigned char data_0209d698;
extern int data_0209d6f8;
extern unsigned char data_0209d660;
extern short data_0209d6d4;

extern "C" void LoadFont(unsigned char arg);
extern "C" void LoadFont(unsigned char arg)
{
    int file;
    int r4;
    data_0209d698 = arg;
    r4 = LoadFile(0x980e);
    if (data_0209d698 == 2) {
        LoadFileAt(0x980e, Mg_GetWorkBuffer());
        data_0209d6f8 = Mg_GetWorkBuffer();
        file = LoadFile(ASSET_D_2D_MARIO_3DFONT_NCL_BIN);
        GX::LoadBGPltt((void *)(file + 2), 2, 0x20);
        Deallocate((void *)file);
    } else if (data_0209d698 == 1) {
        MultiCopy_Int((int *)r4, (int *)G2S::GetBG1CharPtr(), 0x4000);
        file = LoadFile(ASSET_D_2D_MARIO_3DFONT_NCL_BIN);
        GXS::LoadBGPltt((void *)file, 0x40, 0x20);
        Deallocate((void *)file);
    } else {
        MultiCopy_Int((int *)r4, (int *)func_02054d88(), 0x4000);
        file = LoadFile(0x980d);
        DecompressLZ16((void *)file, (void *)(func_02054d88() + 0x8000));
        Deallocate((void *)file);
    }
    Deallocate((void *)r4);
    data_0209d660 = 0;
    data_0209d6d4 = -1;
}
