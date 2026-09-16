//cpp
#include "dActor_c.h"
#include "dCc_c.h"
// @symbol _ZN9MontyMole8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "MontyMole.h"
struct C;
typedef void (C::*PMF)();
struct Entry { PMF pmf[1]; };
extern Entry data_ov080_02128438[];
struct C { char pad[0x17c]; int idx; };
extern "C" {
int func_ov080_02124208(void* c);
void func_ov080_021243d8(char* t);
}

int MontyMole::Behavior()
{
    char* p = (char*)((C*)this);
    ((dActor_c *)p)->MakeVanishLuigiWork(*(dCc_c *)(p + 0x138));
    int j = ((C*)this)->idx;
    (((C*)this)->*data_ov080_02128438[j].pmf[0])();
    func_ov080_02124208(p);
    func_ov080_021243d8(p);
    ((dCc_c *)(p + 0x138))->Clear();
    ((dCc_c *)(p + 0x138))->Update();
    return 1;
}
