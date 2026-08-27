//cpp
// @symbol _ZN9MontyMole8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "MontyMole.h"
struct C;
typedef void (C::*PMF)();
struct Entry { PMF pmf[1]; };
extern Entry data_ov080_02128438[];
struct C { char pad[0x17c]; int idx; };
extern "C" {
void _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(void* self, void* cyl);
int func_ov080_02124208(void* c);
void func_ov080_021243d8(char* t);
void _ZN5dCc_c5ClearEv(void* c);
void _ZN5dCc_c6UpdateEv(void* c);
}

int MontyMole::Behavior()
{
    char* p = (char*)((C*)this);
    _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(p, p + 0x138);
    int j = ((C*)this)->idx;
    (((C*)this)->*data_ov080_02128438[j].pmf[0])();
    func_ov080_02124208(p);
    func_ov080_021243d8(p);
    _ZN5dCc_c5ClearEv(p + 0x138);
    _ZN5dCc_c6UpdateEv(p + 0x138);
    return 1;
}
