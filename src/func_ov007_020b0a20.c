#include "types.h"
typedef struct SubC {
    int unk0;
    char pad4[8];
    volatile s16 unkC;
    char padE[2];
    u16 unk10;
    u16 unk12;
    int unk14;
} SubC;

typedef struct Elem {
    char pad0[0x18];
    SubC *unk18;
    char pad1C[0x24];
    int unk40;
    int unk44;
    int unk48;
} Elem;

typedef struct Sub100 {
    char pad0[4];
    Elem *unk4;
    char pad8[0x10];
    void *unk18;
} Sub100;

typedef struct Sub8 {
    char pad0[2];
    u16 unk2;
    char pad4[8];
    int unkC;
} Sub8;

typedef struct Sub54 {
    u16 unk0;
    u16 unk2;
} Sub54;

typedef struct Sub50 {
    char pad0[0xC];
    u16 unkC;
    char padE[6];
    u16 unk14;
} Sub50;

typedef struct SubX {
    char pad0[2];
    u16 unk2;
} SubX;

typedef struct SubW {
    char pad0[4];
    SubX *unk4;
} SubW;

typedef struct SubV {
    SubW *unk0;
} SubV;

typedef struct SubE0 {
    char pad0[4];
    int unk4;
} SubE0;

typedef struct Game {
    char pad0[8];
    Sub8 *unk8;
    char padC[0x44];
    Sub50 *unk50;
    Sub54 *unk54;
    char pad58[0x6C];
    SubV *unkC4;
    char padC8[0x18];
    SubE0 *unkE0;
    char padE4[0x1C];
    Sub100 *unk100;
} Game;

extern Game *data_ov007_0210342c;

extern int func_ov007_020aebac(void);
extern void RequestScreenFade(int, int, int);
extern void func_ov007_020b1fa4(int);
extern void func_ov007_020b0da0(void);
extern void func_ov007_020be980(Sub100 *, int, int);
extern int func_ov007_020be964(Sub100 *);
extern void func_ov007_020c0638(int, int, int, int);
extern void func_ov007_020b797c(void);
extern void func_ov007_020bdeb0(int);
extern int _ZN4cstd3divEii(int, int);
extern void func_ov007_020bde70(int, int);
extern void func_ov007_020b131c(void);

void func_ov007_020b0a20(void)
{
    Elem *e;
    int mode;
    int flag6;
    int flag5;
    int result;
    Game *g;

    flag6 = 0;
    flag5 = 0;
    g = data_ov007_0210342c;
    e = g->unk100->unk4;
    mode = g->unk8->unkC;
    result = func_ov007_020aebac();

    if (mode == 0) {
        RequestScreenFade(4, 6, 0);
        func_ov007_020b1fa4(4);
        data_ov007_0210342c->unk100->unk18 = (void *)func_ov007_020b0da0;
        func_ov007_020be980(data_ov007_0210342c->unk100, 1, 0);
        func_ov007_020be964(data_ov007_0210342c->unk100);
        func_ov007_020c0638(data_ov007_0210342c->unkE0->unk4, 0, 0, 0);
        func_ov007_020b797c();
        e->unk48 = (func_ov007_020aebac() == 0) ? 0xDAC : 0x1770;
        e->unk44 = e->unk48;
        e->unk40 = e->unk44;
    }
    if (mode == 1) {
        if (result != 0) {
            func_ov007_020bdeb0(0x32);
            data_ov007_0210342c->unkC4->unk0->unk4->unk2 = 0;
        } else {
            func_ov007_020bdeb0(0x33);
        }
    }
    if (mode == 0x1E) {
        RequestScreenFade(7, 0x18, 0);
    }
    if (data_ov007_0210342c->unk100->unk18 == 0) {
        if (e->unk18->unk0 != 0) {
            func_ov007_020be980(data_ov007_0210342c->unk100, 1, 0);
        } else if (result != 0) {
            int v = e->unk18->unk10;
            if (v <= 0x10) {
                v = 0;
            } else if (v >= 0x14) {
                v = 0x1000;
            } else {
                v = _ZN4cstd3divEii((v - 0x10) << 12, 4);
            }
            e->unk48 = 0x1770 - (int)(((long long)v * 0x9C4 + 0x800) >> 12);
            e->unk44 = e->unk48;
            e->unk40 = e->unk44;
        }
    }
    if (e->unk18->unkC != -1 && e->unk18->unk10 == e->unk18->unk12 - 0x1F) {
        flag5 = 1;
    } else if (e->unk18->unkC != -1 && e->unk18->unk10 == e->unk18->unk12 - 6) {
        flag6 = 1;
    }
    if (((data_ov007_0210342c->unk54->unk0 & ~data_ov007_0210342c->unk54->unk2) ||
         (data_ov007_0210342c->unk50->unkC != 0 && data_ov007_0210342c->unk50->unk14 == 0)) &&
        mode >= 4 && result == 0 &&
        e->unk18->unkC != -1 && e->unk18->unk10 <= e->unk18->unk12 - 0x1F)
    {
        flag6 = 1;
        flag5 = 1;
        if (result != 0) {
            func_ov007_020bde70(0x32, 0x1E);
        } else {
            func_ov007_020bde70(0x33, 0x1E);
        }
        func_ov007_020be964(data_ov007_0210342c->unk100);
        e->unk18->unk14 = (e->unk18->unk12 - 1) << 12;
    }
    if (flag5 != 0) {
        RequestScreenFade(3, 0xA, 0);
    }
    if (flag6 == 0) {
        return;
    }
    func_ov007_020b131c();
    data_ov007_0210342c->unk8->unk2 = 0xA;
    data_ov007_0210342c->unk100->unk18 = 0;
    e->unk48 = 0x1000;
    e->unk44 = e->unk48;
    e->unk40 = e->unk44;
    data_ov007_0210342c->unkC4->unk0->unk4->unk2 = 1;
}
