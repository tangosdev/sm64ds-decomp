// NONMATCHING: different op / idiom (div=7). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

void func_02012718(void *a, int b);

typedef struct { u8 f0, f1, f2, f3; } Tab;
extern u8 data_020a0e40;
extern Tab TOUCH_INPUT_ARR[];

void func_ov006_020d6d7c(char *this, int idx) {
    u8 m = data_020a0e40;
    int hit = 0;
    char *b;
    int dx, dy;
    if (TOUCH_INPUT_ARR[m].f0 != 0) {
        if (TOUCH_INPUT_ARR[m].f1 != 0) hit = 1;
    }
    if (hit == 0) return;
    if (*(u8*)(this + 0x62f6) != 0xff) return;
    b = this + idx * 0x40;
    dx = TOUCH_INPUT_ARR[m].f2 - (*(int*)(b + 0x4660) >> 0xc);
    dy = TOUCH_INPUT_ARR[m].f3 - (*(int*)(b + 0x4664) >> 0xc);
    if (dx > 0xc) return;
    if (dx < -0xc) return;
    if (dy > 0xf) return;
    if (dy < -0xf) return;
    *(u8*)(this + 0x62f6) = *(u8*)(b + 0x4696);
    *(u8*)(b + 0x4694) = 1;
    *(u8*)(b + 0x4697) = 2;
    *(int*)(b + 0x4668) = dx << 0xc;
    *(int*)(b + 0x466c) = dy << 0xc;
    *(u8*)(b + 0x469e) = 0;
    func_02012718((void*)0x1d2, *(int*)(b + 0x4660));
}
