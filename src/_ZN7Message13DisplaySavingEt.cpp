//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

struct StarEntry {
    int m0;
    u16 m4;
    u8 m6;
};

extern "C" {
extern s16 data_0209d6d4;
extern u8 data_0209d67c;
extern u8 data_0209d660;
extern u8 data_0209d654;
extern u8 data_0209d658;
extern u8 data_0209d64c;
extern u8 data_0209d6c4;
extern StarEntry* data_0209d708;
extern int* data_0209d70c;
extern int data_0209d6fc;
extern StarEntry* data_0209d6f0;
extern int data_0209d6f4;
extern u8 data_0209d6a8;
extern u8 data_0209d650;
extern u8 data_0209d6d0;
extern u8 data_0209d6c8;
extern u8 data_0209d6cc;
extern u8 data_0209d680;
extern u8 data_0209d65c;
extern u8 data_0209d6bc;
extern u8 data_0209d45c;
extern unsigned int data_0209b454;

void func_0201eaac(void);
void func_02012790(int id);
int func_02054d88(void);
void MultiStore_Int(int a, int b, int n);
int func_02054fb0(void);
void MultiStore16(int a, int b, int n);
void SetBg3Offset(int a, int b);
void _ZN7Message6UpdateEv(void);
void _ZN3G2x18SetBlendBrightnessEPVtts(void *p, int val, int amt);
}

extern "C" void _ZN7Message13DisplaySavingEt(unsigned short n)
{
    volatile int li;
    volatile unsigned short ls;
    int p;
    int s;
    u8* scr;
    int i;
    int cols;
    int rem;
    int idx;
    int one;
    int v7f;
    int v87;
    int v43;
    unsigned w;
    StarEntry* base;
    StarEntry* entry;

    data_0209d6d4 = (s16)n;
    func_0201eaac();

    one = 1;
    v7f = 0x7f;
    v87 = 0x87;
    data_0209d67c = 0x3c;
    data_0209d660 = (u8)one;
    data_0209d654 = (u8)one;
    data_0209d658 = (u8)v7f;
    data_0209d64c = (u8)v87;
    data_0209d6c4 = 0;
    func_02012790(4);

    idx = data_0209d6d4;
    base = data_0209d708;
    entry = (StarEntry*)((char*)base + idx * 8);
    data_0209d6f0 = entry;
    data_0209d6f4 = data_0209d6fc + 0x28 + data_0209d70c[1] + *(int*)((char*)base + idx * 8);

    w = entry->m4;
    w = w >> 3;
    data_0209d6a8 = (u8)((int)(w * 9 + 7) / 8);
    data_0209d650 = (u8)(((0x20 - ((int)data_0209d6a8 + 2)) / 2) << 3);
    data_0209d6d0 = (u8)v7f;
    v43 = 0x43;
    data_0209d6c8 = (u8)v43;
    data_0209d6cc = (u8)(0x44 - (int)(((((int)entry->m6 << 1) + 1) << 3) / 2));
    data_0209d658 = (u8)v7f;
    data_0209d64c = (u8)v43;
    data_0209d680 = 0;
    data_0209d65c = 0;

    p = func_02054d88() + 0x4000;
    li = 0;
    MultiStore_Int(li, p, 0x2000);

    s = func_02054fb0();
    ls = 0x2ff;
    MultiStore16(ls, s, 0x800);

    scr = (u8*)(func_02054fb0() + 0x42);
    SetBg3Offset(-(int)data_0209d650, 4 - (int)data_0209d6cc);

    cols = data_0209d6a8;
    i = 0;
    if ((cols * (int)data_0209d6f0->m6) * 2 > 0) {
        int last = cols - 1;
        do {
            rem = i % cols;
            *(u16*)(scr + (rem << 1)) = (u16)(i + 0x200);
            if (last == rem)
                scr += 0x40;
            i++;
        } while (i < (cols * (int)data_0209d6f0->m6) * 2);
    }

    while (data_0209d6bc < 4)
        _ZN7Message6UpdateEv();

    {
        int b = data_0209d45c | 0x20;
        data_0209b454 |= 0x80000000u;
        _ZN3G2x18SetBlendBrightnessEPVtts((void *)0x4000050, b, -8);
    }

    {
        volatile u16 *win0h = (volatile u16 *)0x4000048;
        volatile u16 *win0v = (volatile u16 *)0x400004a;
        u8 d = data_0209d45c;
        *win0h = (u16)((*win0h & ~0x3f) | (d | 8) | 0x20);
        *win0v = (u16)((*win0v & ~0x3f) | 0x17);
    }

    *(volatile u16 *)0x4000040 =
        (u16)(((data_0209d658 << 8) & 0xff00) |
              ((data_0209d6d0 * 2 - data_0209d658 + 1) & 0xff));
    *(volatile u16 *)0x4000044 =
        (u16)(((data_0209d64c << 8) & 0xff00) |
              ((data_0209d6c8 * 2 - data_0209d64c + 1) & 0xff));

    {
        volatile unsigned int *disp = (volatile unsigned int *)0x4000000;
        *disp = (*disp & ~0xe000u) | 0x2000u;
        data_0209d45c |= 8;
        *disp = (*disp & ~0x1f00u) | ((unsigned int)data_0209d45c << 8);
    }
}
