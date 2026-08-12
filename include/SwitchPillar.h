#ifndef SWITCHPILLAR_H
#define SWITCHPILLAR_H

#include "types.h"
#include "Platform.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   SwitchPillar_Spawn  ActorBase::operator new(800 = 0x320), Platform::Platform(), stores _ZTV12SwitchPillar,
 *                 then the members below in this order.
 *   ~SwitchPillar   the same members destroyed in reverse, then ~Platform.
 *
 * SIZE 0x320 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8Platform. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct SwitchPillar : Platform {

    virtual ~SwitchPillar();            /* slots 16 (D1), 17 (D0) */
};

typedef char SwitchPillar_size_must_be_0x320[sizeof(SwitchPillar) == 0x320 ? 1 : -1];

#endif /* SWITCHPILLAR_H */
