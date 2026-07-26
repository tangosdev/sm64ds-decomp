typedef signed char s8;
typedef short s16;
typedef unsigned short u16;

typedef struct Sb {
    int p0;
    int f4;
} Sb;

typedef struct R8 {
    int p0;
    int f4;
    char pad8[0xc - 8];
    u16 fc;
    char padE[0x70 - 0xe];
    s16 f70;
} R8;

typedef struct Self {
    int p0;
    void *f4;
} Self;

typedef struct G2 {
    char pad0[0x34];
    int f34;
    char pad38[0x174 - 0x38];
    Sb *f174;
    void *f178;
    R8 *f17c;
} G2;

#define AT(p,off) ((void*)(int)(((long long)(int)((char*)(p)+(off)))&0xFFFFFFFFFFFFFFFFLL))
#define MASK(p) ((void*)(int)(((long long)(int)(p))&0xFFFFFFFFFFFFFFFFLL))

extern G2 *data_ov007_0210342c;
extern unsigned char data_ov007_020d7610[];
extern void func_ov007_020c42f8(void *self);
extern int func_ov007_020c3598(int a, int b, s8 *buf, int d);
extern void func_ov007_020c4128(void *self, void *dest, int arg2);

#pragma opt_loop_invariants off

void func_ov007_020b413c(Self *self, int arg1) {
    Sb *sb;
    R8 *r8;
    s16 orig70;
    int acc;
    int i;
    int saved;
    s8 buf[4];

    *(volatile int *)0x4000444 = 0;
    func_ov007_020c42f8(self->f4);
    if (arg1 >= 0) {
        G2 **gpp;
        s8 *pp;
        int j;
        int origsb4;
        unsigned char *tab;
        sb = data_ov007_0210342c->f174;
        r8 = data_ov007_0210342c->f17c;
        saved = sb->f4;
        orig70 = r8->f70;
        acc = 0;
        func_ov007_020c3598(arg1, 3, buf, 0xa);
        for (i = 0; i < 3; i++) {
            if (buf[i] >= 0) acc += data_ov007_020d7610[buf[i]];
        }
        tab = (unsigned char *)AT(data_ov007_020d7610, 0);
        saved = saved + ((-acc / 2) << 12);
        r8->f4 = saved + 0xa000;
        pp = &buf[2];
        gpp = (G2 **)AT(&data_ov007_0210342c, 0);
        for (j = 2; j >= 0; j--) {
            if (*pp >= 0) {
                *(int *)AT(r8, 4) += ((unsigned)tab[*pp] >> 1) << 12;
                r8->f70 = (s16)(*pp * r8->fc + orig70);
                func_ov007_020c4128(self->f4, r8, (*gpp)->f34);
                *(int *)AT(MASK(r8), 4) += (tab[*pp] - ((unsigned)tab[*pp] >> 1)) << 12;
            }
            pp--;
        }
        r8->f70 = orig70;
        origsb4 = sb->f4;
        sb->f4 = saved;
        func_ov007_020c4128(self->f4, sb, data_ov007_0210342c->f34);
        sb->f4 = origsb4;
    } else {
        func_ov007_020c4128(self->f4, data_ov007_0210342c->f178, data_ov007_0210342c->f34);
    }
    *(volatile int *)0x4000448 = 1;
}
