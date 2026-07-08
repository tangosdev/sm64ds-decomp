//cpp
extern "C" int data_ov006_0213fb34;
extern "C" int data_ov006_0213c62c;
extern "C" int _ZN6Memory11gameHeapPtrE;
extern "C" void func_ov006_02120938();
extern "C" void func_ov006_020d1008();
extern "C" void func_ov006_020ccfc8();
extern "C" void func_0207328c(void *p, int a, int b, void (*fn)());
extern "C" void _ZN8Particle10SysTrackerD1Ev(void *p);
extern "C" void func_ov004_020b29c0(void *c);
extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *p, void *heap);

extern "C" void *func_ov006_02120880(char *thiz)
{
    *(int**)thiz = &data_ov006_0213fb34;
    func_0207328c(thiz + 0x5cd0, 5, 0x24, &func_ov006_02120938);
    func_0207328c(thiz + 0x534c, 3, 0x32c, &func_ov006_020d1008);
    func_0207328c(thiz + 0x500c, 4, 0xd0, &func_ov006_020ccfc8);
    *(int**)thiz = &data_ov006_0213c62c;
    _ZN8Particle10SysTrackerD1Ev(thiz + 0x47e4);
    func_ov004_020b29c0(thiz);
    _ZN6Memory10DeallocateEPvP4Heap(thiz, *(void**)&_ZN6Memory11gameHeapPtrE);
    return thiz;
}
