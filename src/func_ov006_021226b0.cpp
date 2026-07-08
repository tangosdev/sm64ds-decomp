//cpp
extern "C" int data_ov006_0213fc7c;
extern "C" int data_ov006_0213c62c;
extern "C" int _ZN6Memory11gameHeapPtrE;
extern "C" void func_ov006_02120938();
extern "C" void func_ov006_02122c68();
extern "C" void func_ov006_020eed64();
extern "C" void func_ov006_021227c8();
extern "C" void func_ov006_020d1008();
extern "C" void func_ov006_020ca604();
extern "C" void func_0207328c(void *p, int a, int b, void (*fn)());
extern "C" void _ZN8Particle10SysTrackerD1Ev(void *p);
extern "C" void func_ov004_020b29c0(void *c);
extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *p, void *heap);

extern "C" void *func_ov006_021226b0(char *thiz)
{
    *(int**)thiz = &data_ov006_0213fc7c;
    func_0207328c(thiz + 0x7ad0, 5, 0x24, &func_ov006_02120938);
    func_0207328c(thiz + 0x7164, 0x14, 0x78, &func_ov006_02122c68);
    func_0207328c(thiz + 0x6ffc, 0xa, 0x24, &func_ov006_020eed64);
    func_0207328c(thiz + 0x5ddc, 0xa, 0x1d0, &func_ov006_021227c8);
    func_0207328c(thiz + 0x5458, 3, 0x32c, &func_ov006_020d1008);
    func_0207328c(thiz + 0x500c, 5, 0xdc, &func_ov006_020ca604);
    *(int**)thiz = &data_ov006_0213c62c;
    _ZN8Particle10SysTrackerD1Ev(thiz + 0x47e4);
    func_ov004_020b29c0(thiz);
    _ZN6Memory10DeallocateEPvP4Heap(thiz, *(void**)&_ZN6Memory11gameHeapPtrE);
    return thiz;
}
