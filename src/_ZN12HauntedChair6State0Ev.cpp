//cpp
// @symbol _ZN12HauntedChair6State0Ev
#include "HauntedChair.h"

typedef int Fix12i;
extern "C" {
extern void *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, void *start);
extern Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);
}

void HauntedChair::State0()
{
    char *c = (char *)this;
    *(int*)(c + 0x378) = 1;
    *(short*)(c + 0x300 + 0xa0) = 1;
    *(short*)(c + 0x300 + 0xa2) = 0;
    *(short*)(c + 0x300 + 0x98) = 0;
    *(short*)(c + 0x300 + 0x9a) = 0;
    *(short*)(c + 0x300 + 0x9c) = 0;
    void *found = _ZN8dActor_c15FindWithActorIDEjPS_(0xf9, 0);
    *(int*)(c + 0x37c) = 0;
    if (!found)
        return;
    Fix12i d = Vec3_Dist((Vector3*)(c + 0x5c), (Vector3*)((char*)found + 0x5c));
    if (d < 0x12c000) {
        *(int*)(c + 0x37c) = *(int*)((char*)found + 4);
        *(short*)(c + 0x300 + 0xa0) = 0;
    }
}
