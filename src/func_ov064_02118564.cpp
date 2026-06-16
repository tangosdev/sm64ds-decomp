//cpp
extern "C" {
extern void* _ZN9ActorBasenwEj(unsigned int);
extern void _ZN8PlatformC2Ev(void*);
extern void _ZN19CylinderClsnWithPosD1Ev(void*);
extern void _ZN19CylinderClsnWithPosC1Ev(void*);
extern void func_020733a8(void* arr, int count, int size, void* ctor, void* dtor);
extern void* data_ov064_0211be10[];
int* func_ov064_02118564(void) {
    int* p = (int*)_ZN9ActorBasenwEj(0x540);
    if (p) {
        _ZN8PlatformC2Ev(p);
        *(void***)p = (void**)data_ov064_0211be10;
        func_020733a8((char*)p + 0x360, 8, 0x3c, _ZN19CylinderClsnWithPosC1Ev, _ZN19CylinderClsnWithPosD1Ev);
    }
    return p;
}
}
