//cpp
extern "C" int data_ov006_0213eefc;
extern "C" void func_0207328c(void* p, int a, int b, void* fn);
extern "C" void func_ov006_0210d894(void);
extern "C" void func_0203d47c(void);
extern "C" void func_ov004_020b29c0(void* c);
extern "C" void* _ZN6Memory11gameHeapPtrE;
struct Heap;
namespace Memory { void Deallocate(void*, Heap*); }
extern "C" void* func_ov006_0210d7e0(void* c) {
    *(int**)c = &data_ov006_0213eefc;
    func_0207328c((char*)c + 0x599c, 0x40, 0x24, (void*)&func_ov006_0210d894);
    func_0207328c((char*)c + 0x48d4, 0x10, 8, (void*)&func_0203d47c);
    func_0207328c((char*)c + 0x4854, 0x10, 8, (void*)&func_0203d47c);
    func_0207328c((char*)c + 0x47c8, 5, 8, (void*)&func_0203d47c);
    func_ov004_020b29c0(c);
    Memory::Deallocate(c, (Heap*)_ZN6Memory11gameHeapPtrE);
    return c;
}
