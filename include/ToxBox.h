/* Started life AUTO-GENERATED from matched-function evidence by
 * tools/gen_header.py; the field names below have since been recovered from
 * the bodies. Offsets/widths are observed, not guessed. Gaps are explicit
 * padding. Renaming cannot change codegen.
 *
 * The rolling crush box. mParam's low two bits pick how it moves: 0..2 index
 * data_ov092_02132294 for a canned direction sequence, 3 makes it follow the
 * path whose id is in mParam bits 8..11. That choice is mMoveKind, and it is
 * what src/func_ov092_021314d0.c and src/func_ov092_021313b0.cpp branch on to
 * choose the next mMoveDir.
 *
 * 0x008, 0x05c..0x064 and 0x08c..0x090 ARE fBase_c's and dActor_c's OWN
 * LAYOUT, not this class's, and were already named from include/dActor_c.h by
 * offset.
 *
 * Provenance table: notes/butterfly-tornado-provenance.md. */
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
    /* The player who set this box moving. src/func_ov092_021319b0.cpp resolves
       it from the collision's own id at 0x50c, keeps it only if its actorID is
       0xbf, and src/func_ov092_021311b0.cpp uses its position as the epicentre
       of the landing earthquake before clearing this back to 0. A dActor_c*,
       stored through an int. */
    s32 mPlayerActor;            /* 0x320 */
    /* dBgCh_Actr member. The cartridge's own ~ToxBox calls _ZN10dBgCh_ActrD1Ev at
       +0x324 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgCh_Actr mWithMeshClsn;            /* 0x324 */
    u8  pad_4e0[0x8];
    /* dCcAcPos_c member. The cartridge's own ~ToxBox calls _ZN10dCcAcPos_cD1Ev at
       +0x4e8 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dCcAcPos_c mdCcAcPos_c;            /* 0x4e8 */
    /* 0x528..0x557 is ONE Matrix4x3, written whole: InitResources copies
       mModel's own matrix at 0xf0 into it, and src/func_ov092_02131aec.cpp
       copies it back out again -- so it is the box's untumbled base transform,
       kept while the render matrix is rebuilt each frame. Four separate
       members rather than a typed one because that is the spelling the bytes
       reproduce. */
    u8  mBaseMtx;                /* 0x528 -- first byte of that matrix */
    u8  pad_529[0x2f];
    s32 mRestPosX;               /* 0x558 -- where the box rests. InitResources
                                     copies mPos here after lifting mPosY by
                                     0xfa000; src/func_ov092_021311b0.cpp
                                     refreshes mRestPosY from mPosY every time
                                     the box lands. */
    s32 mRestPosY;               /* 0x55c */
    s32 mRestPosZ;               /* 0x560 */
    u8  pad_564[0x4];
    s32 mMoveDir;                /* 0x568 -- which way the box rolls next.
                                     src/func_ov092_021314d0.c reads it out of
                                     mMoveSeq for a canned pattern;
                                     src/func_ov092_021313b0.cpp derives it
                                     from the horizontal angle to the next path
                                     node (2/3/4/5 for the four quadrants, 1
                                     when the node did not move); and
                                     src/func_ov092_021311b0.cpp overrides it
                                     with 7 or 8 for the two special floor
                                     types it lands on. */
    s32 mMoveSeq;                /* 0x56c -- pointer to this box's canned
                                     direction sequence,
                                     ((int **)&data_ov092_02132294)[mMoveKind].
                                     A 0 entry ends the sequence and
                                     src/func_ov092_021314d0.c wraps back to
                                     the first. */
    s32 mMoveSeqIndex;           /* 0x570 -- how far into mMoveSeq the box is */
    u8  mMoveKind;               /* 0x574 -- mParam & 3. 0..2 pick a canned
                                     sequence, 3 means follow a path. */
    u8  mOrientBits;             /* 0x575 -- the box's discrete orientation,
                                     three 2-bit fields packed out of the three
                                     rotation angles (X >> 0xe, Y >> 0xc,
                                     Z >> 0xa). InitResources builds it from
                                     the spawn rotation and
                                     src/func_ov092_021314d0.c rebuilds it
                                     after every roll;
                                     src/func_ov092_021316d8.c and
                                     src/func_ov092_02131878.c read it back to
                                     tell which face is down. */
    u8  pad_576[0x2];
    s32 mPathNodeCount;          /* 0x578 -- PathPtr::NumNodes for mPathPtr */
    s32 mPathNodeIndex;          /* 0x57c -- the node the box is heading for;
                                     src/func_ov092_021313b0.cpp advances it
                                     and wraps at mPathNodeCount */
    /* The Vector3 PathPtr::GetNode fills in for mPathNodeIndex.
       src/func_ov092_021313b0.cpp keeps the previous one on the stack and
       takes the horizontal angle between the two to pick mMoveDir. */
    s32 mPathNodeX;              /* 0x580 */
    s32 mPathNodeY;              /* 0x584 */
    s32 mPathNodeZ;              /* 0x588 */
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
