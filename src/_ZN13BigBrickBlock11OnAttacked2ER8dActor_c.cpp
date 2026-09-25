//cpp
// @symbol _ZN13BigBrickBlock11OnAttacked2ER8dActor_c
#include "BigBrickBlock.h"
#include "dActor_c.h"

/* BigBrickBlock::OnAttacked2 -- vtable slot 23, ov002 0x020b3788.
 * include/dActor_c.h's own slot 23 supplies the signature, `virtual int
 * OnAttacked2(dActor_c &other)`.
 *
 * `actorID` and `other.param1` are fBase_c's own fields. Both branches call
 * this class's own Kill (slot 31) through an unqualified virtual call -- the
 * same indirect dispatch the pre-migration `(*(Vt**)c)->fn(c)` read
 * reproduced by hand.
 *
 * `int b = (actorID == 0x11);` materializes the bool the same way the
 * pre-migration recovery did -- load-bearing (cpp-bool-widening-cast). */
int BigBrickBlock::OnAttacked2(dActor_c &other)
{
    int b = (actorID == 0x11);
#ifdef _MSC_VER
    /* THE HOST SPELLS THE EARLY EXITS AS THE ABSENCE OF THE CALL. mwccarm
     * accepts a valueless `return` in an int function; MSVC refuses it
     * (C2561) and no option reaches it. The ROM sets no return value on any
     * path, so the faithful host shape is a body that reaches its closing
     * brace with nothing to return, the shape OnAttacked1 (slot 22) is seated
     * in; port/CMakeLists.txt puts /wd4716 on this source for it. Both Kill
     * calls stay where the ROM has them. Nothing here reaches mwccarm: it
     * builds the #else arm, byte-identical. */
    if (b) {
        if (other.param1 == 2) {
            Kill();
        }
    } else {
        Kill();
    }
#else
    if (b) {
        if (other.param1 != 2) return;
        Kill();
        return;
    }
    Kill();
#endif
}
