typedef unsigned int u32;
typedef unsigned short u16;
extern int func_0200f0bc(void);
extern u16* _ZN3G2S12GetBG2ScrPtrEv(void);
extern u16* _ZN3G2S12GetBG3ScrPtrEv(void);
extern u32 LoadCompressedFileAt(u16 fileID, void* target);
extern int func_0203da9c(void);
extern int func_0203d974(void);
extern void func_02012790(int id);
extern unsigned char data_0209d454[];

void func_ov075_0211919c(char* self)
{
    int r6;
    int i;
    u16* p;
    u32 v;
    data_0209d454[0] &= ~5;
    if (func_0200f0bc() == 0)
        LoadCompressedFileAt(0x840a, _ZN3G2S12GetBG2ScrPtrEv());
    else
        LoadCompressedFileAt(0x980c, _ZN3G2S12GetBG2ScrPtrEv());
    LoadCompressedFileAt(0x840b, _ZN3G2S12GetBG3ScrPtrEv());
    p = _ZN3G2S12GetBG2ScrPtrEv() + 0x2d6 / 2;
    v = (u32)((func_0203da9c() + 0xa) << 0x1c) >> 0x10;
    for (r6 = 0; r6 < 10; r6++) {
        for (i = 0; i < 9; i++) {
            p[i] = v | (p[i] & 0xfff);
        }
        p += 0x40 / 2;
    }
    data_0209d454[0] |= 4;
    self[0x280] = 0;
    *(int*)(self + 0x264) = 0xb4;
    if (func_0203d974() != 0) {
        self[0x282] = 0;
        *(int*)(self + 0x268) = 0;
        *(int*)(self + 0x274) = 0;
    } else {
        self[0x282] = 1;
    }
    func_02012790(0x122);
}
