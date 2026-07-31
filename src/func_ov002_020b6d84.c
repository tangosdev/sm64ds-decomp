#include "types.h"
// @symbol func_ov002_020b6d84
// @emits daObjLava_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjLava_c::Behavior - recovered from vtable slot identity */
struct Vec3 {
    Fix12 x, y, z;
};

struct Player {
    char pad[0x5c];
    struct Vec3 pos;
};

struct Actor {
    char pad[0xd4];
    u32 uniqueID;
};

extern struct Player* _ZN5Actor13ClosestPlayerEv(struct Actor* self);
extern u32 func_02022c3c(u32 uniqueID, u32 effectID, Fix12 x, Fix12 y, Fix12 z, const void* dir);

int daObjLava_c_Behavior(struct Actor* self) {
    struct Vec3* v = (struct Vec3*)(((int)_ZN5Actor13ClosestPlayerEv(self) + 0x5c));
    self->uniqueID = func_02022c3c(self->uniqueID, 0xb7, v->x, v->y, v->z, 0);
    return 1;
}
