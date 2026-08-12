//cpp
// @symbol func_ov006_0210d7e0
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
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
    _ZN11dScMgBase_cD2Ev(c);
    Memory::Deallocate(c, (Heap*)data_020a0eac);
    return c;
}
