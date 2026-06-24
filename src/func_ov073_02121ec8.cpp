//cpp
extern "C" void* _ZN9ActorBasenwEj(unsigned int sz);
extern "C" void func_020aed98(void);
extern "C" void _ZN25MovingCylinderClsnWithPosC1Ev(void*);
extern "C" void _ZN12WithMeshClsnC1Ev(void*);
extern "C" void _ZN14BlendModelAnimC1Ev(void*);
extern "C" void _ZN11ShadowModelC1Ev(void*);
extern "C" void func_020733a8(void* p, int a, int b, void* f1, void* f2);

extern int data_ov073_02123090;
extern "C" void func_020072c0(void);
extern "C" void func_0203d384(void);

extern "C" void* func_ov073_02121ec8(void)
{
    char* p = (char*)_ZN9ActorBasenwEj(0x504);
    if (p) {
        func_020aed98();
        *(void**)p = &data_ov073_02123090;
        _ZN25MovingCylinderClsnWithPosC1Ev(p + 0x110);
        _ZN12WithMeshClsnC1Ev(p + 0x150);
        _ZN14BlendModelAnimC1Ev(p + 0x30c);
        _ZN11ShadowModelC1Ev(p + 0x380);
        func_020733a8(p + 0x3e8, 8, 0xc, (void*)&func_0203d384, (void*)&func_020072c0);
        func_020733a8(p + 0x448, 8, 0xc, (void*)&func_0203d384, (void*)&func_020072c0);
        func_020733a8(p + 0x4d4, 2, 0xc, (void*)&func_0203d384, (void*)&func_020072c0);
    }
    return p;
}
