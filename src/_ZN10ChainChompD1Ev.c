extern int func_0207328c(void *arr, int n, int sz, void *dtor);
extern void func_020072c0(void);
extern void _ZN11ShadowModelD1Ev(void *p);
extern void _ZN5ModelD1Ev(void *p);
extern void _ZN9ModelAnimD1Ev(void *p);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *p);
extern int func_ov002_020aed18(int *x);
extern void *data_ov034_021147ec[];

void *_ZN10ChainChompD1Ev(char *c) {
    *(void**)c = data_ov034_021147ec;
    func_0207328c(c + 0x578, 7, 0xc, (void*)func_020072c0);
    func_0207328c(c + 0x524, 7, 0xc, (void*)func_020072c0);
    func_0207328c(c + 0x40c, 7, 0x28, (void*)_ZN11ShadowModelD1Ev);
    func_0207328c(c + 0x1dc, 7, 0x50, (void*)_ZN5ModelD1Ev);
    _ZN11ShadowModelD1Ev(c + 0x1b4);
    _ZN9ModelAnimD1Ev(c + 0x150);
    _ZN25MovingCylinderClsnWithPosD1Ev(c + 0x110);
    func_ov002_020aed18((int*)c);
    return c;
}
