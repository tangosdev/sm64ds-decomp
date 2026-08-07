#ifndef OAMATTR_H
#define OAMATTR_H

#include "types.h"

/* One hardware OAM entry, as the DS lays it out: four 16-bit attribute words.
 *
 * This header is new. The layout is not invented -- it is the definition the two
 * `OAM::RenderSub` overloads each carried locally and independently agreed on.
 *
 * THE NAME IS `OamAttr`, AND BOTH THOSE FILES HAD IT WRONG. They declared
 * `struct OamAttri`. The mangled names read `P7OamAttrii` and `P7OamAttriiii`:
 * `7` is Itanium's length prefix, so the class name is the SEVEN characters
 * `OamAttr` and every remaining `i` is an `int` parameter. That error was
 * invisible for as long as the symbols were hand-spelled -- nothing derived the
 * name from the declaration. The first compile that mangles it emits
 * `P8OamAttriii` instead and the function disappears from the object. Migration
 * is what surfaced it.
 *
 * Nothing in the tree declared this type centrally, so those two files each
 * shadowed it -- the debt notes/plan-cpp-language-mode.md Phase 4 exists to
 * retire, here retired by giving it somewhere to point.
 *
 * NINETEEN OTHER FILES still declare `struct OamAttri` locally with the same wrong
 * name. They are harmless today, because each hand-spells its own symbol and so
 * never asks the compiler to mangle the type -- but every one of them will fail to
 * emit its function the first time it is migrated, in exactly the way described
 * above. Point them here when their slice comes up; do not rename the struct in
 * place and leave it local, which just moves the shadow.
 *
 * attr3 is the fourth word, which the hardware reads as an affine parameter when
 * the entry belongs to a rotation/scaling group rather than as a fourth OBJ
 * attribute -- which is why it has no descriptive name yet. */
struct OamAttr {
    u16 attr0;
    u16 attr1;
    u16 attr2;
    u16 attr3;
};

#endif /* OAMATTR_H */
