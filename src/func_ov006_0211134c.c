#pragma opt_propagation off
typedef unsigned char u8;

typedef struct Obj {
    char _0[4];
    char *p;
    int f8;
    int fc;
    int f10;
    int f14;
    char _18[8];
    int f20;
    int f24;
    char _28[9];
    u8  f31;
    char _32[2];
    int f34;
    int f38;
    int f3c;
    int f40;
    int f44;
} Obj;

extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];

extern int func_020124c4(int, int, int, int);

void func_ov006_0211134c(Obj *c)
{
    u8 lvl;
    int de8v;
    int flag;
    int d;

    c->f10 = c->f8;
    c->f14 = c->fc;
    if (*(u8 *)(c->p + 0x595d) != 0)
        return;

    de8v = data_020a0de8[data_020a0e40 * 4];
    lvl = data_020a0e40;
    flag = 0;
    if (de8v != 0)
        flag = data_020a0de9[lvl * 4] != 0;

    if (flag != 0) {
        c->f31 = 1;
        c->f20 = 0;
        c->f24 = 0;
        c->f3c = data_020a0dea[data_020a0e40 * 4] << 12;
        c->f34 = c->f3c;
        c->f40 = data_020a0deb[data_020a0e40 * 4] << 12;
        c->f38 = c->f40;
        if (c->fc <= 0xa0000)
            return;
        {
            int *pf38 = (int *)(((int)c + 0x38) & 0xFFFFFFFFFFFFFFFFLL);
            *pf38 = *pf38 - (c->fc - 0xa0000);
        }
        return;
    }

    if (c->f31 == 1 && de8v != 0) {
        c->f40 = data_020a0deb[lvl * 4] << 12;
        d = c->f40 - c->f38;
        if (d >= 0x38000)
            c->f40 = c->f38 + 0x38000;
        else if (c->f40 < c->f38)
            c->f40 = c->f38;
        c->fc = c->f40 - c->f38 + 0xa0000;
        if (c->f14 == c->fc)
            return;
        c->f44 = func_020124c4(c->f44, 2, 0x16c, 0);
        return;
    }

    {
        int m;
        if (de8v == 0 && data_020a0de9[lvl * 4] != 0)
            m = 1;
        else
            m = 0;
        if (m != 0) {
            c->f31 = 0;
            c->f20 = 0;
            c->f24 = 0;
            return;
        }
    }

    {
        int *pf24 = (int *)(((int)c + 0x24) & 0xFFFFFFFFFFFFFFFFLL);
        *pf24 = *pf24 - (c->fc - 0xa0000) / 16;
    }
    {
        int *pfc = (int *)(((int)c + 0xc) & 0xFFFFFFFFFFFFFFFFLL);
        *pfc = *pfc + c->f24;
    }
    if (c->fc < 0xa0000) {
        c->fc = 0xa0000;
        c->f24 = 0;
    }
}
