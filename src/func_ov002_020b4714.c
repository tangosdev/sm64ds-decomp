// @symbol func_ov002_020b4714
/* recovered: shared common types */
#include "common.h"
extern char* _ZN5Actor15FindWithActorIDEjPS_(unsigned int id, void* from);
extern void _ZN5Actor13LandingDustAtER7Vector3b(char* thiz, struct Vector3* v, int flag);
extern char* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int id, unsigned int p, struct Vector3* pos, void* rot, int e, int f);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(char* thiz);

void func_ov002_020b4714(char* a) {
    char* b;
    b = _ZN5Actor15FindWithActorIDEjPS_(0x13f, 0);
    while (b) {
        if (*(unsigned char*)(a + 0x109) == *(unsigned char*)(b + 0x109)) {
            struct Vector3 v;
            struct Vector3* p;
            char* spawned;
            p = (struct Vector3*)(((long long)(int)(b + 0x5c)));
            v.x = p->x;
            v.y = p->y;
            v.z = p->z;
            _ZN5Actor13LandingDustAtER7Vector3b(a, &v, 1);
            spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0x115, 0xd, (struct Vector3*)(b + 0x5c), 0, *(signed char*)(a + 0xcc), -1);
            *(int*)(spawned + 0xa8) = 0x28000;
            _ZN5Actor24KillAndTrackInDeathTableEv(b);
            _ZN5Actor24KillAndTrackInDeathTableEv(a);
            return;
        }
        b = _ZN5Actor15FindWithActorIDEjPS_(0x13f, b);
    }
}
