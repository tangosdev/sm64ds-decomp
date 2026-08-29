/* class dScMgJump_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgJump_c : dScMgD3DBase_c, single edge, offset 0 (build/rtti.json).
 * Its own vtable is ov006:0x0213cbe4.
 *
 * SIZE 0x5834, from MgBounceAndPounce_Spawn's own
 * `_ZN7fBase_cnwEj(0x5834)`. THAT FACTORY IS THIS CLASS'S, despite its
 * name: it writes dScMgD3DBase_c's vtable, then THIS class's, and
 * everything it builds after that derived vptr store belongs here. The
 * English name is kept because it is already attributed, the same
 * convention as MgPicturePoker_Spawn for dScMgCard_c -- but note that until
 * the base was renamed, mModel and both arrays below were declared on
 * dScMgD3DBase_c itself and 0x5834 was asserted as the BASE's size. They
 * are this class's.
 *
 * THE LAST MEMBER CLOSES EXACTLY ON THE ALLOCATION LITERAL:
 * 0x5294 + 6 * 0xf0 = 0x5834. Nothing is left over.
 *
 * mModel IS A TYPED Model, and here that works. The factory constructs it
 * (`_ZN5ModelC1Ev(p + 0x501c)`), the destructor destroys it, and
 * sizeof(Model) == 0x50 closes exactly on mPlayers at 0x506c. Because mModel
 * is declared BEFORE the two arrays, reverse-declaration order destroys
 * mArray2, mPlayers, then mModel -- exactly the ROM's order, so the compiler
 * emits the Model call and the body does not. Contrast
 * include/dScMgJump2_c.h, whose ROM destructor destroys its Model FIRST and
 * therefore cannot use a typed member at all. Same class, same member, two
 * different answers, decided by the ROM's own ordering.
 *
 * 0x5004 IS A PAIR OF s32, not padding: src/func_ov006_020ee2c4.c assigns
 * `*(P*)(c + 0x5004) = data_ov006_0213cb7c` through its own local
 * `typedef struct { int a, b; } P`. That write, and the s16 at 0x5014 in
 * the same function, are what bound dScMgD3DBase_c at 0x5004 from above.
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf. No separate operator
 * delete is needed: dScMgD3DBase_c, the immediate base, provides one. */
#ifndef DSCMGJUMP_C_H
#define DSCMGJUMP_C_H
#include "dScMgD3DBase_c.h"
#include "dMgJump3DMario_c.h"
#include "Model.h"

extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);
extern "C" void func_ov006_020c6f3c(void);

struct dScMgJump_c : dScMgD3DBase_c {
    virtual ~dScMgJump_c();

    /* This class's own overrides, read off the ROM's vtable: the slots where the
       table differs from dScMgD3DBase_c's. Spelled WITHOUT the `virtual` keyword,
       the way include/daObjMarioCap_c.h and include/daObjRc_Dorifu_c.h spell
       theirs -- an override of a virtual an ancestor already declares is
       implicitly virtual either way, so each reuses an existing slot and adds no
       field, and the 0x5834 assert below still holds. The destructor above is
       declared first and out of line, so it stays this class's KEY FUNCTION and
       none of these translation units emits _ZTV11dScMgJump_c. */
    s32 InitResources();    /* slot  0 -- src/_ZN11dScMgJump_c13InitResourcesEv.cpp */
    s32 CleanupResources(); /* slot  3 -- src/_ZN11dScMgJump_c16CleanupResourcesEv.cpp */
    s32 Behavior();         /* slot  6 -- src/_ZN11dScMgJump_c8BehaviorEv.cpp */
    s32 Render();           /* slot  9 -- src/_ZN11dScMgJump_c6RenderEv.cpp */

    s32   unk_5004;        /* 0x5004 -- written as an { int, int } pair */
    s32   unk_5008;        /* 0x5008 */
    u8    pad_500c[0x8];   /* 0x500c */
    s16   unk_5014;        /* 0x5014 */
    u8    pad_5016[0x6];   /* 0x5016 */
    Model mModel;          /* 0x501c -- 0x50, destroyed last, see banner */
    dMgJump3DMario_c mPlayers[3]; /* 0x506c -- RTTI-proven element type */
    u8    mArray2[0x5a0];  /* 0x5294 -- 6 * 0xf0, elem dtor func_ov006_020c6f3c */
};

typedef char dScMgJump_c_size_must_be_0x5834[sizeof(dScMgJump_c) == 0x5834 ? 1 : -1];

#endif
