/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Number: 6 matched functions, 18 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen.
 *
 * Real inheritance (was a flat, non-inheriting struct re-declaring dActor_c's
 * own mParam/mPosX/mPosY/mPosZ/mVertAccel/mTerminalVelocity/mVertSpeed
 * inline): build/rtti_vtables.json keys this class as "daObjNumber_c" (13
 * chars) vs the decomp name "Number" (6 chars) -- a length mismatch, not
 * data-verifiable. parent: dActor_c, parent_slots: 31, own overrides at
 * slots 0/3/6/9 (InitResources/CleanupResources/Behavior/Render) plus the
 * destructor pair at 16/17 -- no OnPendingDestroy override.
 */
#ifndef NUMBER_H
#define NUMBER_H
#include "types.h"
#include "dActor_c.h"
#include "Model.h"
#include "TextureSequence.h"

struct Number : dActor_c {
    u8  pad_0d0[0x4];
    /* Model member. The cartridge's own ~Number calls _ZN5ModelD1Ev at +0x0d4 (D0/D1),
       a relocation the ROM build checks; recovered by tools/dtor_members.py. D1 and not
       D2, so it is this type and not an inlined base. */
    Model mModel;            /* 0x0d4 */
    /* TextureSequence member. The cartridge's own ~Number calls
       _ZN15TextureSequenceD1Ev at +0x124 (D0/D1), a relocation the ROM build checks;
       recovered by tools/dtor_members.py. D1 and not D2, so it is this type and not an
       inlined base. */
    TextureSequence mTextureSequence;            /* 0x124 */
    s32 unk_138;            /* 0x138 */
    s32 unk_13c;            /* 0x13c */
    s32 mStartPosY;            /* 0x140 */
    s32 unk_144;            /* 0x144 */
    s32 unk_148;            /* 0x148 */
    u16 unk_14c;            /* 0x14c */
    u8  unk_14e;            /* 0x14e */

    virtual ~Number();

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
};

typedef char Number_size_must_be_0x150[sizeof(Number) == 0x150 ? 1 : -1];

#endif
