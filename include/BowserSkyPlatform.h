#ifndef BOWSERSKYPLATFORM_H
#define BOWSERSKYPLATFORM_H

#include "types.h"
#include "Platform.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   BowserSkyPlatform_Spawn  ActorBase::operator new(812 = 0x32c), Platform::Platform(), stores _ZTV17BowserSkyPlatform,
 *                 then the members below in this order.
 *   ~BowserSkyPlatform   the same members destroyed in reverse, then ~Platform.
 *
 * SIZE 0x32c is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8Platform. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct BowserSkyPlatform : Platform {
    u8  pad_320[0xc];

    virtual ~BowserSkyPlatform();            /* slots 16 (D1), 17 (D0) */

    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */
};

typedef char BowserSkyPlatform_size_must_be_0x32c[sizeof(BowserSkyPlatform) == 0x32c ? 1 : -1];

#endif /* BOWSERSKYPLATFORM_H */
