#include "types.h"
// Actor::FindExplosionActor - find the actor that caused an explosion hit
struct CylinderClsn {
    void* vtable;
    int radius;
    int height;
    int pushbackX;
    int pushbackY;
    int pushbackZ;
    u32 flags1;
    u32 vulnerableFlags;
    u32 hitFlags;
    u32 otherObjID;
};

struct Actor {
    void* vtable;
};

extern struct Actor* _ZN5Actor10FindWithIDEj(u32 id);

struct Actor* _ZN5Actor18FindExplosionActorER12CylinderClsn(struct Actor* self, struct CylinderClsn* clsn) {
    u32 id = clsn->otherObjID;
    u32 flags;
    if (id != 0) {
        flags = clsn->hitFlags & 0x4000;
        if (flags != 0)
            return _ZN5Actor10FindWithIDEj(id);
    }
    return 0;
}