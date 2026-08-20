//cpp
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned int);
extern void _ZN10dBgActor_cC2Ev(void*);
extern void _ZN8dCcPos_cD1Ev(void*);
extern void _ZN8dCcPos_cC1Ev(void*);
extern void func_020733a8(void* arr, int count, int size, void* ctor, void* dtor);
extern void* _ZTV15RotatingFirebar[];
int* RotatingFirebar_Spawn(void) {
    int* p = (int*)_ZN7fBase_cnwEj(0x540);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        *(void***)p = (void**)_ZTV15RotatingFirebar;
        func_020733a8((char*)p + 0x360, 8, 0x3c, _ZN8dCcPos_cC1Ev, _ZN8dCcPos_cD1Ev);
    }
    return p;
}
}
