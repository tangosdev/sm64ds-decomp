#include "types.h"
extern s32 func_0200f0bc(void);
extern unsigned _ZN3G2S13GetBG2CharPtrEv(void);
extern u32 LoadCompressedFileAt(u32 fileID, void *target);
extern char *_ZN3G2S13GetBG3CharPtrEv(void);
extern char *_ZN3G2S12GetBG2ScrPtrEv(void);
extern char *_ZN3G2S12GetBG3ScrPtrEv(void);

extern u32 data_0208a10c[];
extern u32 data_0208a134[];

void func_0200f220(void)
{
    s32 idx;
    idx = func_0200f0bc();
    LoadCompressedFileAt(data_0208a10c[idx], (void *)_ZN3G2S13GetBG2CharPtrEv());
    idx = func_0200f0bc();
    LoadCompressedFileAt(data_0208a134[idx], _ZN3G2S13GetBG3CharPtrEv() + 0x6000);
    LoadCompressedFileAt(0x8405, _ZN3G2S13GetBG3CharPtrEv() + 0x3000);
    LoadCompressedFileAt(0x9802, _ZN3G2S12GetBG2ScrPtrEv() + 0x800);
    LoadCompressedFileAt(0x9800, _ZN3G2S12GetBG2ScrPtrEv() + 0x2000);
    LoadCompressedFileAt(0x8408, _ZN3G2S12GetBG2ScrPtrEv() + 0x2800);
    LoadCompressedFileAt(0x8407, _ZN3G2S12GetBG3ScrPtrEv());
}
