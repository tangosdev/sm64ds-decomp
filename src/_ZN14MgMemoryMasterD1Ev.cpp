//cpp
extern "C" int func_ov006_020c1c64(char* t);
struct SysTracker { ~SysTracker(); char pad[4]; };
extern "C" void _ZN11dScMgBase_cD2Ev(void* c);
extern void* _ZTV14MgMemoryMaster;
extern void* data_ov006_0213e448;
extern "C" void* _ZN14MgMemoryMasterD1Ev(char* c) {
    *(void**)c = &_ZTV14MgMemoryMaster;
    func_ov006_020c1c64(c + 0x4f38);
    *(void**)c = &data_ov006_0213e448;
    ((SysTracker*)(c + 0x471c))->~SysTracker();
    _ZN11dScMgBase_cD2Ev(c);
    return c;
}
