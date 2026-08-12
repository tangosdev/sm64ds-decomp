//cpp
// @symbol func_ov006_020ea2c8
// recovered name: dScMgHanachan_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgHanachan_c::OnYoshiTryEat - recovered from vtable slot identity */
extern "C" void __destroy_arr(void* p, int a, int b, void* fn);
extern "C" void* data_020a0eac;
struct Heap;
namespace Memory { void Deallocate(void*, Heap*); }
extern "C" void* func_ov006_020ea2c8(void* c) {
    *(int**)c = &data_ov006_0213cab8;
    __destroy_arr((char*)c + 0x4678, 0xf, 0x98, (void*)&func_ov006_020ea324);
    _ZN11dScMgBase_cD2Ev(c);
    Memory::Deallocate(c, (Heap*)data_020a0eac);
    return c;
}
