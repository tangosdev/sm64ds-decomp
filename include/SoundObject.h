#ifndef SOUNDOBJECT_H
#define SOUNDOBJECT_H

#include "types.h"
#include "Actor.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   SoundObject_Spawn  ActorBase::operator new(228 = 0xe4), Actor::Actor(), stores _ZTV11SoundObject,
 *                 then the members below in this order.
 *   ~SoundObject   the same members destroyed in reverse, then ~Actor.
 *
 * SIZE 0xe4 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV5Actor. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct SoundObject : Actor {
    u8  pad_0d0[0x14];

    virtual ~SoundObject();            /* slots 16 (D1), 17 (D0) */
};

typedef char SoundObject_size_must_be_0xe4[sizeof(SoundObject) == 0xe4 ? 1 : -1];

#endif /* SOUNDOBJECT_H */
