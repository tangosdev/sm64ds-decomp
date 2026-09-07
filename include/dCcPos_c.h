#ifndef DCCPOS_C_H
#define DCCPOS_C_H

#include "types.h"
#include "dCc_c.h"

/* A cylinder that carries its own position, vtable _ZTV8dCcPos_c
 * at 0x0208e6bc.
 *
 * VTABLE, 4 slots, read out of the ROM:
 *
 *   slot 0  0x02014854  ~dCcPos_c (D1)
 *   slot 1  0x02014828  ~dCcPos_c (D0)
 *   slot 2  0x02014820  GetPos()      - returns this->pos
 *   slot 3  0x02014818  GetOwnerID()  - `mov r0,#0; bx lr'
 *
 * Derives from dCc_c: its typeinfo at 0x0208e68c is the 12-byte
 * __si_class_type_info kind whose base pointer resolves to dCc_c, and the
 * ROM's own name for this class is dCcPos_c. C1 confirms it from the other
 * side -- it calls dCc_c's C2 and then stores this vtable.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND D1 IS A REAL METHOD -- see
 * include/ModelBase.h for the key-function rule and the objisolate exemption
 * to it. D0 and D2 stay C files.
 *
 * LAYOUT: the base is 0x30, and pos starts at exactly 0x30 (GetPos is
 * `add r0, r0, #0x30'), so the object is 0x3c.
 *
 * Having no owning dActor_c is what slot 3 encodes: GetOwnerID is a constant 0,
 * where dCcAc_c's reads through its owner pointer.
 */

#ifdef __cplusplus

#include "math/Fix12.h"

struct dCcPos_c : dCc_c {
    Vector3 pos;            /* 0x30 */

    /* --- vtable, in ROM order. Do not reorder. --- */
#ifndef _MSC_VER
    virtual ~dCcPos_c();     /* slots 0 (D1), 1 (D0) */
#else
    /* THE DESTRUCTOR PAIR AS TWO PLAIN VIRTUALS, HOST ONLY.

       mwccarm gives `virtual ~dCcPos_c()` TWO vtable entries -- D1 complete and
       D0 deleting, the Itanium pair this header's own slot map reads out of
       the ROM at slots 0 and 1. MSVC folds them into ONE. Spelt as a
       destructor the declaration indexes correctly on the ARM and ONE SLOT
       EARLY on the host: every virtual declared after it moves down a slot,
       so host-compiled dispatch through this class reaches the wrong ROM
       slot. It is not theoretical -- `dCcPos_c::Process` came out as
       `call [eax+4]` for GetPos (ROM slot 1, the DELETING DESTRUCTOR) and
       `call [eax+8]` for GetOwnerID (ROM slot 2, GetPos), and the matched
       symbol had to be switched off behind a host transcription. Two
       ordinary virtuals occupy the SAME two entries under MSVC, so the host
       lands where the ARM does. Neither is ever called by name; they hold
       the two slots the ROM's table holds, and the SAME TWO NAMES must be
       used in every derived class or MSVC appends a slot instead of
       overriding.

       INVISIBLE TO THE ROM BUILD, and that is measured, not assumed: the
       pinned compiler (mwccarm 2004/b56) does not define _MSC_VER -- a
       translation unit whose whole body is `#ifdef _MSC_VER  #error ...
       #endif` compiles clean under the pinned flags -- so the preprocessor
       never enters this arm and the token stream mwccarm sees is unchanged.
       Verified per translation unit: `mwccarm -E` output is byte-identical
       before and after this commit for every source that includes this
       header. */
    virtual void Destructor1();  /* slot 0 (D1) */
    virtual void Destructor0();  /* slot 1 (D0) */
#endif
    virtual Vector3 &GetPos();          /* slot 2 */
    virtual u32 GetOwnerID();           /* slot 3 - always 0 */

    /* DECLARED, defined out of line in src/_ZN8dCcPos_cC1Ev.cpp as real C++
     * -- the body is empty; the base step and the vptr store are the whole
     * ROM function. */
    dCcPos_c();

    /* --- non-virtual --- */
    void Init(const Vector3 &pos, Fix12<int> radius, Fix12<int> height, u32 flags, u32 vulnFlags);
};

typedef char dCcPos_c_size_must_be_0x3c[
    sizeof(dCcPos_c) == 0x3c ? 1 : -1];

#else

struct dCcPos_c {
    struct dCc_c base; /* 0x00 */
    Vector3 pos;              /* 0x30 */
};

#endif /* __cplusplus */

#endif /* DCCPOS_C_H */
