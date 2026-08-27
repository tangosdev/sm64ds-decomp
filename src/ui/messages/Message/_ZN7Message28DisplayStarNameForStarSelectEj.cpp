//cpp
#include "types.h"
// @symbol _ZN7Message28DisplayStarNameForStarSelectEj
/* recovered: named members + shared header */
#include "Message.h"
struct StarEntry {
    int m0;
    int m4;
};

extern "C" {
extern signed char data_02092110;
extern short data_0209d6d4;
extern u8 data_0209d660;
extern u8 data_0209d6c0;
extern u8 data_0209d6c4;
extern StarEntry* data_0209d708;
extern StarEntry* data_0209d6f0;
extern int* data_0209d70c;
extern int data_0209d6fc;
extern int data_0209d6f4;
extern u8 data_0209d6b0;
extern u8 data_0209d65c;
extern u8 data_0209d6a8;

int SublevelToLevel(int i);
void func_0201eaac(void);
void func_0201b6f8(int a);
int func_02054d88(void);
void MultiStore_Int(int a, int b, int n);
int _ZN2G212GetBG3ScrPtrEv(void);
void func_0201b7cc(void);
void func_0201b388(int a);
}

void Message::DisplayStarNameForStarSelect(unsigned int msg)
{
    volatile int li;
    int p;
    int idx;
    u16* q;
    int i;

    data_0209d6d4 = (short)(msg + (SublevelToLevel(data_02092110) * 7 + 0x1b4));
    data_0209d660 = 0;
    func_0201eaac();

    idx = data_0209d6d4;
    data_0209d6c0 = 2;
    data_0209d6c4 = 1;
    data_0209d6f0 = (StarEntry*)((char*)data_0209d708 + idx * 8);
    data_0209d6f4 = data_0209d6fc + 0x28 + data_0209d70c[1] + *(int*)((char*)data_0209d708 + idx * 8);
    func_0201b6f8(0);

    data_0209d65c = (0x100 - (int)data_0209d6b0) / 2;
    data_0209d6a8 = 0x20;

    p = func_02054d88() + 0x5000;
    li = 0;
    MultiStore_Int(li, p, 0x800);

    q = (u16*)(_ZN2G212GetBG3ScrPtrEv() + 0x180);
    for (i = 0; i < 0x40; i++) {
        q[i] = (u16)(i + 0x280);
    }

    func_0201b7cc();
    func_0201b388(data_0209d6a8);
}
