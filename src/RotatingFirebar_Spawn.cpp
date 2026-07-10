//cpp
extern "C" {
extern void* _ZN9ActorBasenwEj(unsigned int);
extern void _ZN8PlatformC2Ev(void*);
extern void _ZN19CylinderClsnWithPosD1Ev(void*);
extern void _ZN19CylinderClsnWithPosC1Ev(void*);
extern void func_020733a8(void* arr, int count, int size, void* ctor, void* dtor);
extern void* _ZTV15RotatingFirebar[];
int* RotatingFirebar_Spawn(void) {
    int* p = (int*)_ZN9ActorBasenwEj(0x540);
    if (p) {
        _ZN8PlatformC2Ev(p);
        *(void***)p = (void**)_ZTV15RotatingFirebar;
        func_020733a8((char*)p + 0x360, 8, 0x3c, _ZN19CylinderClsnWithPosC1Ev, _ZN19CylinderClsnWithPosD1Ev);
    }
    return p;
}
}
