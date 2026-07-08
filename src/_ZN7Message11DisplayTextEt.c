//cpp
// NONMATCHING: register allocation (div=8). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned short u16;

struct StarEntry {
    int m0;
    u16 m4;
    u8 m6;
};

extern "C" {
extern short CURR_MSG_ID;
extern u8 PLAYER_TALKING;
extern u8 data_0209d6c4;
extern u8 data_0209d6c0;
extern u8 data_0209d668;
extern StarEntry* MSG_ARR_PTR;
extern StarEntry* CURR_MSG_PTR;
extern int* MSG_FILE_PTR;
extern int data_0209d6fc;
extern int CURR_MSG_TEXT_CHAR;
extern u8 data_0209d6b0;
extern u8 data_0209d65c;
extern u8 MSG_LINE_HEIGHT;
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
    u16* q;
    int i, cnt, div;
    int base;

    CURR_MSG_ID = (short)t;
    PLAYER_TALKING = 0;
    func_0201eaac();

    base = data_0209d6fc;
    idx = CURR_MSG_ID;
    data_0209d6c4 = 1;
    data_0209d6c0 = 0;
    data_0209d668 = 0;
    MSG_LINE_HEIGHT = 0;
    CURR_MSG_PTR = (StarEntry*)((char*)MSG_ARR_PTR + idx * 8);
    CURR_MSG_TEXT_CHAR = base + 0x28 + MSG_FILE_PTR[1] + *(int*)((char*)MSG_ARR_PTR + idx * 8);

    p = func_02054d88() + 0x4000;
    li = 0;
    MultiStore_Int(li, p, 0x2000);

    s = func_02054fb0();
    ls = 0x2ff;
    MultiStore16(ls, s, 0x800);

    if (t != 0x1b3) {
        func_0201b6f8(0);
        MSG_LINE_HEIGHT = (u8)(((int)data_0209d6b0 + 7) / 8);
        data_0209d65c = 0;
        div = 0x107;
    } else {
        CURR_MSG_TEXT_CHAR += 3;
        func_0201b6f8(0);
        data_0209d65c = (u8)((0xb0 - (int)data_0209d6b0) >> 1);
        MSG_LINE_HEIGHT = 0x16;
        data_0209d678 = data_0209d65c & 7;
        div = 0x105;
    }

    q = (u16*)(func_02054fb0() + div * 2);
    cnt = MSG_LINE_HEIGHT;
    for (i = 0; i < cnt; i++) {
        u16* q2 = q + 0x20;
        q[i] = (u16)(i + 0x200);
        q2[i] = (u16)(i + 0x200 + cnt);
    }

    func_0201b7cc();
    func_0201b388(MSG_LINE_HEIGHT);
}
