//cpp
extern "C" {
extern int func_0207328c(void*, int, int, void*);
extern void _ZN5ModelD1Ev(void *);
extern void _ZN8Particle10SysTrackerD1Ev(void *);
extern void func_ov004_020b29c0(void *c);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void _ZN6Player18St_LevelEnter_MainEv();
extern void func_ov006_020c893c();
extern void func_ov006_020eed64();
extern void *data_ov006_0213ccfc[];
extern void *_ZTV17MgBounceAndPounce[];
extern void *data_020a0eac;
void *func_ov006_020eec9c(char *c);
void *func_ov006_020eec9c(char *c) {
    *(void ***)c = data_ov006_0213ccfc;
    _ZN5ModelD1Ev(c + 0x5a14);
    func_0207328c(c + 0x57d4, 0x10, 0x24, (void*)&func_ov006_020eed64);
    func_0207328c(c + 0x5234, 6, 0xf0, (void*)&_ZN6Player18St_LevelEnter_MainEv);
    func_0207328c(c + 0x500c, 3, 0xb8, (void*)&func_ov006_020c893c);
    *(void ***)c = _ZTV17MgBounceAndPounce;
    _ZN8Particle10SysTrackerD1Ev(c + 0x47e4);
    func_ov004_020b29c0(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, *(void **)&data_020a0eac);
    return c;
}
}
