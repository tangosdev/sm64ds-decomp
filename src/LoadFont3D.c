#include "types.h"
extern void* LoadFile(u32 fileID);
extern void Deallocate(void* ptr);
extern void* func_02054d88(void);
extern void MultiCopy_Int(void* src, void* dest, u32 size);
extern void DecompressLZ16(void* src, void* dest);
extern void _ZN7Message10LoadTextVSEv(void);

extern u8 data_0209fc9c;
extern s16 data_0209fce8;

void LoadFont3D(void)
{
    void* r5;
    void* r4;

    r5 = LoadFile(0x980e);
    MultiCopy_Int(r5, func_02054d88(), 0x4000);

    r4 = LoadFile(0x980d);
    DecompressLZ16(r4, (u8*)func_02054d88() + 0x8000);

    Deallocate(r4);
    Deallocate(r5);

    _ZN7Message10LoadTextVSEv();

    data_0209fc9c = 0;
    data_0209fce8 = -1;
}
