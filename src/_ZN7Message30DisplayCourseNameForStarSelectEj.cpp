//cpp
// @symbol _ZN7Message30DisplayCourseNameForStarSelectEj
/* recovered: named members + shared header, named members + shared header */
#include "Message.h"
typedef unsigned char u8;
typedef unsigned short u16;

struct StarEntry {
    int m0;
    u16 m4;
    u8 m6;
};

extern "C" {
extern short data_0209d6d4;
extern u8 data_0209d660;
extern StarEntry* data_0209d708;
extern StarEntry* data_0209d6f0;
extern int* data_0209d70c;
extern int data_0209d6fc;
extern volatile int data_0209d6f4;
extern u8 data_0209d6c4;
extern u8 data_0209d65c;
extern u8 data_0209d6a8;
extern u8 data_0209d6b0;
extern u8 data_0209d6c0;

void func_0201eaac(void);
void func_0201b6f8(int a);
int func_02054d88(void);
void MultiStore_Int(int a, int b, int n);
int _ZN2G212GetBG3ScrPtrEv(void);
void MultiStore16(int a, int b, int n);
void SetSubBg1Offset(int a, int b);
void func_0201b7cc(void);
void func_0201b388(int a);
}

extern "C" void _ZN7Message30DisplayCourseNameForStarSelectEj(struct Message *self) {
    volatile unsigned short ls;
    volatile int li1;
    volatile int li2;
    int idx;
    int sum;
    u16* q;
    int div;
    int div2;

    data_0209d6d4 = (short)((unsigned int)&self->unk_196);
    data_0209d660 = 0;
    func_0201eaac();

    idx = data_0209d6d4;
    data_0209d6f0 = (StarEntry*)((char*)data_0209d708 + idx * 8);
    sum = data_0209d6fc + 0x28 + data_0209d70c[1] + *(int*)((char*)data_0209d708 + idx * 8);
    data_0209d6f4 = sum;
    data_0209d6f4 = sum + 3;
    func_0201b6f8(0);

    data_0209d65c = (u8)((0x100 - data_0209d6b0) / 2);
    data_0209d6a8 = 0x20;

    {
        int p1 = func_02054d88() + 0x4000;
        li1 = 0;
        MultiStore_Int(li1, p1, 0x2000);
    }

    {
        int s1 = _ZN2G212GetBG3ScrPtrEv();
        ls = 0x2ff;
        MultiStore16(ls, s1, 0x800);
    }

    if (data_0209d6d4 >= 0x1a5) {
        data_0209d6c4 = 0;
        q = (u16*)(_ZN2G212GetBG3ScrPtrEv() + 0x400);
    } else {
        data_0209d6c4 = 1;
        q = (u16*)(_ZN2G212GetBG3ScrPtrEv() + 0x4c0);
    }
    div = 0;
    do {
        q[div] = (u16)(div + 0x200);
        div++;
    } while (div < 0x40);

    SetSubBg1Offset(0, 0);
    func_0201b7cc();
    func_0201b388(data_0209d6a8);

    if (data_0209d6d4 >= 0x1a5) {
        return;
    }

    data_0209d6c0 = 1;
    {
        StarEntry* e = (StarEntry*)(int)(((long long)(int)((char*)data_0209d708 + 0x1470)));
        data_0209d6f0 = e;
        sum = data_0209d6fc + 0x28;
        sum += data_0209d70c[1];
        sum += *(int*)e;
        data_0209d6f4 = sum;
    }
    func_0201b6f8(0);

    data_0209d65c = (u8)((0xd2 - data_0209d6b0) / 2);
    data_0209d6a8 = 0x20;

    {
        int p2 = func_02054d88() + 0x4800;
        li2 = 0;
        MultiStore_Int(li2, p2, 0x800);
    }

    q = (u16*)(_ZN2G212GetBG3ScrPtrEv() + 0x240);
    div2 = 0;
    do {
        q[div2] = (u16)(div2 + 0x240);
        div2++;
    } while (div2 < 0x40);

    func_0201b7cc();
    func_0201b388(data_0209d6a8);
}
