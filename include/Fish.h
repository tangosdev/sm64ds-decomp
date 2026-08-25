/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Fish: 6 matched functions, 11 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FISH_H
#define FISH_H
#include "types.h"
#include "ModelAnim.h"

struct Fish {
    u8  pad_000[0x4];
    s32 uniqueID;            /* 0x004 */
    u32 mParam;            /* 0x008 */
    u8  pad_00c[0x54];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x70];
    /* ModelAnim member. The cartridge's own ~Fish calls _ZN9ModelAnimD1Ev at +0x0d4
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    u8  pad_138[0x4];
    s32 unk_13c;            /* 0x13c */
    s32 unk_140;            /* 0x140 */
    u8  pad_144[0x8];
    s32 unk_14c;            /* 0x14c */
    s32 unk_150;            /* 0x150 */
    u8  pad_154[0x5];
    u8  unk_159;            /* 0x159 */
    u8  pad_15a[0x1];
    u8  mModelIndex;            /* 0x15b */
    u8  unk_15c;            /* 0x15c */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();
#endif
};

typedef char Fish_size_must_be_0x160[sizeof(struct Fish) == 0x160 ? 1 : -1];

#endif
