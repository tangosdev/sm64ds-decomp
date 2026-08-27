typedef long long s64;

extern int func_020531a4(int a);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void func_ov007_020bde2c(int idx);
extern void func_0204f924(int *p, int v);
extern void func_0204f7fc(int *p, int a, int b);

extern int data_ov007_02104bb4;
extern int data_ov007_02104bb0;
extern int *data_ov007_02104bbc;
extern int data_ov007_02104ba8;
extern int data_ov007_02104ba4;
extern int data_ov007_02104bac;

void func_ov007_020bdbcc(int a0, int a1)
{
    int stateA = data_ov007_02104bb4;
    int stateB = data_ov007_02104bb0;
    int newA = (int)((((s64)stateA * 0xc18) + ((s64)(a0 << 12) * 0x3e8)) >> 12);
    int newB = (int)((((s64)stateB * 0xc18) + ((s64)(a1 << 12) * 0x3e8)) >> 12);
    int magSq;
    int spd;
    int percent;
    int active;
    int *pp;
    int r5v;
    int t;

    data_ov007_02104bb0 = newB;
    magSq = (int)(((s64)newA * newA + 0x800) >> 12) + (int)(((s64)newB * newB + 0x800) >> 12);
    data_ov007_02104bb4 = newA;

    spd = func_020531a4(magSq);

    if (spd <= 0x1388) {
        percent = 0;
    } else if (spd >= 0x1b58) {
        percent = 0x1000;
    } else {
        percent = _ZN4cstd4fdivEii(spd - 0x1388, 0x7d0);
    }

    if (data_ov007_02104bbc[0x35] == 0) {
        data_ov007_02104ba8 = 0;
        data_ov007_02104ba4 = 0;
    }

    active = (percent >= 0x800) ? 0x1000 : 0;

    if (active > 0) {
        data_ov007_02104ba4 = data_ov007_02104ba4 + 1;
    } else {
        data_ov007_02104ba8 = data_ov007_02104ba8 + 1;
    }

    if (data_ov007_02104ba4 > 3) {
        data_ov007_02104ba4 = 3;
        data_ov007_02104ba8 = 0;
        active = 0x1000;
    } else if (data_ov007_02104ba8 > 3) {
        active = 0;
        data_ov007_02104ba8 = 3;
        data_ov007_02104ba4 = 0;
    }

    r5v = (active * 0x7f) >> 12;

    if (spd <= 0x1388) {
        t = 0;
    } else if (spd >= 0x5000) {
        t = 0x1000;
    } else {
        t = _ZN4cstd4fdivEii(spd - 0x1388, 0x3c78);
    }

    pp = &data_ov007_02104bbc[0x35];
    func_ov007_020bde2c(0x35);
    func_0204f924(pp, r5v);
    func_0204f7fc(pp, 0xf, -(((0x1000 - t) * 0x300) >> 12));

    data_ov007_02104bac = data_ov007_02104bac + 1;
}
