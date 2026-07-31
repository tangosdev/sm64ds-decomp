/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Enemy: 10 matched functions, 16 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ENEMY_H
#define ENEMY_H
#include "types.h"

/* fwd */
struct CylinderClsn;
struct ModelAnim;
struct Player;
struct Vector3_16;
struct WithMeshClsn;
struct a1_;
struct a2_;
struct clsn_;
struct flags;
struct mm_;
struct outAngle_;
struct ww_;
struct Enemy {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x2c];
    u8  unk_094;            /* 0x094 */
    u8  pad_095[0xf];
    u8  unk_0a4;            /* 0x0a4 */
    u8  pad_0a5[0x3];
    u8  unk_0a8;            /* 0x0a8 */
    u8  pad_0a9[0x3];
    u8  unk_0ac;            /* 0x0ac */
    u8  pad_0ad[0x27];
    u8  unk_0d4;            /* 0x0d4 */
    u8  pad_0d5[0x3];
    u8  unk_0d8;            /* 0x0d8 */
    u8  pad_0d9[0x3];
    u8  unk_0dc;            /* 0x0dc */
    u8  pad_0dd[0x3];
    u8  unk_0e0;            /* 0x0e0 */
    u8  pad_0e1[0x7];
    u8  unk_0e8;            /* 0x0e8 */
    u8  pad_0e9[0x19];
    u8  unk_102;            /* 0x102 */
    u8  pad_103[0x3];
    u8  unk_106;            /* 0x106 */
    u8  unk_107;            /* 0x107 */
    u8  pad_108[0x4];
    u8  unk_10c;            /* 0x10c */
#ifdef __cplusplus
    /* methods */
    int AngleAwayFromWallOrCliff(WithMeshClsn & clsn_, short & outAngle_);
    int SpawnParticlesIfHitOtherObj(CylinderClsn & clsn_);
    int UpdateKillByInvincibleChar(WithMeshClsn & ww_, ModelAnim & mm_, unsigned int flags);
    void KillByInvincibleChar(const Vector3_16 & a1_, Player & a2_);
#endif
};

#endif
