/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SwitchActivatedPlank: 6 matched functions, 8 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SWITCHACTIVATEDPLANK_H
#define SWITCHACTIVATEDPLANK_H
#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

struct SwitchActivatedPlank {
    u8  pad_000[0x8e];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN20SwitchActivatedPlankD1Ev.c] */
    Model mModel1;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~SwitchActivatedPlank calls
       _ZN10dBgW_KcMbgD1Ev at +0x124 (D0/D1), a relocation the ROM build checks;
       recovered by tools/dtor_members.py. D1 and not D2, so it is this type and not an
       inlined base. */
    dBgW_KcMbg mMovingMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    /* Model member, named by _ZN5ModelD1Ev at +0x320 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. The marker's pad ran 0x30
       bytes PAST the end of the object; that space is not evidenced and stays explicit
       padding rather than being folded into the member. */
    Model mModel2;            /* 0x320 */
    u8  pad_370[0x30];
    s16 mStateTimer;            /* 0x3a0 */
    u8  mState;            /* 0x3a2 */
    u8  mVisible;            /* 0x3a3 */
    u8  mEventID;            /* 0x3a4 */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

typedef char SwitchActivatedPlank_size_must_be_0x3a8[
    sizeof(struct SwitchActivatedPlank) == 0x3a8 ? 1 : -1];

#endif
