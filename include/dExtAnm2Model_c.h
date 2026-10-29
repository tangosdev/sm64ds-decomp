#ifndef DEXTANM2MODEL_C_H
#define DEXTANM2MODEL_C_H

#include "types.h"
#include "dExtAnmModel_c.h"
#include "dExtFrameCtrl_c.h"

/* dExtAnmModel_c child that carries a SECOND dExtFrameCtrl_c as a data member (not a
 * base): its C1 builds otherAnim with dExtFrameCtrl_c::C1 after chaining
 * dExtAnmModel_c::C2. Vtable _ZTV15dExtAnm2Model_c at 0x0208e9b4 overrides only the
 * destructor pair; every other slot still points at the dExtAnmModel_c or
 * dExtSimpleModel_c implementation.
 *
 * THE DESTRUCTOR IS DECLARED FIRST. It is defined out of line in
 * src/engine/model/d_ext_model.cpp, which makes that file the key function
 * and emits the MI vtable group with its RTTI; the retail table stays
 * externalized. See include/dExtAnmModel_c.h for why the declaration stays first.
 *
 * Func_020162C4 keeps its placeholder name, but its body is SetAnim for
 * the second animation, fast path and all. It is a real member: speed is
 * the scalar Fix12i, so the symbol is _ZN15dExtAnm2Model_c13Func_020162C4Ejiit,
 * as dExtAnmModel_c::SetAnim's is.
 */

#ifdef __cplusplus

namespace Memory {
void operator_delete2(void *);
}

struct dExtAnm2Model_c : dExtAnmModel_c {
    u32 otherFile;             /* 0x64 - the second animation's BCA, stored as a word */
    dExtFrameCtrl_c otherAnim;       /* 0x68 - a member, built with dExtFrameCtrl_c::C1 */

    /* --- vtable order. Do not reorder. --- */
    /* The destructor pair spelled as two plain virtuals on the host, plus the
       non-virtual destructor declaration the src/ definitions need; the whole
       ruling is in include/dExtModel_c.h. Nothing is declared after it here, so
       this class has no skew of its own -- but an override must still carry
       the SAME TWO NAMES its base declares, or slots 0 and 1 of the primary
       table would be a fresh pair appended past Virtual18 instead. */
#ifdef _MSC_VER
    virtual void Destructor1();                /* slot 0 (D1) */
    virtual void Destructor0();                /* slot 1 (D0) */
    ~dExtAnm2Model_c();                        /* no slot */
#else
    virtual ~dExtAnm2Model_c(); /* slots 0 (D1), 1 (D0) */
#endif

    /* DECLARED, defined out of line in src/engine/model/d_ext_model.cpp as real
     * C++ -- complete-object context, hence C1. Init list `: otherFile(0)`
     * puts the scalar store BEFORE otherAnim's implicit construction, which
     * is the ROM's schedule; a body statement would come one call too late
     * (mem-initialisers run in declaration order, ahead of nothing here --
     * both stores are initialisation, not body). */
    dExtAnm2Model_c();

    /* --- non-virtual --- */
    void Copy(const dExtAnm2Model_c &src, char *newFile, u32 newOtherFile);
    void Func_020162C4(u32 animFile, int flags, Fix12i speed, u16 startFrame);

    /* ITS OWN, TO RESOLVE AN AMBIGUITY MULTIPLE INHERITANCE CREATES. dExtAnmModel_c
       derives from dExtSimpleModel_c (so dExtModel_c) and from dExtFrameCtrl_c, and both bases
       declare operator delete, so an inherited one is "ambiguous access to
       name found: dExtModel_c::operator delete and dExtFrameCtrl_c::operator delete".
       Declaring it here picks the same deallocator both bases name, and also
       satisfies the rule in include/dActor_c.h that mwcc only inlines the member
       when it is in the class or its immediate base. */
    void operator delete(void *ptr) {
        Memory::operator_delete2(ptr);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dExtAnm2Model_c_size_must_be_0x78[sizeof(dExtAnm2Model_c) == 0x78 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DEXTANM2MODEL_C_H */
