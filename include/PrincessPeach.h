/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class PrincessPeach: 5 matched functions, 6 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef PRINCESSPEACH_H
#define PRINCESSPEACH_H
#include "types.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

struct PrincessPeach {
    u8  pad_000[0xd4];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over mAnimation (+0x50 = the
       Animation base), which the header declared separately inside it. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x138 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN13PrincessPeachD0Ev.c] */
    ShadowModel mShadowModel;            /* 0x138 */
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x160 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN13PrincessPeachD0Ev.c] */
    dCcAc_c mdCcAc_c;            /* 0x160 */
    /* dBgCh_Actr member, named by the class's own destructor calling
       dBgCh_Actr's D1 at +0x194 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN13PrincessPeachD0Ev.c] */
    dBgCh_Actr mWithMeshClsn;            /* 0x194 */
    u8  pad_350[0x4];
    s32 mState;            /* 0x354 */
    /* Trailing remainder, 0x14 bytes. All three markers are typed and the last
       field the five recovered functions touch ends at 0x358;
       PrincessPeach_Spawn allocates 0x36c. The reference does not document
       this class's members. */
    u8  pad_358[0x14];
#ifdef __cplusplus
    /* methods */
    int Render();
    int CleanupResources();
    void OnPendingDestroy();
    int Behavior();
    int InitResources();
#endif
};

typedef char PrincessPeach_size_must_be_0x36c[sizeof(struct PrincessPeach) == 0x36c ? 1 : -1];

#endif
