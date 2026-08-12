//cpp
extern "C" int func_ov006_020c1c64(char* t);
struct SysTracker { ~SysTracker(); char pad[4]; };
extern "C" void _ZN11dScMgBase_cD2Ev(void* c);
extern void* data_ov006_0213d1b8;
extern void* _ZTV19dScMgSingle3DBase_c;
extern "C" void* func_ov006_020f3834(char* c) {
    *(void**)c = &data_ov006_0213d1b8;
    func_ov006_020c1c64(c + 0x4f38);
    *(void**)c = &_ZTV19dScMgSingle3DBase_c;
    ((SysTracker*)(c + 0x471c))->~SysTracker();
    _ZN11dScMgBase_cD2Ev(c);
    return c;
}
