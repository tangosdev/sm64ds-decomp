/* class dScMgMemory2_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgMemory2_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). The THIRTEENTH and last direct child of that base;
 * with this file the family is complete.
 *
 * SAME COINED-NAME STORY AS ITS SIBLING -- see include/dScMgMemory_c.h's
 * own banner for the full version. Short form: this class shipped as
 * `_ZN14MgMemoryMasterD1Ev`/`D0Ev` and `_ZTV14MgMemoryMaster`, and the ROM
 * disagrees. The vptr value is ov006:0x0213d4d4; the RTTI pointer one word
 * below it is `_ZTI14dScMgMemory2_c`, whose `_ZTS` string reads
 * "14dScMgMemory2_c". The factory keeps `MgMemoryMaster_Spawn`, already
 * attributed. "MgMemoryMaster" and "dScMgMemory2_c" are both 14 characters,
 * so the rename is length-neutral and no mangled prefix changes.
 *
 * SIZE 0x5410, from MgMemoryMaster_Spawn's own `_ZN7fBase_cnwEj(0x5410)`.
 *
 * SHARED TABLE at 0x4f38, size 0x270 (func_ov006_020c1d80 /
 * func_ov006_020c1c64), the same one four siblings use. As with
 * dScMgMemory_c, none of this class's own six recovered methods reads
 * inside it, so it stays entirely opaque here rather than borrowing a
 * sibling's field names for bytes this class never touches.
 *
 * 0x51a8..0x53d4 HAS NO MATCHED ACCESS IN ANY OF THIS CLASS'S OWN METHODS,
 * checked for split-literal forms as well as whole constants. That is a
 * statement about the six recovered files, not a proof about the ROM.
 *
 * OWN TAIL: three fields carried over verbatim from this header's previous
 * auto-generated form, unchanged offsets and widths. The three fields the
 * old header declared below 0x4f38 are dScMgBase_c's own.
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf, no RTTI descendants of
 * its own. Defined for real in src/_ZN14dScMgMemory2_cD1Ev.cpp; D0Ev.cpp
 * carries an identical copy. No separate operator delete is needed --
 * dScMgBase_c, two levels up, already provides one. */
#ifndef DSCMGMEMORY2_C_H
#define DSCMGMEMORY2_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int func_ov006_020c1c64(char *t); /* decl_common.h's own signature */

struct dScMgMemory2_c : dScMgSingle3DBase_c {
    virtual ~dScMgMemory2_c();

    /* This class's own overrides, read off the ROM's vtable: the slots where the
       table differs from dScMgSingle3DBase_c's. Spelled WITHOUT the `virtual`
       keyword, the way include/daObjMarioCap_c.h and include/daObjRc_Dorifu_c.h
       spell theirs -- an override of a virtual an ancestor already declares is
       implicitly virtual either way, so each reuses an existing slot and adds no
       field, and the 0x5410 assert below still holds. The destructor above is
       declared first and out of line, so it stays this class's KEY FUNCTION and
       none of these translation units emits _ZTV14dScMgMemory2_c. */
    s32 InitResources();   /* slot  0 -- src/_ZN14dScMgMemory2_c13InitResourcesEv.cpp */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual int  OnTurnIntoEgg(int mode);              /* slot 19 */
    virtual void OnGroundPounded();                    /* slot 21 */
    s32 Behavior();        /* slot  6 -- src/_ZN14dScMgMemory2_c8BehaviorEv.cpp */
    s32 Render();          /* slot  9 -- src/_ZN14dScMgMemory2_c6RenderEv.cpp */

    u8  pad_4f38[0x270];   /* 0x4f38 -- shared table, opaque here; see banner */
    u8  pad_51a8[0x22c];   /* 0x51a8 -- no matched access, see banner */
    s32 unk_53d4;          /* 0x53d4 */
    s32 unk_53d8;          /* 0x53d8 */
    u8  pad_53dc[0x2d];    /* 0x53dc */
    u8  unk_5409;          /* 0x5409 */
    u8  pad_540a[0x6];     /* 0x540a -- rounds up to the 0x5410 boundary */
};

typedef char dScMgMemory2_c_size_must_be_0x5410[sizeof(dScMgMemory2_c) == 0x5410 ? 1 : -1];

#endif
