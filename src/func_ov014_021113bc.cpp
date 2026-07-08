//cpp
extern "C" {
extern int func_0207328c(void *p, int a, int b, void *fn);
extern void _ZN11ShadowModelD1Ev(void *p);
extern void _ZN9ModelAnimD1Ev(void *p);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *p);
extern int func_ov002_020aed18(int *x);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *p, void *heap);
extern void _ZN5ModelD1Ev(void *p);
extern int data_ov034_021147ec[];
extern int func_020072c0(void);
extern int _ZN6Memory11gameHeapPtrE;

void *func_ov014_021113bc(char *self) {
    *(int**)self = data_ov034_021147ec;
    func_0207328c(self + 0x578, 7, 0xc, (void*)func_020072c0);
    func_0207328c(self + 0x524, 7, 0xc, (void*)func_020072c0);
    func_0207328c(self + 0x40c, 7, 0x28, (void*)_ZN11ShadowModelD1Ev);
    func_0207328c(self + 0x1dc, 7, 0x50, (void*)_ZN5ModelD1Ev);
    _ZN11ShadowModelD1Ev(self + 0x1b4);
    _ZN9ModelAnimD1Ev(self + 0x150);
    _ZN25MovingCylinderClsnWithPosD1Ev(self + 0x110);
    func_ov002_020aed18((int*)self);
    _ZN6Memory10DeallocateEPvP4Heap(self, *(void**)&_ZN6Memory11gameHeapPtrE);
    return self;
}
}
