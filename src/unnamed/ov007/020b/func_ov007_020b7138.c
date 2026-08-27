extern void func_ov007_020b6c54(void);
extern void func_ov007_020c3e3c(int v);
extern void func_ov007_020c3e2c(int v);
extern void func_ov007_020c3e1c(int v);
extern void func_ov007_020c3bc8(void);
extern void func_ov007_020b4100(int a, int b);
extern void func_ov007_020bd960(void);
extern void func_ov007_020bd5a8(void);
extern void func_ov007_020bd500(void);
extern void func_ov007_020bd648(int mode);
extern void func_02045d9c(void);
extern void _ZN3G2x18SetBlendBrightnessEPVtts(void *p, unsigned short a, short b);
extern void func_ov007_020b68e8(void);
extern void func_ov007_020b30b0(void);
extern void func_ov007_020be00c(void);
extern void func_ov007_020b8714(int a0, int a1, int a2);
extern void func_ov007_020b782c(void);
extern void func_ov007_020bfcec(void);
extern void func_ov007_020be50c(void);
extern void func_ov007_020b6544(void);

struct Arg {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
};

struct Sub {
    char _0[0x3a];
    unsigned char f3a;
};

struct S {
    char _0[0x28];
    struct Sub *f28;
    int f2c;
    char _30[4];
    int f34;
    char _38[0x68];
    int fa0;
    char _a4[0x4c];
    int ff0;
    char _f4[8];
    unsigned char ffc;
    unsigned char ffd;
};

extern struct S *data_ov007_0210342c;
extern int data_ov007_02103430;

int func_ov007_020b7138(struct Arg *a, int *b)
{
    struct S *p;
    int flags;

    data_ov007_02103430 = 0;
    func_ov007_020b6c54();
    func_ov007_020c3e3c(a->f0);
    func_ov007_020c3e2c(a->f4);
    func_ov007_020c3e1c(a->f8);
    func_ov007_020c3bc8();
    func_ov007_020b4100(a->fc, a->f10);
    func_ov007_020bd960();
    func_ov007_020bd5a8();
    func_ov007_020bd500();
    func_ov007_020bd648(1);
    func_02045d9c();
    _ZN3G2x18SetBlendBrightnessEPVtts((void *)0x4000050, 0x3f, -0x10);
    _ZN3G2x18SetBlendBrightnessEPVtts((void *)0x4001050, 0x3f, -0x10);
    *(unsigned int *)0x4000000 &= ~0x1f00;
    *(unsigned int *)0x4001000 &= ~0x1f00;
    *(unsigned short *)0x4000304 &= ~0x8000;
    func_ov007_020b68e8();
    *(int *)data_ov007_0210342c->f28 = (int)b;
    data_ov007_0210342c->f2c = a->f14;
    func_ov007_020b30b0();
    func_ov007_020be00c();
    p = data_ov007_0210342c;
    func_ov007_020b8714(p->f34, p->fa0, p->f28->f3a);
    func_ov007_020b782c();
    func_ov007_020bfcec();
    func_ov007_020be50c();
    func_ov007_020b6544();
    flags = *b;
    data_ov007_0210342c->ff0 = flags & 1;
    data_ov007_0210342c->ffc = (flags & 6) >> 1;
    data_ov007_0210342c->ffd = (flags & 0x18) >> 3;
    data_ov007_02103430 = 1;
    return 1;
}
