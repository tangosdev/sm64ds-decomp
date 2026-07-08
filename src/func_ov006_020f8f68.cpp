//cpp
extern "C" {
extern int func_0207328c(void*, int, int, void*);
extern int func_ov006_020c1c64(char *t);
extern void _ZN8Particle10SysTrackerD1Ev(void *);
extern void func_ov004_020b29c0(void *c);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void func_ov006_020f8ff0();
extern void *data_ov006_0213d7e8[];
extern void *data_ov006_0213e448[];
extern void *_ZN6Memory11gameHeapPtrE;
void *func_ov006_020f8f68(char *c);
void *func_ov006_020f8f68(char *c) {
    *(void ***)c = data_ov006_0213d7e8;
    func_0207328c(c + 0x51a8, 0x28, 0x30, (void*)&func_ov006_020f8ff0);
    func_ov006_020c1c64(c + 0x4f38);
    *(void ***)c = data_ov006_0213e448;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    func_ov004_020b29c0(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, *(void **)&_ZN6Memory11gameHeapPtrE);
    return c;
}
}
