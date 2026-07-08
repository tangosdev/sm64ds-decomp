//cpp
extern "C" int LoadFile(int handle);
extern "C" int func_ov004_020adc4c(void);
extern "C" int LoadFileAt(int handle, int dest);
extern "C" void Deallocate(void *ptr);
extern "C" unsigned int func_02054e88(void);
extern "C" int func_02054d88(void);
extern "C" void MultiCopy_Int(int *dst, int *src, int len);
extern "C" void DecompressLZ16(void *dst, void *src);

struct GX  { static void LoadBGPltt(void const *p, unsigned a, unsigned b); };
struct GXS { static void LoadBGPltt(void const *p, unsigned a, unsigned b); };

extern unsigned char data_0209d698;
extern int data_0209d6f8;
extern unsigned char PLAYER_TALKING;
extern short CURR_MSG_ID;

extern "C" void LoadFont(unsigned char arg);
extern "C" void LoadFont(unsigned char arg)
{
    int file;
    int r4;
    data_0209d698 = arg;
    r4 = LoadFile(0x980e);
    if (data_0209d698 == 2) {
        LoadFileAt(0x980e, func_ov004_020adc4c());
        data_0209d6f8 = func_ov004_020adc4c();
        file = LoadFile(0x237);
        GX::LoadBGPltt((void *)(file + 2), 2, 0x20);
        Deallocate((void *)file);
    } else if (data_0209d698 == 1) {
        MultiCopy_Int((int *)r4, (int *)func_02054e88(), 0x4000);
        file = LoadFile(0x237);
        GXS::LoadBGPltt((void *)file, 0x40, 0x20);
        Deallocate((void *)file);
    } else {
        MultiCopy_Int((int *)r4, (int *)func_02054d88(), 0x4000);
        file = LoadFile(0x980d);
        DecompressLZ16((void *)file, (void *)(func_02054d88() + 0x8000));
        Deallocate((void *)file);
    }
    Deallocate((void *)r4);
    PLAYER_TALKING = 0;
    CURR_MSG_ID = -1;
}
