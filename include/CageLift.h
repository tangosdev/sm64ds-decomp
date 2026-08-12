#ifndef CAGELIFT_H
#define CAGELIFT_H

#include "types.h"
#include "Platform.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   CageLift_Spawn  ActorBase::operator new(816 = 0x330), Platform::Platform(), stores _ZTV8CageLift,
 *                 then the members below in this order.
 *   ~CageLift   the same members destroyed in reverse, then ~Platform.
 *
 * SIZE 0x330 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8Platform. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct CageLift : Platform {
    u8  pad_320[0x10];

    virtual ~CageLift();            /* slots 16 (D1), 17 (D0) */
};

typedef char CageLift_size_must_be_0x330[sizeof(CageLift) == 0x330 ? 1 : -1];

#endif /* CAGELIFT_H */
