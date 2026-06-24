//cpp
extern "C" void func_0207328c(void* p, int n, int sz, void* dtor);
extern "C" void _ZN11ShadowModelD1Ev(void*);
extern "C" void _ZN9ModelAnimD1Ev(void*);
extern "C" void _ZN12WithMeshClsnD1Ev(void*);
extern "C" void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern "C" int func_ov002_020aed18(int* x);
extern "C" void _ZN5ModelD1Ev(void*);

extern int data_ov100_02148054;
extern "C" void func_02011508(void);
extern "C" void func_020072c0(void);

extern "C" void* func_ov100_021431c4(char* thiz)
{
    char* c = thiz;
    *(void**)c = &data_ov100_02148054;
    func_0207328c(c + 0x768, 6, 6, (void*)&func_02011508);
    func_0207328c(c + 0x720, 6, 0xc, (void*)&func_020072c0);
    func_0207328c(c + 0x6d8, 6, 0xc, (void*)&func_020072c0);
    _ZN11ShadowModelD1Ev(c + 0x640);
    func_0207328c(c + 0x550, 6, 0x28, (void*)&_ZN11ShadowModelD1Ev);
    func_0207328c(c + 0x370, 6, 0x50, (void*)&_ZN5ModelD1Ev);
    _ZN9ModelAnimD1Ev(c + 0x30c);
    _ZN12WithMeshClsnD1Ev(c + 0x150);
    _ZN25MovingCylinderClsnWithPosD1Ev(c + 0x110);
    func_ov002_020aed18((int*)c);
    return c;
}
