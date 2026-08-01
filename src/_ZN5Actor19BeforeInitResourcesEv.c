#include "types.h"
typedef struct Actor {
    char pad[0xb0];
    u32 flags;
} Actor;

extern int _ZN5Actor18GetBitInDeathTableEv(Actor* self);
extern void _ZN9ActorBase18MarkForDestructionEv(Actor* self);
extern int _ZN9ActorBase19BeforeInitResourcesEv(Actor* self);

int _ZN5Actor19BeforeInitResourcesEv(Actor* self) {
    int spawnEvenIfKilledBefore;
    spawnEvenIfKilledBefore = (self->flags & 0x8000000) ? 1 : 0;
    if (spawnEvenIfKilledBefore != 0) {
        goto skip;
    }
    if (_ZN5Actor18GetBitInDeathTableEv(self) == 0) {
        goto skip;
    }
    _ZN9ActorBase18MarkForDestructionEv(self);
    return 1;
skip:;
    {
        int r = _ZN9ActorBase19BeforeInitResourcesEv(self);
        if (r == 0) return 0;
        return 1;
    }
}
