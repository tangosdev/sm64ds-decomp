//cpp
extern "C" int func_ov006_020c1c64(char* t);
struct SysTracker { ~SysTracker(); char pad[4]; };
extern "C" void func_ov004_020b29c0(void* c);
extern void* data_ov006_0213d4d4;
extern void* data_ov006_0213e448;
extern "C" void* func_ov006_020f5564(char* c) {
    *(void**)c = &data_ov006_0213d4d4;
    func_ov006_020c1c64(c + 0x4f38);
    *(void**)c = &data_ov006_0213e448;
    ((SysTracker*)(c + 0x471c))->~SysTracker();
    func_ov004_020b29c0(c);
    return c;
}
