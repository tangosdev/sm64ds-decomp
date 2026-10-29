//cpp
#include "types.h"
#include "G2.h"

extern "C" {

extern s32 func_0200f0bc(void);
extern u32 LoadCompressedFileAt(u32 fileID, void *target);

extern u32 data_0208a10c[];
extern u32 data_0208a134[];

void func_0200f220(void) {
    s32 idx;
    idx = func_0200f0bc();
    LoadCompressedFileAt(data_0208a10c[idx], G2S::GetBG2CharPtr());
    idx = func_0200f0bc();
    LoadCompressedFileAt(data_0208a134[idx], (char *)G2S::GetBG3CharPtr() + 0x6000);
    LoadCompressedFileAt(0x8405, (char *)G2S::GetBG3CharPtr() + 0x3000);
    LoadCompressedFileAt(0x9802, (char *)G2S::GetBG2ScrPtr() + 0x800);
    LoadCompressedFileAt(0x9800, (char *)G2S::GetBG2ScrPtr() + 0x2000);
    LoadCompressedFileAt(0x8408, (char *)G2S::GetBG2ScrPtr() + 0x2800);
    LoadCompressedFileAt(0x8407, G2S::GetBG3ScrPtr());
}
}
