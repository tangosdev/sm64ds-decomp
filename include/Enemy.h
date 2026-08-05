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
    /* 0x094 is Actor.h:93 mPrevAngleY. The ROM loads it with a signed halfword
       load, which is the one thing no source pass can settle -- and 27 derived
       headers already declare it s16. Was a bare u8 marker. */
    s16 unk_094;            /* 0x094 */
    u8  pad_096[0xe];
    s32 unk_0a4;            /* 0x0a4 */
    /* 0x0a8 is Actor.h:99 mVertSpeed; 10 derived headers agree, both evidence
       passes see 4-byte accesses. */
    s32 unk_0a8;            /* 0x0a8 */
    /* 0x0ac: no Actor field, but 6 derived headers agree and both passes see
       4-byte accesses. The marker's span ran to 0x0d4; only its first word is
       evidenced, so the rest stays padding. */
    s32 unk_0ac;            /* 0x0ac */
    u8  pad_0b0[0x24];
    s32 unk_0d4;            /* 0x0d4 */
    s32 unk_0d8;            /* 0x0d8 */
    s32 unk_0dc;            /* 0x0dc */
    u8  unk_0e0;            /* 0x0e0 */
    u8  pad_0e1[0x7];
    u8  unk_0e8;            /* 0x0e8 */
    u8  pad_0e9[0x19];
    s16 unk_102;            /* 0x102 */
    u8  pad_104[0x2];
    u8  unk_106;            /* 0x106 */
    u8  unk_107;            /* 0x107 */
    u8  pad_108[0x4];
    /* 0x10c: 4 derived headers agree, history sees 4 four-byte accesses and the
       ROM two. This is the last field, so widening it grows sizeof(Enemy) by 3 --
       the only site here that is not offset-neutral. */
    s32 unk_10c;            /* 0x10c */
#ifdef __cplusplus
    /* methods */
    int AngleAwayFromWallOrCliff(WithMeshClsn & clsn_, short & outAngle_);
    int SpawnParticlesIfHitOtherObj(CylinderClsn & clsn_);
    int UpdateKillByInvincibleChar(WithMeshClsn & ww_, ModelAnim & mm_, unsigned int flags);
    void KillByInvincibleChar(const Vector3_16 & a1_, Player & a2_);
#endif
};

#endif
