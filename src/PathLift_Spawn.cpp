//cpp
extern "C" {
void* _ZN9ActorBasenwEj(unsigned int sz);
void _ZN8PlatformC2Ev(void* p);
void func_020733a8(void* p, int a, int b, void* ctor, void* dtor);
void _ZN7PathPtrC1Ev(void* p);
void _ZN11ShadowModelC1Ev(void* p);
void _ZN5ModelD1Ev(void* p);
void _ZN5ModelC1Ev(void* p);
}
extern void* data_ov002_0210af70;
extern void* data_ov100_0214857c;

extern "C" void* PathLift_Spawn(void)
{
    char* p = (char*)_ZN9ActorBasenwEj(0x4b4);
    if (p) {
        _ZN8PlatformC2Ev(p);
        *(void**)p = &data_ov002_0210af70;
        func_020733a8(p + 0x320, 3, 0x50, (void*)_ZN5ModelC1Ev, (void*)_ZN5ModelD1Ev);
        _ZN7PathPtrC1Ev(p + 0x430);
        *(void**)p = &data_ov100_0214857c;
        _ZN11ShadowModelC1Ev(p + 0x450);
    }
    return p;
}
