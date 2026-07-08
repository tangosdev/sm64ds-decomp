//cpp
typedef unsigned char u8;
typedef unsigned short u16;

struct StarEntry {
    int m0;
    int m4;
};

extern "C" {
extern signed char NEXT_LEVEL_ID;
extern short CURR_MSG_ID;
extern u8 PLAYER_TALKING;
extern u8 data_0209d6c0;
extern u8 data_0209d6c4;
extern StarEntry* MSG_ARR_PTR;
extern StarEntry* CURR_MSG_PTR;
extern int* MSG_FILE_PTR;
extern int data_0209d6fc;
extern int CURR_MSG_TEXT_CHAR;
extern u8 data_0209d6b0;
extern u8 data_0209d65c;
extern u8 MSG_LINE_HEIGHT;

int SublevelToLevel(int i);
void func_0201eaac(void);
void func_0201b6f8(int a);
int func_02054d88(void);
void MultiStore_Int(int a, int b, int n);
int func_02054fb0(void);
void func_0201b7cc(void);
void func_0201b388(int a);
}

extern "C" void _ZN7Message28DisplayStarNameForStarSelectEj(unsigned int msg)
{
    volatile int li;
    int p;
    int idx;
    u16* q;
    int i;

    CURR_MSG_ID = (short)(msg + (SublevelToLevel(NEXT_LEVEL_ID) * 7 + 0x1b4));
    PLAYER_TALKING = 0;
    func_0201eaac();

    idx = CURR_MSG_ID;
    data_0209d6c0 = 2;
    data_0209d6c4 = 1;
    CURR_MSG_PTR = (StarEntry*)((char*)MSG_ARR_PTR + idx * 8);
    CURR_MSG_TEXT_CHAR = data_0209d6fc + 0x28 + MSG_FILE_PTR[1] + *(int*)((char*)MSG_ARR_PTR + idx * 8);
    func_0201b6f8(0);

    data_0209d65c = (0x100 - (int)data_0209d6b0) / 2;
    MSG_LINE_HEIGHT = 0x20;

    p = func_02054d88() + 0x5000;
    li = 0;
    MultiStore_Int(li, p, 0x800);

    q = (u16*)(func_02054fb0() + 0x180);
    for (i = 0; i < 0x40; i++) {
        q[i] = (u16)(i + 0x280);
    }

    func_0201b7cc();
    func_0201b388(MSG_LINE_HEIGHT);
}
