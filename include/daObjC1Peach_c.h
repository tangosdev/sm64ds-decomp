#ifndef DAOBJC1PEACH_C_H
#define DAOBJC1PEACH_C_H

#include "dActor_c.h"
#include "Model.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daObjC1Peach_c_classInit  fBase_c::operator new(296 = 0x128), dActor_c::dActor_c(),
 *                 stores _ZTV14daObjC1Peach_c, then constructs the Model at 0x0d4.
 *   ~daObjC1Peach_c  destroys nothing of its own, then ~dActor_c.
 *
 * SIZE 0x128 is the factory's own literal, and the last member closes exactly on it.
 *
 * The __si_class_type_info record at 0x02112c24 points directly at dActor_c, and
 * the 31-slot vtable at 0x02112c60 has the same extent as that base's table: only
 * the slots declared below hold this class's own words, so every other slot is
 * inherited and is deliberately not redeclared here.
 */
struct daObjC1Peach_c : dActor_c {
    u32 unk_0d0;                 /* 0x0d0 */
    Model mModel;                /* 0x0d4 */
    u8 mOpacity;                 /* 0x124 -- 0 hides the painting, 0xff is fully opaque */
    u8 pad_125[3];

    /* Inline, and declared FIRST. This TU defines every virtual the class has,
     * so it emits the vtable and RTTI no matter what is declared first -- moving
     * the declaration last buys nothing. Out of line, mwccarm emits the D2/D1/D0
     * triple in D0-before-D1 order, but retail puts D1 (0x02111e10) BELOW D0
     * (0x02111e40), and objisolate then refuses the whole TU for emitting out of
     * ROM address order. The inline body emits only the retail D1/D0 pair, in
     * retail order, and emits no D2. */
    virtual ~daObjC1Peach_c() {}   /* slots 16 (D1), 17 (D0) */

    virtual int InitResources();    /* slot  0 */
    virtual int CleanupResources(); /* slot  3 */
    virtual int Behavior();         /* slot  6 */
    virtual int Render();           /* slot  9 */

    /* Nonvirtual: no vtable slot holds 0x02111e84. Called once by InitResources.
     * Its body consumes this actor's angle, position and embedded Model, which
     * is what makes it a member of this class rather than a free function. */
    void UpdateModelTransform();
};

typedef char daObjC1Peach_c_size_must_be_0x128[
    sizeof(daObjC1Peach_c) == 0x128 ? 1 : -1];

#endif /* DAOBJC1PEACH_C_H */
