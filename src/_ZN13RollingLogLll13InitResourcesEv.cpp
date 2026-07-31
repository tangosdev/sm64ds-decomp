//cpp
// @symbol _ZN13RollingLogLll13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RollingLogLll.h"
struct Actor;
struct MovingCylinderClsn {
    void Init(Actor *a, int b, int c, unsigned int d, unsigned int e);
};

int RollingLogLll::InitResources()
{
    char *c = (char*)((Actor *)this);
    *(int*)(c + 0xa0) = -0xc8000;
    ((MovingCylinderClsn*)(c + 0xd4))->Init(((Actor *)this), 0x1e000, 0x1e000, 0x200002, 0);
    func_ov022_02112790((void*)((Actor *)this), (void*)&data_ov022_02114690);
    return 1;
}
