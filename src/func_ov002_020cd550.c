typedef unsigned char u8;
typedef unsigned short u16;

struct S18 { short f0; char pad[0x16]; };

extern u8 data_020a0e40;
extern struct S18 data_0209f4a4[];
extern short data_02082214[];

extern void func_ov002_020cd448(char* self);

/* the += / -= on 0x92 go through a materialized base (add r1,c,#0x92;
   ldrsh/strh [r1]); the (long long)-mask launder stops mwcc folding it. */
#define LS16(p) (*(short*)(int)(((long long)(int)(p))))

void func_ov002_020cd550(char* c)
{
    short lim, v, w, a;
    int step, idx;

    lim = (short)(int)((((long long)data_0209f4a4[data_020a0e40].f0 * -0x3f00LL) + 0x800) >> 12);
    lim &= 0xfff0;

    step = *(short*)(c + 0x92) > 0 ? 0x100 : 0x200;
    if (*(short*)(c + 0x92) < lim) {
        LS16(c + 0x92) += step;
        if (*(short*)(c + 0x92) > lim) *(short*)(c + 0x92) = lim;
    } else if (*(short*)(c + 0x92) > lim) {
        LS16(c + 0x92) -= step;
        if (*(short*)(c + 0x92) < lim) *(short*)(c + 0x92) = lim;
    }

    func_ov002_020cd448(c);

    v = *(short*)(c + 0x92);
    if (v < 0) *(short*)(c + 0x8c) = v * 6 / 10;
    else *(short*)(c + 0x8c) = v * 10 / 8;

    *(int*)(c + 0x690) = 0;
    w = *(short*)(c + 0x8c);
    if (w >= 0)
        *(int*)(c + 0x690) = (int)((((long long)w * 0xc000LL) + 0x800) >> 12);

    a = (short)(*(short*)(c + 0x8c) - 0x2000);
    if (a < -0x2000) a = -0x2000;
    if (a > 0) a = 0;
    idx = (u16)(short)(a * 2) >> 4;
    *(int*)(c + 0x694) = data_02082214[idx * 2 + 1] * -0x30;
}
