#include "types.h"

extern void *_ZN2G213GetBG2CharPtrEv(void);
extern void *_ZN2G212GetBG1ScrPtrEv(void);
extern void *_ZN2G212GetBG2ScrPtrEv(void);
extern u32 LoadCompressedFileAt(int fileID, void *target);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void *LoadFile(int handle);
extern void Deallocate(void *ptr);
extern int func_0200f0bc(void);
extern void SetBg1Offset(int a, int b);
extern void func_ov075_02116128(char *c, int i, int z);
extern void func_ov075_021165b0(char *c);
extern int func_0203da9c(void);
extern void func_02033390(int a);
extern void func_02032f9c(int a);
extern int func_02030958(int val);
extern void func_02033464(unsigned short a, signed char b, int c);
extern void func_ov075_0211a194(char *self, int *src);
extern void *_ZN7dBase_c5SpawnEjP7fBase_cii(u32 a, void *b, int c, int d);

extern u8 data_0209fc50;
extern int data_ov075_0211c990[];
extern int data_ov075_0211c97c[];
extern u8 data_0209b2f0[];
extern int data_ov075_0211d7e0[];
extern void *data_0209f5c0;

int func_ov075_02116818(char *c)
{
    int k;
    void *tmp;
    int id0;
    int id1;
    int i;
    int z;
    int found;
    int j;
    int acc;
    int base;
    int step;
    void *sub;
    int n;

    *(volatile u16 *)0x400000a = (u16)((*(volatile u16 *)0x400000a & 0x43) | 0x208);
    *(volatile u16 *)0x400000a = (u16)(*(volatile u16 *)0x400000a & ~0x40);

    LoadCompressedFileAt(0x8413, _ZN2G213GetBG2CharPtrEv());
    tmp = LoadFile(0x8414);
    _ZN2GX10LoadBGPlttEPKvjj(tmp, 0, 0x1c0);
    Deallocate(tmp);

    n = data_0209fc50;
    if (n == 2) {
        id0 = 0x840c;
        id1 = 0x840d;
    } else if (n == 3) {
        id0 = 0x840e;
        id1 = 0x840f;
    } else {
        id0 = 0x8410;
        id1 = 0x8411;
    }
    LoadCompressedFileAt(id0, _ZN2G212GetBG1ScrPtrEv());
    LoadCompressedFileAt(id1, _ZN2G212GetBG2ScrPtrEv());

    i = 0;
    if (n > 0) {
        z = i;
        do {
            func_ov075_02116128(c, i, z);
            i = i + 1;
        } while (i < n);
    }

    LoadCompressedFileAt(data_ov075_0211c990[func_0200f0bc()], (void *)0x6400000);
    tmp = LoadFile(0x8415);
    _ZN2GX11LoadOBJPlttEPKvjj(tmp, 0, 0x140);
    Deallocate(tmp);

    *(volatile u16 *)0x400000a = (u16)((*(volatile u16 *)0x400000a & ~3) | 2);
    SetBg1Offset(0, 0);
    sub = (void *)0x6600000;
    LoadCompressedFileAt(0x8402, sub);
    LoadCompressedFileAt(data_ov075_0211c97c[func_0200f0bc()], (char *)sub + 0x6000);
    tmp = LoadFile(0x8403);
    _ZN3GXS11LoadOBJPlttEPKvjj(tmp, 0, 0x200);
    Deallocate(tmp);

    func_ov075_021165b0(c);

    found = -1;
    if (data_0209b2f0[func_0203da9c()] != 0) {
        found = func_0203da9c();
    } else {
        j = 0;
        do {
            if (data_0209b2f0[(u8)j] != 0) {
                found = j;
                break;
            }
            j = j + 1;
        } while (j < 4);
    }
    if (found >= 0) {
        func_02032f9c((s8)(u8)found);
    } else {
        func_02033390(1);
    }

    k = 0;
    if (n > 0) {
        acc = k;
        step = (4 - n) * 0x20 + 0xa0;
        base = (4 - n) * 0x40 + 0xa5;
        do {
            unsigned char ch = (unsigned char)func_02030958(k);
            func_02033464((unsigned short)(base + acc), (signed char)ch, 1);
            k = k + 1;
            acc = acc + step;
        } while (k < n);
    }

    func_ov075_0211a194(c, data_ov075_0211d7e0);
    *(void **)(c + 0x50) = _ZN7dBase_c5SpawnEjP7fBase_cii(0x15a, data_0209f5c0, 1, 0);
    return 1;
}
