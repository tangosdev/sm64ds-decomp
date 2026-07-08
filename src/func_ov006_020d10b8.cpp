//cpp
extern "C" int data_ov006_0213b918;
extern "C" void func_0207328c(void* p, int a, int b, void* fn);
extern "C" void func_ov006_020d116c(void);
extern "C" void func_0203d47c(void);
extern "C" void func_ov004_020b29c0(void* c);
extern "C" void* _ZN6Memory11gameHeapPtrE;
struct Heap;
namespace Memory { void Deallocate(void*, Heap*); }
extern "C" void* func_ov006_020d10b8(void* c) {
    *(int**)c = &data_ov006_0213b918;
    func_0207328c((char*)c + 0x4768, 0x80, 0x18, (void*)&func_ov006_020d116c);
    func_0207328c((char*)c + 0x4744, 4, 8, (void*)&func_0203d47c);
    func_0207328c((char*)c + 0x4724, 4, 8, (void*)&func_0203d47c);
    func_0207328c((char*)c + 0x4660, 4, 8, (void*)&func_0203d47c);
    func_ov004_020b29c0(c);
    Memory::Deallocate(c, (Heap*)_ZN6Memory11gameHeapPtrE);
    return c;
}
