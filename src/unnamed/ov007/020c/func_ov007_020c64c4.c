// @symbol func_ov007_020c64c4
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern void SubVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern int LenVec3(struct Vector3 *a);

void func_ov007_020c64c4(char* c, int b) {
    char* r4;
    int i;
    for (i = 0; i < *(int*)(c + 8); i++) {
        func_ov007_020c6314(c, i, b);
    }
    if (*(int*)(c + 4) != 2) return;
    *(struct Vector3*)(c + 0x98) = *(struct Vector3*)(c + 0x70);
    r4 = *(char**)(c + 0xa4);
    SubVec3(*(struct Vector3**)*(char**)r4, *(struct Vector3**)*(char**)(r4 + 4), (struct Vector3*)(r4 + 0xc));
    *(int*)(r4 + 8) = LenVec3((struct Vector3*)(r4 + 0xc));
}
