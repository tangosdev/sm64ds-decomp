/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class dBgCh_SphCrr: 4 matched functions, 16 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef DBGCH_SPHCRR_H
#define DBGCH_SPHCRR_H
#include "types.h"
#include "dBgPi.h"

/* fwd */
struct dBgPi;
struct src_;
struct dBgCh_SphCrr {
    u8  pad_000[0x10];
    u8  unk_010;            /* 0x010 */
    u8  pad_011[0x27];
    u8  unk_038;            /* 0x038 */
    u8  pad_039[0x3];
    /* The query sphere. dBgW_KcMbg::DetectClsn(dBgCh_SphCrr&) hands 0x03c
       to func_02039e48, which transforms a Vector3 into the collider's local
       frame, and scales 0x048 by the collider's uniform scale before passing it
       as the `Fix12<int> radius` argument of dBgCh_SphCrr::SetObjAndSphere -- so
       0x03c is twelve bytes of centre and 0x048 is the radius that follows it. */
    Vector3 pos;            /* 0x03c */
    Fix12i radius;          /* 0x048 */
    u8  pad_04c[0x24];
    /* Result flags, read and OR-ed a bit at a time by the same function:
       1 = any hit, 4 = floor, 8 = wall, 0x10 = from underneath. Each bit gates
       copying the matching dBgPi below it. */
    u8  flags;              /* 0x070 */
    u8  pad_071[0x3];
    dBgPi mClsnResult1; /* 0x074 */
    dBgPi mClsnResult2; /* 0x09c */
    dBgPi mClsnResult3; /* 0x0c4 */
    s32 unk_0ec;            /* 0x0ec */
    u8  pad_0f0[0xc];
    /* An adjacent pair, kept unnamed. All that is evidenced is the shape of
       their use in dBgW_KcMbg::DetectClsn(dBgCh_SphCrr&): 0x100 is
       compared `<` against the local query's own 0x100, and on winning, 0x0fc
       is handed BY ADDRESS to func_0203794c -- i.e. a score at 0x100 selecting
       a payload that starts at 0x0fc. What the payload is is not settled here. */
    s32 unk_0fc;            /* 0x0fc */
    s32 unk_100;            /* 0x100 */
    u8  pad_104[0x4];
    s32 unk_108;            /* 0x108 */
#ifdef __cplusplus
    /* methods */
    void SetFloorResult(const dBgPi & src_);
#endif
};

#endif
