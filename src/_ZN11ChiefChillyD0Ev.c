extern int func_0207328c(void* c, int a, int b, void* d);
extern void* _ZTV11ChiefChilly[];
extern void func_020072c0(void);
extern int _ZN11ShadowModelD1Ev(void* c);
extern int _ZN14BlendModelAnimD1Ev(void* c);
extern int _ZN12WithMeshClsnD1Ev(void* c);
extern int _ZN25MovingCylinderClsnWithPosD1Ev(void* c);
extern int func_ov002_020aed18(int* x);
extern int data_020a0eac;
extern int _ZN6Memory10DeallocateEPvP4Heap(void* p, void* h);
int _ZN11ChiefChillyD0Ev(void* c)
{
    *(void**)c = _ZTV11ChiefChilly;
    func_0207328c((char*)c+0x4d4, 2, 0xc, (void*)func_020072c0);
    func_0207328c((char*)c+0x448, 8, 0xc, (void*)func_020072c0);
    func_0207328c((char*)c+0x3e8, 8, 0xc, (void*)func_020072c0);
    _ZN11ShadowModelD1Ev((char*)c+0x380);
    _ZN14BlendModelAnimD1Ev((char*)c+0x30c);
    _ZN12WithMeshClsnD1Ev((char*)c+0x150);
    _ZN25MovingCylinderClsnWithPosD1Ev((char*)c+0x110);
    func_ov002_020aed18((int*)c);
    _ZN6Memory10DeallocateEPvP4Heap(c, (void*)data_020a0eac);
    return (int)c;
}
