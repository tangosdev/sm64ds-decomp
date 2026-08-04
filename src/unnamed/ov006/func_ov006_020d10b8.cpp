//cpp
// @symbol func_ov006_020d10b8
// recovered name: dScMgAmida_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgAmida_c::OnYoshiTryEat - recovered from vtable slot identity */
extern "C" void __destroy_arr(void* p, int a, int b, void* fn);
extern "C" void NullDestructor_0203d47c(void);
extern "C" void* data_020a0eac;
struct Heap;
namespace Memory { void Deallocate(void*, Heap*); }
extern "C" void* func_ov006_020d10b8(void* c) {
    *(int**)c = &data_ov006_0213b918;
    __destroy_arr((char*)c + 0x4768, 0x80, 0x18, (void*)&func_ov006_020d116c);
    __destroy_arr((char*)c + 0x4744, 4, 8, (void*)&NullDestructor_0203d47c);
    __destroy_arr((char*)c + 0x4724, 4, 8, (void*)&NullDestructor_0203d47c);
    __destroy_arr((char*)c + 0x4660, 4, 8, (void*)&NullDestructor_0203d47c);
    func_ov004_020b29c0(c);
    Memory::Deallocate(c, (Heap*)data_020a0eac);
    return c;
}
