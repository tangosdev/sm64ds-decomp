//cpp
extern "C" {
extern void func_ov006_020c3e70(char *t);
extern int func_0207328c(void*, int, int, void*);
extern void _ZN8Particle10SysTrackerD1Ev(void *);
extern void func_ov004_020b29c0(void *c);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void func_ov006_0212a650();
extern void *data_ov006_02140140[];
extern void *data_ov006_0213e448[];
extern void *_ZN6Memory11gameHeapPtrE;
void *func_ov006_0212a5c8(char *c);
void *func_ov006_0212a5c8(char *c) {
    *(void ***)c = data_ov006_02140140;
    func_ov006_020c3e70(c + 0x51f8);
    func_0207328c(c + 0x4f38, 0x16, 0x20, (void*)&func_ov006_0212a650);
    *(void ***)c = data_ov006_0213e448;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    func_ov004_020b29c0(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, *(void **)&_ZN6Memory11gameHeapPtrE);
    return c;
}
}
