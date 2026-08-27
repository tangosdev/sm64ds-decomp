//cpp
// @symbol func_ov078_02125f8c
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"


extern "C" {
extern struct Matrix4x3* _ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3(void* self, void* player, struct Vector3* pos);
void func_ov078_02125f8c(void* c_){
    char* c = (char*)c_;
    void* player = *(void**)(c+0x494);
    int idx = 0;
    if(player == 0) return;
    if(*(int*)((char*)player+8) == 2) idx = 1;
    struct Matrix4x3* res = _ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3(c, player, &data_ov078_0212711c[idx]);
    *(struct Matrix4x3*)(c+0x2e8) = *res;
}
}
