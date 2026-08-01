//cpp
// @symbol func_ov006_0210a900
// recovered name: dScMgSlot1_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgSlot1_c::OnYoshiTryEat - recovered from vtable slot identity */
struct Heap;
namespace Memory { void Deallocate(void* p, Heap* h); }
extern Heap* data_020a0eac;
extern "C" void* func_ov006_0210a900(char* c) {
    *(void**)c = &data_ov006_0213eb40;
    *(void**)(c + 0x4660) = &data_ov006_0213e5d4;
    *(void**)(c + 0x4660) = (void*)&func_020ad494;
    func_ov004_020b29c0(c);
    Memory::Deallocate(c, data_020a0eac);
    return c;
}
