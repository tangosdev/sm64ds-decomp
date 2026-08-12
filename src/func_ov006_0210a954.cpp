//cpp
extern "C" int func_ov006_020c21e4(char* t);
struct SysTracker { ~SysTracker(); char pad[4]; };
extern "C" void _ZN11dScMgBase_cD2Ev(void* c);
extern void* data_ov006_0213eaa8;
extern void* _ZTV19dScMgSingle3DBase_c;
extern "C" void* func_ov006_0210a954(char* c) {
    *(void**)c = &data_ov006_0213eaa8;
    func_ov006_020c21e4(c + 0x4f38);
    *(void**)c = &_ZTV19dScMgSingle3DBase_c;
    ((SysTracker*)(c + 0x471c))->~SysTracker();
    _ZN11dScMgBase_cD2Ev(c);
    return c;
}
