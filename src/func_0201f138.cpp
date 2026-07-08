//cpp
typedef unsigned char u8;
typedef unsigned short u16;

struct StarEntry {
    int m0;
    u16 m4;
    u8 m6;
};

extern "C" {
extern short CURR_MSG_ID;
extern u8 data_0209d658;
extern u8 data_0209d64c;
extern u8 PLAYER_TALKING;
extern u8 data_0209d6c4;
extern StarEntry* MSG_ARR_PTR;
extern StarEntry* CURR_MSG_PTR;
extern int* MSG_FILE_PTR;
extern int data_0209d6fc;
extern int CURR_MSG_TEXT_CHAR;
extern u8 data_0209d680;
extern u8 data_0209d65c;
extern u8 MSG_LINE_HEIGHT;
extern u8 data_0209d6bc;
extern u8 TOP_SCREEN_RELATED;

void func_0201eaac(void);
int func_02054d88(void);
void MultiStore_Int(int a, int b, int n);
int func_02054fb0(void);
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

    CURR_MSG_ID = 0;
    func_0201eaac();

    idx = CURR_MSG_ID;
    data_0209d658 = 0x7f;
    data_0209d64c = 0x87;
    PLAYER_TALKING = 0;
    data_0209d6c4 = 1;
    CURR_MSG_PTR = (StarEntry*)((char*)MSG_ARR_PTR + idx * 8);
    CURR_MSG_TEXT_CHAR = data_0209d6fc + 0x28 + MSG_FILE_PTR[1] + *(int*)((char*)MSG_ARR_PTR + idx * 8);

    MSG_LINE_HEIGHT = (u8)((((CURR_MSG_PTR->m4 / 8) * 9) + 7) / 8);
    data_0209d680 = 0;
    data_0209d65c = 0;

    p = func_02054d88() + 0x4000;
    li = 0;
    MultiStore_Int(li, p, 0x2000);

    s = func_02054fb0();
    ls = 0x2ff;
    MultiStore16(ls, s, 0x800);

    q = (u16*)(func_02054fb0() + 0x42);
    div = MSG_LINE_HEIGHT;
    cnt = div * CURR_MSG_PTR->m6 * 2;
    for (i = 0; i < cnt; i++) {
        rem = i % div;
        q[rem] = (u16)(i + 0x200);
        if (div - 1 == rem) {
            q += 0x20;
        }
        cnt = div * CURR_MSG_PTR->m6 * 2;
    }

    bcPtr = &data_0209d6bc;
    while (*bcPtr < 4) {
        _ZN7Message6UpdateEv();
    }
    SetBg3Offset(-0x11, -0x3c);
    TOP_SCREEN_RELATED |= 8;
}
