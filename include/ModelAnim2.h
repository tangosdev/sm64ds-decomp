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

struct ModelAnim2 : ModelAnim {
    u32 otherFile;             /* 0x64 - the second animation's BCA, stored as a word */
    Animation otherAnim;       /* 0x68 - a member, built with Animation::C1 */

    /* --- vtable order. Do not reorder. --- */
    /* The destructor pair spelled as two plain virtuals on the host; the whole
       ruling is in include/ModelBase.h. Nothing is declared after it here, so
       this class has no skew of its own -- but an override must still carry
       the SAME TWO NAMES ModelAnim declares, or slots 0 and 1 of the primary
       table would be a fresh pair appended past Virtual18 instead. */
#ifdef _MSC_VER
    virtual void Destructor1();                /* slot 0 (D1) */
    virtual void Destructor0();                /* slot 1 (D0) */
#else
    virtual ~ModelAnim2();                     /* slots 0 (D1), 1 (D0) */
#endif

    /* --- non-virtual --- */
    void Copy(const ModelAnim2 &src, char *newFile, u32 newOtherFile);
    void Func_020162C4(u32 animFile, int flags, Fix12<int> speed,
                       u16 startFrame);        /* free function, wall 6az */
};

typedef char ModelAnim2_size_must_be_0x78[sizeof(ModelAnim2) == 0x78 ? 1 : -1];

#endif /* __cplusplus */

#endif /* MODELANIM2_H */
