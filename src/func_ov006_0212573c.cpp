//cpp
extern "C" {
extern int func_0207328c(void*, int, int, void*);
extern void _ZN5ModelD1Ev(void *);
extern void _ZN8Particle10SysTrackerD1Ev(void *);
extern void func_ov004_020b29c0(void *c);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void func_ov006_02125800();
extern void func_0203d47c();
extern void *data_ov006_0214000c[];
extern void *data_ov006_0213e448[];
extern void *_ZN6Memory11gameHeapPtrE;
void *func_ov006_0212573c(char *c);
void *func_ov006_0212573c(char *c) {
    *(void ***)c = data_ov006_0214000c;
    func_0207328c(c + 0xba14, 0x20, 0x24, (void*)&func_ov006_02125800);
    func_0207328c(c + 0xb5d8, 0x80, 8, (void*)&func_0203d47c);
    func_0207328c(c + 0xacd8, 0x80, 8, (void*)&func_0203d47c);
    _ZN5ModelD1Ev(c + 0xaba4);
    *(void ***)c = data_ov006_0213e448;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    func_ov004_020b29c0(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, *(void **)&_ZN6Memory11gameHeapPtrE);
    return c;
}
}
