#ifndef SUBMARINE_H
#define SUBMARINE_H

#include "types.h"
#include "dBgActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daObjWlSubmarine_c_classInit (historically Submarine_Spawn)
 *                 fBase_c::operator new(800 = 0x320), dBgActor_c::dBgActor_c(), stores _ZTV9Submarine,
 *                 then the members below in this order.
 *   ~Submarine   the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x320 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct Submarine : dBgActor_c {

    /* The destructor pair spelled as two plain virtuals on the host, plus
       the non-virtual destructor declaration the src/ definitions need; the
       whole ruling is in include/ModelBase.h. An override takes its base's
       slots, so these carry the SAME TWO NAMES the base declares -- a fresh
       name would append a slot instead of claiming one. */
#ifdef _MSC_VER
    virtual void Destructor1();   /* D1 */
    virtual void Destructor0();   /* D0 */
    ~Submarine();   /* no slot */
#else
    virtual ~Submarine();   /* D1 and D0 */
#endif

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Render();                /* slot  9 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Submarine_size_must_be_0x320[sizeof(Submarine) == 0x320 ? 1 : -1];
#endif

#endif /* SUBMARINE_H */
