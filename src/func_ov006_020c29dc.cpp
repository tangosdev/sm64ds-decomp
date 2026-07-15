//cpp
extern "C" {
extern void func_02016a04(void *p, int a);
extern void func_0203cd80(int *m, short angle);
extern void func_0203ccd4(int *m, short angle);
extern void _ZN15TextureSequence6UpdateER15ModelComponents(void *ts, void *mc);
extern void _ZN18TextureTransformer6UpdateER15ModelComponents(void *tt, void *mc);
extern char data_ov006_0212b8fc;
}
struct Obj {
    void* vt;
    virtual void v0(); virtual void v1(); virtual void v2();
    virtual void v3(); virtual void v4();
    virtual void m(void* arg);
};
extern "C" void func_ov006_020c29dc(char *c);
void func_ov006_020c29dc(char *c)
{
    int v0[3];
    int v1[3];
    int *p;
    char *mc;

    v0[0] = 0; v0[1] = 0; v0[2] = 0xfffff008;
    v1[0] = 0; v1[1] = 0; v1[2] = 0xfffff008;
    func_02016a04(c + 8, 0x3d6d);
    func_0203cd80(v0, -0x2000);
    func_0203ccd4(v0, 0x2000);
    func_0203cd80(v1, 0x4200);
    func_0203ccd4(v1, 0x7000);

    *(volatile int *)0x40004cc = 0x7fff;
    *(volatile int *)0x40004c8 =
        (((short)v0[0] >> 3) & 0x3ff) |
        ((((short)v0[1] >> 3) & 0x3ff) << 10) |
        ((((short)v0[2] >> 3) & 0x3ff) << 20);
    mc = c + 0x10;
    *(volatile int *)0x40004cc = 0x4000294b;
    *(volatile int *)0x40004c8 =
        (((short)v1[0] >> 3) & 0x3ff) |
        ((((short)v1[1] >> 3) & 0x3ff) << 10) |
        ((((short)v1[2] >> 3) & 0x3ff) << 20) | 0x40000000;

    p = *(int **)(mc + 4);
    _ZN15TextureSequence6UpdateER15ModelComponents(c + 0xc8, mc);
    _ZN18TextureTransformer6UpdateER15ModelComponents(c + 0xdc, mc);
    *(int *)(((int)p + 0x104) & 0xFFFFFFFFFFFFFFFFLL) -= *(int *)(c + 0x194);
    *(int *)(((int)p + 0x108) & 0xFFFFFFFFFFFFFFFFLL) += *(int *)(c + 0x198);
    *(int *)(((int)p + 0x134) & 0xFFFFFFFFFFFFFFFFLL) += *(int *)(c + 0x18c);
    *(int *)(((int)p + 0x138) & 0xFFFFFFFFFFFFFFFFLL) += *(int *)(c + 0x190);
    ((Obj *)(c + 8))->m(0);
    ((Obj *)(c + 0x78))->m(&data_ov006_0212b8fc);
}
