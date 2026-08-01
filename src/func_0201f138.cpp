//cpp
#include "types.h"
struct StarEntry {
    int m0;
    u16 m4;
    u8 m6;
};

extern "C" {
extern short data_0209d6d4;
extern u8 data_0209d658;
extern u8 data_0209d64c;
extern u8 data_0209d660;
extern u8 data_0209d6c4;
extern StarEntry* data_0209d708;
extern StarEntry* data_0209d6f0;
extern int* data_0209d70c;
extern int data_0209d6fc;
extern int data_0209d6f4;
extern u8 data_0209d680;
extern u8 data_0209d65c;
extern u8 data_0209d6a8;
extern u8 data_0209d6bc;
extern u8 data_0209d45c;

void func_0201eaac(void);
int func_02054d88(void);
void MultiStore_Int(int a, int b, int n);
int _ZN2G212GetBG3ScrPtrEv(void);
void MultiStore16(int a, int b, int n);
void SetBg3Offset(int a, int b);
}

void _ZN7Message6UpdateEv(void);

extern "C" void func_0201f138(void)
{
    volatile int li;
    volatile unsigned short ls;
    int idx;
    int p, s;
    u16* q;
    int i, cnt, rem, div;
    u8* bcPtr;

    data_0209d6d4 = 0;
    func_0201eaac();

    idx = data_0209d6d4;
    data_0209d658 = 0x7f;
    data_0209d64c = 0x87;
    data_0209d660 = 0;
    data_0209d6c4 = 1;
    data_0209d6f0 = (StarEntry*)((char*)data_0209d708 + idx * 8);
    data_0209d6f4 = data_0209d6fc + 0x28 + data_0209d70c[1] + *(int*)((char*)data_0209d708 + idx * 8);

    data_0209d6a8 = (u8)((((data_0209d6f0->m4 / 8) * 9) + 7) / 8);
    data_0209d680 = 0;
    data_0209d65c = 0;

    p = func_02054d88() + 0x4000;
    li = 0;
    MultiStore_Int(li, p, 0x2000);

    s = _ZN2G212GetBG3ScrPtrEv();
    ls = 0x2ff;
    MultiStore16(ls, s, 0x800);

    q = (u16*)(_ZN2G212GetBG3ScrPtrEv() + 0x42);
    div = data_0209d6a8;
    cnt = div * data_0209d6f0->m6 * 2;
    for (i = 0; i < cnt; i++) {
        rem = i % div;
        q[rem] = (u16)(i + 0x200);
        if (div - 1 == rem) {
            q += 0x20;
        }
        cnt = div * data_0209d6f0->m6 * 2;
    }

    bcPtr = &data_0209d6bc;
    while (*bcPtr < 4) {
        _ZN7Message6UpdateEv();
    }
    SetBg3Offset(-0x11, -0x3c);
    data_0209d45c |= 8;
}
