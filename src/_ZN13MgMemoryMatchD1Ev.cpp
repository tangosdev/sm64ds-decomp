//cpp
extern "C" int func_ov006_020c1c64(char* t);
struct SysTracker { ~SysTracker(); char pad[4]; };
extern "C" void _ZN11dScMgBase_cD2Ev(void* c);
extern void* _ZTV13MgMemoryMatch;
extern void* data_ov006_0213e448;
extern "C" void* _ZN13MgMemoryMatchD1Ev(char* c) {
    *(void**)c = &_ZTV13MgMemoryMatch;
    func_ov006_020c1c64(c + 0x4f38);
    *(void**)c = &data_ov006_0213e448;
    ((SysTracker*)(c + 0x471c))->~SysTracker();
    _ZN11dScMgBase_cD2Ev(c);
    return c;
}
