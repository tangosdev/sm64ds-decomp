//cpp
// @symbol _ZN13BigBrickBlock17NotifyLinkedActorEv
#include "BigBrickBlock.h"

/* The helper belongs to BigBrickBlock's recovered TU and has one caller:
   BigBrickBlock::Kill. It follows mLinkedActor and clears the per-actor state
   used by actor 0x121 or sets the equivalent state on actors 0x141..0x144. */
void BigBrickBlock::NotifyLinkedActor()
{
    dActor_c* linkedActor = mLinkedActor;
    int actorID;
    if (linkedActor == 0)
        return;

    actorID = linkedActor->actorID;
    if (actorID <= 0x121) {
        if (actorID == 0x121)
            ((u8*)linkedActor)[0x3b0] = 0;
        return;
    }

    switch (actorID) {
    case 0x141:
    case 0x142:
    case 0x143:
    case 0x144:
        ((u8*)linkedActor)[0xd6] = 1;
    }
}
