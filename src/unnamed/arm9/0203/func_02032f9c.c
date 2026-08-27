#include "types.h"
typedef struct {
    int m0;
    short m4;
    u8 m6;
    u8 m7;
} StarEntry;

extern short data_0209fce8;
extern u8 data_0209fc9c;
extern StarEntry *data_0209fd08;
extern int data_0209fd14;
extern int *data_0209fcf8;
extern u8 data_0209fc94;
extern u8 data_0209fc78;
extern StarEntry *data_0209fd00;
extern int data_0209fd0c;
extern u8 data_0209fc84;
extern u8 data_0209fc88;
extern u8 data_0209fc7c;
extern u8 data_0209fc90;
extern u8 data_0209fcd8;
extern u8 data_0209fccc;
extern u8 data_0209fcd4;
extern u8 data_0209fcdc;

void func_02034504(void);
int func_02054d88(void);
void MultiStore_Int(int a, int b, int n);
int _ZN2G212GetBG3ScrPtrEv(void);
void MultiStore16(int a, int b, int n);
void func_02031cd4(void *arg);
void func_02031e00(void);
void func_020319fc(u8 a);

void func_02032f9c(int a)
{
    volatile int li;
    volatile unsigned short ls;
    int idx;
    int p, s;
    int v;
    int i;
    u16 *q;
    int v2;
    int w, n;
    u8 c;
    u8 old94;

    data_0209fce8 = 0x20;
    data_0209fc9c = 0;
    func_02034504();

    idx = data_0209fce8;
    v = data_0209fd14;
    data_0209fc94 = 1;
    data_0209fc78 = 0;
    data_0209fd00 = (StarEntry *)((char *)data_0209fd08 + idx * 8);
    data_0209fd0c = v + 0x28 + data_0209fcf8[1] + *(int *)((char *)data_0209fd08 + idx * 8);

    p = func_02054d88() + 0x4000;
    li = 0;
    MultiStore_Int(li, p, 0x2000);

    s = _ZN2G212GetBG3ScrPtrEv();
    ls = 0x2ff;
    MultiStore16(ls, s, 0x800);

    if (a < 0) {
        return;
    }

    data_0209fc84 = (u8)a;
    func_02031cd4(0);

    w = 0x100 - data_0209fc88;
    data_0209fc7c = (u8)((w >> 1) & 7);
    q = (u16 *)(_ZN2G212GetBG3ScrPtrEv() + 0x40) + (u16)(w / 8 >> 1);
    c = data_0209fc7c;
    n = (data_0209fc88 + c + 7) >> 3;
    i = 0;
    if (n > 0) {
        v2 = n + 0x200;
        do {
            q[i] = (u16)(i + 0x200);
            (q + i)[0x20] = (u16)v2;
            v2++;
            i++;
        } while (i < n);
    }

    data_0209fc90 = 1;
    data_0209fcd4 = 4;
    data_0209fccc = c;
    data_0209fcd8 = 0;
    data_0209fc88 = 0;
    old94 = data_0209fc94;
    data_0209fc94 = 0;
    func_02031e00();

    data_0209fc88 = data_0209fc88 + data_0209fc7c;
    func_020319fc((u8)((data_0209fc88 + 7) >> 3));

    data_0209fc94 = old94;
    data_0209fcd4 = 0;
    data_0209fcdc = (u8)(((data_0209fc88 + 7) >> 3) << 1);
}
