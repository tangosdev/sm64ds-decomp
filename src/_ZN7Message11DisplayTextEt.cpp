//cpp
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
extern u8 data_0209d6c4;
extern u8 data_0209d6c0;
extern u8 data_0209d668;
extern StarEntry* data_0209d708;
extern StarEntry* data_0209d6f0;
extern int* data_0209d70c;
extern int data_0209d6fc;
extern int data_0209d6f4;
extern u8 data_0209d6b0;
extern u8 data_0209d65c;
extern u8 data_0209d6a8;
extern u8 data_0209d678;

void func_0201eaac(void);
int func_02054d88(void);
void MultiStore_Int(int a, int b, int n);
int func_02054fb0(void);
void MultiStore16(int a, int b, int n);
void func_0201b6f8(int a);
void func_0201b7cc(void);
void func_0201b388(int a);
}

extern "C" void _ZN7Message11DisplayTextEt(unsigned short t)
{
    volatile int li;
    volatile unsigned short ls;
    int idx;
    int p, s;
    int i;
    u16* q;
    int cnt, div;
    int base;

    data_0209d6d4 = (short)t;
    data_0209d660 = 0;
    func_0201eaac();

    base = data_0209d6fc;
    idx = data_0209d6d4;
    data_0209d6c4 = 1;
    data_0209d6c0 = 0;
    data_0209d668 = 0;
    data_0209d6a8 = 0;
    data_0209d6f0 = (StarEntry*)((char*)data_0209d708 + idx * 8);
    data_0209d6f4 = base + 0x28 + data_0209d70c[1] + *(int*)((char*)data_0209d708 + idx * 8);

    p = func_02054d88() + 0x4000;
    li = 0;
    MultiStore_Int(li, p, 0x2000);

    s = func_02054fb0();
    ls = 0x2ff;
    MultiStore16(ls, s, 0x800);

    if (t != 0x1b3) {
        func_0201b6f8(0);
        data_0209d6a8 = (u8)(((int)data_0209d6b0 + 7) / 8);
        data_0209d65c = 0;
        div = 0x107;
    } else {
        data_0209d6f4 += 3;
        func_0201b6f8(0);
        data_0209d65c = (u8)((0xb0 - (int)data_0209d6b0) >> 1);
        data_0209d6a8 = 0x16;
        data_0209d678 = data_0209d65c & 7;
        div = 0x105;
    }

    q = (u16*)(func_02054fb0() + div * 2);
    cnt = data_0209d6a8;
    for (i = 0; i < cnt; i++) {
        u16* q2 = q + 0x20;
        q[i] = (u16)(i + 0x200);
        q2[i] = (u16)(i + 0x200 + cnt);
    }

    func_0201b7cc();
    func_0201b388(data_0209d6a8);
}
