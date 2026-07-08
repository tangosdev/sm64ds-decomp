//cpp
// NONMATCHING: different op / idiom (div=17). Logic verified correct vs ROM; not
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
extern StarEntry* MSG_ARR_PTR;
extern StarEntry* CURR_MSG_PTR;
extern int* MSG_FILE_PTR;
extern int data_0209d6fc;
extern volatile int CURR_MSG_TEXT_CHAR;
extern u8 data_0209d6c4;
extern u8 data_0209d65c;
extern u8 MSG_LINE_HEIGHT;
extern u8 data_0209d6b0;
extern u8 data_0209d6c0;

void func_0201eaac(void);
void func_0201b6f8(int a);
int func_02054d88(void);
void MultiStore_Int(int a, int b, int n);
int func_02054fb0(void);
void MultiStore16(int a, int b, int n);
void SetSubBg1Offset(int a, int b);
void func_0201b7cc(void);
void func_0201b388(int a);
}

extern "C" void _ZN7Message30DisplayCourseNameForStarSelectEj(unsigned int courseID)
{
    volatile unsigned short ls;
    volatile int li1;
    volatile int li2;
    int idx;
    int sum;
    u16* q;
    int div;
    int div2;

    CURR_MSG_ID = (short)(courseID + 0x196);
    PLAYER_TALKING = 0;
    func_0201eaac();

    idx = CURR_MSG_ID;
    CURR_MSG_PTR = (StarEntry*)((char*)MSG_ARR_PTR + idx * 8);
    sum = data_0209d6fc + 0x28 + MSG_FILE_PTR[1] + *(int*)((char*)MSG_ARR_PTR + idx * 8);
    CURR_MSG_TEXT_CHAR = sum;
    CURR_MSG_TEXT_CHAR = sum + 3;
    func_0201b6f8(0);

    MSG_LINE_HEIGHT = (u8)((0x100 - data_0209d6b0) / 2);
    data_0209d65c = 0x20;

    {
        int p1 = func_02054d88() + 0x4000;
        li1 = 0;
        MultiStore_Int(li1, p1, 0x2000);
    }

    {
        int s1 = func_02054fb0();
        ls = 0x2ff;
        MultiStore16(ls, s1, 0x800);
    }

    if (CURR_MSG_ID >= 0x1a5) {
        data_0209d6c4 = 0;
        q = (u16*)(func_02054fb0() + 0x400);
    } else {
        data_0209d6c4 = 1;
        q = (u16*)(func_02054fb0() + 0x4c0);
    }
    div = 0;
    do {
        q[div] = (u16)(div + 0x200);
        div++;
    } while (div < 0x40);

    SetSubBg1Offset(0, 0);
    func_0201b7cc();
    func_0201b388(MSG_LINE_HEIGHT);

    if (CURR_MSG_ID >= 0x1a5) {
        return;
    }

    idx = 654;
    data_0209d6c0 = 1;
    CURR_MSG_PTR = (StarEntry*)((char*)MSG_ARR_PTR + idx * 8);
    CURR_MSG_TEXT_CHAR = data_0209d6fc + 0x28 + MSG_FILE_PTR[1] + *(int*)((char*)MSG_ARR_PTR + idx * 8);
    func_0201b6f8(0);

    MSG_LINE_HEIGHT = (u8)((0xd2 - data_0209d6b0) / 2);
    data_0209d65c = 0x20;

    {
        int p2 = func_02054d88() + 0x4800;
        li2 = 0;
        MultiStore_Int(li2, p2, 0x800);
    }

    q = (u16*)(func_02054fb0() + 0x240);
    div2 = 0;
    do {
        q[div2] = (u16)(div2 + 0x240);
        div2++;
    } while (div2 < 0x40);

    func_0201b7cc();
    func_0201b388(MSG_LINE_HEIGHT);
}
