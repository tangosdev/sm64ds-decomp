//cpp
#include "decl_common.h"

struct C; typedef void (C::*PMF)();
extern s16 data_02082214[];
extern signed char data_0209f2f8;
extern PMF data_ov102_0214e870[][4];
extern PMF data_ov102_0214e8c0[][4];
extern "C" {
void func_ov102_02149684(int *dst, void *src);
int DecIfAbove0_Short(s16 *p);
void _ZN5Sound9PlayBank3EjRK7Vector3(int bank, void *pos);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned id, int x, int y, int z);
int _ZN8SaveData16HasPlayerLostCapEv(void);
void *func_ov102_02149220(void *c);
void func_ov102_0214953c(void *c, int p1, int p2);
}
extern "C" void func_ov102_021498e0(C *self)
{
    char *c = (char *)self;
    Vector3 pos;
    int ch;
    u16 typ;
    void *held;
    func_ov102_02149684((int *)&pos, c);
    if (DecIfAbove0_Short((s16 *)(c + 0x3ee)) != 0) {

        s16 *pang = (s16 *)((int)(c + 0x3ec));
        u16 ang = *(u16 *)(c + 0x3ec);
        s16 s = data_02082214[(ang >> 4) * 2];
        int conf = 0x999;
        int t = (int)s + 0x1000;
        int y = (int)(((s64)t * conf + 0x800) >> 12);
        *(int *)(c + 0x84) = y + 0x666;
        ang = *(u16 *)(c + 0x3ec);
        s = data_02082214[(ang >> 4) * 2];
        int u = 0x1000 - (int)s;
        *(int *)(c + 0x80) = (int)(((s64)u * 0x1000 + 0x800) >> 12) + 0x1000;
        *(int *)(c + 0x88) = *(int *)(c + 0x80);
        ang = *(u16 *)(c + 0x3ec);
        s = data_02082214[(ang >> 4) * 2];
        *(int *)(c + 0x3dc) = (0x1000 - (int)s) * 0xd;
        *pang += 0x1000;

        return;
    }
    held = *(void **)(c + 0x3f4);
    if (held != 0) {
        if (*(u16 *)((char *)held + 0xc) == 0x149)
            func_ov002_020f0438(held);
        *(void **)(c + 0x3f4) = 0;
    }
    _ZN5Sound9PlayBank3EjRK7Vector3(0, c + 0x74);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb, pos.x, pos.y, pos.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd, pos.x, pos.y, pos.z);
    typ = *(u16 *)(c + 0xc);
    switch (typ - 0x14) {
    case 1: case 2:
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x10, pos.x, pos.y, pos.z); break;
    case 0:
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xc, pos.x, pos.y, pos.z); break;
    case 3: case 4: case 5:
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(9, pos.x, pos.y, pos.z); break;
    }
    ch = (int)*(u8 *)(c + 0x3f2);
    if (ch < 0) ch = 0; else if (ch > 3) ch = 3;
    typ = *(u16 *)(c + 0xc);
    switch (typ - 0x14) {
    case 1: {
        PMF (*tbl)[4] = data_ov102_0214e8c0;
        u8 content = *(u8 *)(c + 0x3f3);
        (self->*tbl[content][0])();
        break;
    }
    case 2: {
        PMF (*tbl)[4] = data_ov102_0214e870;
        u8 content = *(u8 *)(c + 0x3f3);
        if (ch >= 4) ch = 0;
        (self->*tbl[content][ch])();
        break;
    }
    case 0:
        if (_ZN8SaveData16HasPlayerLostCapEv() == 0 || data_0209f2f8 == 0x1f) {
            PMF (*tbl)[4] = data_ov102_0214e8c0;
            u8 content = *(u8 *)(c + 0x3f3);
            if (ch >= 4) ch = 0;
            (self->*tbl[content][ch])();
        } else {
            func_ov102_02149220(c);
        }
        break;
    case 3:
        if (_ZN8SaveData16HasPlayerLostCapEv() == 0) func_ov102_0214953c(c, 0, 0x12);
        else func_ov102_02149220(c);
        break;
    case 5:
        if (_ZN8SaveData16HasPlayerLostCapEv() == 0) func_ov102_0214953c(c, 1, 0x12);
        else func_ov102_02149220(c);
        break;
    case 4:
        if (_ZN8SaveData16HasPlayerLostCapEv() == 0) func_ov102_0214953c(c, 2, 0x12);
        else func_ov102_02149220(c);
        break;
    }
    func_ov102_02149da8(c, 2);
}
