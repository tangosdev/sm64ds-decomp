//cpp
#include "types.h"
extern "C" {
s32 func_0200f0bc(void);
u32 LoadCompressedFileAt(unsigned int fileID, void *target);
void *LoadFile(u16 fileID);
void Deallocate(void *ptr);
}
namespace G2 { void *GetBG2CharPtr(); void *GetBG2ScrPtr(); }
namespace GX { void LoadOBJPltt(void const *, unsigned int, unsigned int); }
namespace GXS { void LoadOBJPltt(void const *, unsigned int, unsigned int); }
extern unsigned int data_0208a148[];
extern unsigned int data_0208a15c[];
extern unsigned int data_0208a120[];
extern "C" void func_0200f13c(void) {
    s32 i = func_0200f0bc();
    LoadCompressedFileAt(data_0208a148[i], G2::GetBG2CharPtr());
    LoadCompressedFileAt(0x9808, G2::GetBG2ScrPtr());
    LoadCompressedFileAt(0x9809, (char*)G2::GetBG2ScrPtr() + 0x800);
    LoadCompressedFileAt(0x980a, (char*)G2::GetBG2ScrPtr() + 0x1000);
    char *a5 = (char*)0x6400000; a5 += 0x2000;
    LoadCompressedFileAt(data_0208a15c[func_0200f0bc()], a5);
    void *p = LoadFile(0x980f);
    GX::LoadOBJPltt(p, 0x120, 0xe0);
    Deallocate(p);
    char *a6 = (char*)0x6600000; a6 += 0x800;
    LoadCompressedFileAt(data_0208a120[func_0200f0bc()], a6);
    void *q = LoadFile(0x8400);
    GXS::LoadOBJPltt(q, 0, 0x200);
    Deallocate(q);
}
