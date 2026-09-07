//cpp
// @symbol func_ov102_02149288
/* recovered: shared common types */
#include "common.h"

struct Vector3_16;
extern "C" {
void func_ov102_02149684(Vector3* v, void* c);
void* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned a, unsigned b, const Vector3& v, const Vector3_16* p, int e, int f);
}
extern "C" void func_ov102_02149288(char* c){
    Vector3 v;
    func_ov102_02149684(&v, c);
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x132u, 0u, v, 0, *(signed char*)(c+0xcc), -1);
}
