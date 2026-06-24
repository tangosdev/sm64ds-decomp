//cpp
extern "C" {
extern int func_0207328c(void*, int, int, void*);
extern int func_ov006_020c3288(char *t);
extern void _ZN8Particle10SysTrackerD1Ev(void *);
extern void func_ov004_020b29c0(void *c);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void func_0203d47c();
extern void func_ov006_020deac4();
extern void *data_ov006_0213c154[];
extern void *data_ov006_0213e448[];
extern void *data_020a0eac;
void *func_ov006_020dea1c(char *c);
void *func_ov006_020dea1c(char *c) {
    *(void ***)c = data_ov006_0213c154;
    func_0207328c(c + 0x53e8, 3, 8, (void*)&func_0203d47c);
    func_0207328c(c + 0x50e8, 0x20, 0x18, (void*)&func_ov006_020deac4);
    func_ov006_020c3288(c + 0x4f38);
    *(void ***)c = data_ov006_0213e448;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    func_ov004_020b29c0(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, *(void **)&data_020a0eac);
    return c;
}
}
