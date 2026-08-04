//cpp
struct SysTracker { ~SysTracker(); char pad[4]; };
extern "C" void __destroy_arr(char* p, int a, int b, void* fn);
extern "C" int func_ov006_020c1c64(char* t);
extern "C" void func_ov004_020b29c0(void* c);
extern void* data_ov006_0213d7e8;
extern void* data_ov006_0213e448;
extern "C" void func_ov006_020f8ff0();

extern "C" void* func_ov006_020f8ef4(char* c) {
    *(void**)c = &data_ov006_0213d7e8;
    __destroy_arr(c + 0x51a8, 0x28, 0x30, (void*)&func_ov006_020f8ff0);
    func_ov006_020c1c64(c + 0x4f38);
    *(void**)c = &data_ov006_0213e448;
    ((SysTracker*)(c + 0x471c))->~SysTracker();
    func_ov004_020b29c0(c);
    return c;
}
