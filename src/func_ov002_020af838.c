#include "types.h"
// @symbol func_ov002_020af838
/* recovered: shared common types */
#include "common.h"
struct Vector3_16;

extern void _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 a, u32 b, struct Vector3* v, struct Vector3_16* rot, int e, int f);
extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void* thiz);

void func_ov002_020af838(char* c)
{
    struct Vector3 vec;

    vec.x = *(Fix12i*)(c + 0x378);
    vec.y = *(Fix12i*)(c + 0x37c);
    vec.z = *(Fix12i*)(c + 0x380);
    vec.y = *(Fix12i*)(c + 0x37c) + 0x32000;
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        *(unsigned short*)(c + 0xc), 0x25, &vec, 0, *(signed char*)(c + 0xcc), -1);

    vec.y = *(Fix12i*)(c + 0x37c);
    vec.x = *(Fix12i*)(c + 0x378) - 0x1f4000;
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        *(unsigned short*)(c + 0xc), 0xb, &vec, 0, *(signed char*)(c + 0xcc), -1);

    vec.x = *(Fix12i*)(c + 0x378) + 0x1f4000;
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        *(unsigned short*)(c + 0xc), 0xb, &vec, 0, *(signed char*)(c + 0xcc), -1);

    _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
}
