// @symbol func_ov006_020c61c4
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *out);
void func_ov006_020c61c4(int this) {
    int f24 = *(int*)(this + 0x24);
    int a0  = *(int*)(this + 0xa0);
    if (a0 < f24 + *(int*)(this + 0x18) && ((int*)this)[43] < 0) {
        ((int*)this)[43] = -((int*)this)[43];
        ((short*)this)[117] = 0;
    } else if (a0 > f24 + *(int*)(this + 0x1c) && ((int*)this)[43] > 0) {
        ((int*)this)[43] = -((int*)this)[43];
        ((short*)this)[117] = 0;
    }
    AddVec3((struct Vector3*)(this + 0x9c), (struct Vector3*)(this + 0xa8), (struct Vector3*)(this + 0x9c));
    func_ov006_020c49d8((void*)this);
}
