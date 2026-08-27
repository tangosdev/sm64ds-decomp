// @symbol func_ov006_020c0af8
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
void func_ov006_020c0af8(char* c)
{
    struct Matrix4x3 tmp;
    *(volatile struct Matrix4x3*)&tmp = data_ov006_0213ad28;
    *(struct Matrix4x3*)(c + 0x38) = *(struct Matrix4x3*)&data_ov006_0213ad28;
    *(struct Matrix4x3*)(c + 0xa8) = tmp;
}
