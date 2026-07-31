//cpp
// @symbol func_ov102_021493dc
/* recovered: shared common types */
#include "common.h"


extern "C" int func_ov102_02149684(void* sp, void* c);
extern "C" int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int,unsigned int,const Vector3&,const Vector3_16*,int,int);
extern "C" void func_ov102_021493dc(void* c) {
    Vector3 v;
    func_ov102_02149684(&v, c);
    signed char cc = *(signed char*)((char*)c+0xcc);
    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x115, 0, v, (const Vector3_16*)0, (int)cc, -1);
}
