//cpp
// @symbol func_ov006_0210d7e0
// recovered name: dScMgSmartball_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgSmartball_c::OnYoshiTryEat - recovered from vtable slot identity */
extern "C" void __destroy_arr(void* p, int a, int b, void* fn);
extern "C" void NullDestructor_0203d47c(void);
extern "C" void* data_020a0eac;
struct Heap;
namespace Memory { void Deallocate(void*, Heap*); }
extern "C" void* func_ov006_0210d7e0(void* c) {
    *(int**)c = &data_ov006_0213eefc;
    __destroy_arr((char*)c + 0x599c, 0x40, 0x24, (void*)&func_ov006_0210d894);
    __destroy_arr((char*)c + 0x48d4, 0x10, 8, (void*)&NullDestructor_0203d47c);
    __destroy_arr((char*)c + 0x4854, 0x10, 8, (void*)&NullDestructor_0203d47c);
    __destroy_arr((char*)c + 0x47c8, 5, 8, (void*)&NullDestructor_0203d47c);
    func_ov004_020b29c0(c);
    Memory::Deallocate(c, (Heap*)data_020a0eac);
    return c;
}
