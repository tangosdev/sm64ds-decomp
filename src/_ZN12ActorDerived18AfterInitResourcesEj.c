#include "types.h"
typedef struct ActorDerived { char pad[0]; } ActorDerived;

extern void _ZN9ActorBase18MarkForDestructionEv(ActorDerived* self);
extern void _ZN9ActorBase18AfterInitResourcesEj(ActorDerived* self, u32 result);

void _ZN12ActorDerived18AfterInitResourcesEj(ActorDerived* self, u32 result) {
    if (result == 1) {
        _ZN9ActorBase18MarkForDestructionEv(self);
    }
    _ZN9ActorBase18AfterInitResourcesEj(self, result);
}
