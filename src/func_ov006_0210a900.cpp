//cpp
struct Heap;
namespace Memory { void Deallocate(void* p, Heap* h); }
extern "C" void func_ov004_020b29c0(void* c);
extern void* data_ov006_0213eb40;
extern void* data_ov006_0213e5d4;
extern void func_020ad494(void);
extern Heap* data_020a0eac;
extern "C" void* func_ov006_0210a900(char* c) {
    *(void**)c = &data_ov006_0213eb40;
    *(void**)(c + 0x4660) = &data_ov006_0213e5d4;
    *(void**)(c + 0x4660) = (void*)&func_020ad494;
    func_ov004_020b29c0(c);
    Memory::Deallocate(c, data_020a0eac);
    return c;
}
