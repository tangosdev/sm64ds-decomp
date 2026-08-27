// @symbol func_ov060_02115b0c
/* recovered: shared common types */
#include "common.h"
extern void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void* self, struct Vector3* v, int f);
extern void* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, struct Vector3* pos, void* rot, int e, int f);

void func_ov060_02115b0c(char* c)
{
    struct Vector3 v;
    if (*(unsigned char*)(c + 0x414) != 2) return;
    v.x = *(int*)(c + 0x5c);
    v.y = *(int*)(c + 0x60);
    v.z = *(int*)(c + 0x64);
    _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(c, &v, 0x7d0000);
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x119, 0, (struct Vector3*)(c + 0x5c), 0, *(signed char*)(c + 0xcc), -1);
}
