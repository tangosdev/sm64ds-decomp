//cpp
// @symbol func_ov006_020ea2c8
// @emits dScMgHanachan_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgHanachan_c::OnYoshiTryEat - recovered from vtable slot identity */
extern "C" void func_0207328c(void* p, int a, int b, void* fn);
extern "C" void* data_020a0eac;
struct Heap;
namespace Memory { void Deallocate(void*, Heap*); }
extern "C" void* dScMgHanachan_c_OnYoshiTryEat(void* c) {
    *(int**)c = &data_ov006_0213cab8;
    func_0207328c((char*)c + 0x4678, 0xf, 0x98, (void*)&func_ov006_020ea324);
    func_ov004_020b29c0(c);
    Memory::Deallocate(c, (Heap*)data_020a0eac);
    return c;
}
