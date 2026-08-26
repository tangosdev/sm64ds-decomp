/* Reconciled from matched-function, vtable, RTTI, and constructor evidence.
 * class Tree: 7 matched methods plus its matched spawn function.
 * Offsets/widths are observed, not guessed. Field NAMES are placeholders -
 * renaming cannot change codegen.
 *
 * Real inheritance (was a flat, non-inheriting struct re-declaring
 * fBase_c's mParam and dActor_c's mPosX/mPosY/mPosZ inline): build/
 * rtti_vtables.json keys this class as "daTree_c" (8 chars) vs the decomp
 * name "Tree" (4 chars) -- a length mismatch, not data-verifiable. parent:
 * dActor_c, parent_slots: 31, own overrides at slots 0/3/6/9/12
 * (InitResources/CleanupResources/Behavior/Render/OnPendingDestroy) plus
 * the destructor pair at 16/17.
 *
 * All seven functions are compiler-spelled Tree methods. InitResources keeps
 * one low-level extern-C declaration for dCcPos_c::Init: its ROM name carries
 * by-value Fix12 parameters whose honest C++ declaration changes the call ABI.
 * That exception is about the callee signature, not Tree method ownership.
 *
 * The destructor is declared first and is Tree's key function. A destructor
 * source therefore emits D0/D1/D2 plus a complete 0x84-byte `_ZTV4Tree` object
 * and RTTI. The per-function build keeps only the licensed destructor variant
 * and rebinds its vptr store to the ROM-owned table. Raw metadata inspection is
 * still required: the compiler calls this class `Tree` (`_ZTI4Tree`), while the
 * cartridge RTTI string is `daTree_c` (`_ZTI8daTree_c`). The 31-slot shape and
 * every slot destination conform, but the differently named RTTI means that
 * compiler-emitted data must remain isolated rather than replacing the ROM
 * bytes. This is also why the shadow TU is evidence, not a production-TU
 * promotion candidate yet.
 */
#ifndef TREE_H
#define TREE_H
#include "types.h"
#include "dActor_c.h"
#include "Model.h"

struct Tree : dActor_c {
    u8  pad_0d0[0x4];
    /* Model[5] array, named by _ZN5ModelD1Ev/_ZN5ModelC1Ev at +0xd4 via
       __destroy_arr/func_020733a8 (a relocation the ROM build checks) --
       five Models, one per compass-ish direction slot indexed by
       InitResources/Render. Fills the class to its exact trailing size,
       0xd0 (dActor_c) + 0x4 (pad) + 5*0x50 (Model) = 0x264. */
    Model mModel[5];                  /* 0x0d4 */

    virtual ~Tree();
    virtual s32 InitResources();       /* slot  0 */
    virtual s32 CleanupResources();    /* slot  3 */
    virtual s32 Behavior();            /* slot  6 */
    virtual s32 Render();              /* slot  9 */
    virtual void OnPendingDestroy();   /* slot 12 -- empty body in the ROM */
};

typedef char Tree_size_must_be_0x264[sizeof(Tree) == 0x264 ? 1 : -1];

#endif
