//cpp
extern "C" int data_ov006_0213cab8;
extern "C" void func_0207328c(void* p, int a, int b, void* fn);
extern "C" void func_ov006_020ea324(void);
extern "C" void func_ov004_020b29c0(void* c);
extern "C" void* data_020a0eac;
struct Heap;
namespace Memory { void Deallocate(void*, Heap*); }
extern "C" void* func_ov006_020ea2c8(void* c) {
    *(int**)c = &data_ov006_0213cab8;
    func_0207328c((char*)c + 0x4678, 0xf, 0x98, (void*)&func_ov006_020ea324);
    func_ov004_020b29c0(c);
    Memory::Deallocate(c, (Heap*)data_020a0eac);
    return c;
}
