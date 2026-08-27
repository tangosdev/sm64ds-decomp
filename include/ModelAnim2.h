#ifndef MODELANIM2_H
#define MODELANIM2_H

#include "types.h"
#include "ModelAnim.h"
#include "Animation.h"

/* ModelAnim child that carries a SECOND Animation as a data member (not a
 * base): its C1 builds otherAnim with Animation::C1 after chaining
 * ModelAnim::C2. Vtable _ZTV10ModelAnim2 at 0x0208e9b4 overrides only the
 * destructor pair; every other slot still points at the ModelAnim or
 * Model implementation.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD -- see
 * include/ModelAnim.h.
 *
 * Func_020162C4 keeps its placeholder name, but its body is SetAnim for
 * the second animation, fast path and all; its definition stays a
 * mangled free function (wall 6az, Fix12<int> in the signature).
 */

#ifdef __cplusplus

extern "C" void _ZN6Memory16operator_delete2EPv(void *);

struct ModelAnim2 : ModelAnim {
    u32 otherFile;             /* 0x64 - the second animation's BCA, stored as a word */
    Animation otherAnim;       /* 0x68 - a member, built with Animation::C1 */

    /* --- vtable order. Do not reorder. --- */
    virtual ~ModelAnim2();                     /* slots 0 (D1), 1 (D0) */

    /* DECLARED, defined out of line in src/runtime/graphics/ModelAnim2/_ZN10ModelAnim2C1Ev.cpp as real
     * C++ -- complete-object context, hence C1. Init list `: otherFile(0)`
     * puts the scalar store BEFORE otherAnim's implicit construction, which
     * is the ROM's schedule; a body statement would come one call too late
     * (mem-initialisers run in declaration order, ahead of nothing here --
     * both stores are initialisation, not body). */
    ModelAnim2();

    /* --- non-virtual --- */
    void Copy(const ModelAnim2 &src, char *newFile, u32 newOtherFile);
    void Func_020162C4(u32 animFile, int flags, Fix12<int> speed,
                       u16 startFrame);        /* free function, wall 6az */

    /* ITS OWN, TO RESOLVE AN AMBIGUITY MULTIPLE INHERITANCE CREATES. ModelAnim
       derives from Model (so ModelBase) and from Animation, and both bases
       declare operator delete, so an inherited one is "ambiguous access to
       name found: ModelBase::operator delete and Animation::operator delete".
       Declaring it here picks the same deallocator both bases name, and also
       satisfies the rule in include/dActor_c.h that mwcc only inlines the member
       when it is in the class or its immediate base. */
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }

};

typedef char ModelAnim2_size_must_be_0x78[sizeof(ModelAnim2) == 0x78 ? 1 : -1];

#endif /* __cplusplus */

#endif /* MODELANIM2_H */
