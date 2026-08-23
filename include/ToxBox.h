/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ToxBox: 6 matched functions, 25 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef TOXBOX_H
#define TOXBOX_H
#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"
#include "dBgCh_Actr.h"
#include "dCcAcPos_c.h"

struct ToxBox {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    u8  pad_092[0x42];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. The marker's pad stopped
       short of the object, so the member also takes over unk_0f0 (+0x1c = mat4x3), which
       the header declared separately inside it. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~ToxBox calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    s32 unk_320;            /* 0x320 */
    /* dBgCh_Actr member. The cartridge's own ~ToxBox calls _ZN10dBgCh_ActrD1Ev at
       +0x324 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgCh_Actr mWithMeshClsn;            /* 0x324 */
    u8  pad_4e0[0x8];
    /* dCcAcPos_c member. The cartridge's own ~ToxBox calls _ZN10dCcAcPos_cD1Ev at
       +0x4e8 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dCcAcPos_c mdCcAcPos_c;            /* 0x4e8 */
    u8  unk_528;            /* 0x528 */
    u8  pad_529[0x2f];
    s32 unk_558;            /* 0x558 */
    s32 unk_55c;            /* 0x55c */
    s32 unk_560;            /* 0x560 */
    u8  pad_564[0x4];
    s32 unk_568;            /* 0x568 */
    s32 unk_56c;            /* 0x56c */
    s32 unk_570;            /* 0x570 */
    u8  unk_574;            /* 0x574 */
    u8  unk_575;            /* 0x575 */
    u8  pad_576[0x2];
    s32 unk_578;            /* 0x578 */
    s32 unk_57c;            /* 0x57c */
    u8  pad_580[0xc];
    u8  mPathPtr;            /* 0x58c */
    /* trailing extent the ROM's `new ToxBox` literal proves; see tools/opnew_sizes.py */
    u8 pad_590[0x4];
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

typedef char ToxBox_size_must_be_0x594[sizeof(struct ToxBox) == 0x594 ? 1 : -1];

#endif
