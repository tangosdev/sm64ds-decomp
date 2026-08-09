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
    u8  pad_068[0x24];
    /* 0x08c-0x090 are Actor.h:91-93 mAngleX/Y/Z, the rotation triple.
       Enemy::UpdateKillByInvincibleChar adds the spin rates at 0x0ec-0x0f0 to
       these three in ascending order and hands the same three, in the same
       order, to Matrix4x3_ApplyInPlaceToRotationZXYExt -- consistent with
       Actor.h's assignment, which is the primary evidence. */
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    u8  pad_092[0x2];
    /* 0x094 is Actor.h:95 mPrevAngleY. The ROM loads it with a signed halfword
       load, which is the one thing no source pass can settle -- and 27 derived
       headers already declare it s16. Was a bare u8 marker. */
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0xe];
    s32 unk_0a4;            /* 0x0a4 */
    /* 0x0a8 is Actor.h:99 mVertSpeed; 10 derived headers agree, both evidence
       passes see 4-byte accesses. */
    s32 mVertSpeed;            /* 0x0a8 */
    /* 0x0ac: no Actor field, but 6 derived headers agree and both passes see
       4-byte accesses. The marker's span ran to 0x0d4; only its first word is
       evidenced, so the rest stays padding. */
    s32 unk_0ac;            /* 0x0ac */
    u8  pad_0b0[0x24];
    /* Two surface normals, written a Vector3 at a time by
       SurfaceInfo::CopyNormalTo in Enemy::UpdateWMClsn -- the floor result's
       normal into 0x0d4, the wall result's into 0x0e0. The wall triple was
       declared u8 + pad + u8, which made any natural access an ldrb;
       Enemy::AngleAwayFromWallOrCliff reads X and Z of it as four-byte values
       and hands them to Actor::ReflectAngle, which is what a wall normal is
       for. Same shape and same names as Player.h's mFloorNormalX/Y/Z. */
    s32 mFloorNormalX;            /* 0x0d4 */
    s32 mFloorNormalY;            /* 0x0d8 */
    s32 mFloorNormalZ;            /* 0x0dc */
    s32 mWallNormalX;            /* 0x0e0 */
    s32 mWallNormalY;            /* 0x0e4 */
    s32 mWallNormalZ;            /* 0x0e8 */
    /* Per-frame spin applied while the enemy is dying. KillByInvincibleChar
       (ov002 0x020ada40) copies the three s16 of its `const Vector3_16 &`
       argument straight into these slots; UpdateKillByInvincibleChar then adds
       them to mAngleX/Y/Z once a frame. Named from that behaviour -- the ROM
       evidences the widths and the copy, not the intent. */
    s16 mSpinRateX;            /* 0x0ec */
    s16 mSpinRateY;            /* 0x0ee */
    s16 mSpinRateZ;            /* 0x0f0 */
    u8  pad_0f2[0x10];
    /* Death timer, counted down a frame at a time. Was s16: every read in the
       tree is unsigned -- two ldrh in UpdateKillByInvincibleChar, and
       UpdateDeath's DecIfAbove0_Short is `unsigned short*` in both its source
       and its ROM bytes (ldrh at 0x0203adbc). No reference reads it signed,
       so the declared sign was wrong rather than locally over-ridden. */
    u16 mDeathTimer;            /* 0x102 */
    u8  pad_104[0x2];
    u8  unk_106;            /* 0x106 */
    u8  unk_107;            /* 0x107 */
    u8  pad_108[0x4];
    /* 0x10c: 4 derived headers agree, history sees 4 four-byte accesses and the
       ROM two. This is the last field, so widening it grows sizeof(Enemy) by 3 --
       the only site here that is not offset-neutral. */
    s32 mDeathState;            /* 0x10c */
#ifdef __cplusplus
    /* methods */
    int AngleAwayFromWallOrCliff(WithMeshClsn & clsn_, short & outAngle_);
    int UpdateDeath(WithMeshClsn & clsn_);
    void UpdateWMClsn(WithMeshClsn & clsn_, unsigned int sel);
    /* Already a real method -- its own file builds _ZN5Enemy9SpawnCoinEv from a
       local `struct Enemy : Actor` shadow. Declared here so callers need not
       spell the mangled name. */
    void SpawnCoin();
    int SpawnParticlesIfHitOtherObj(CylinderClsn & clsn_);
    int UpdateKillByInvincibleChar(WithMeshClsn & ww_, ModelAnim & mm_, unsigned int flags);
    /* PROVISIONAL SIGNATURE -- do not migrate a caller against it yet. Two
       separate problems, both caller-side:

       (1) This name is currently attached to ov004 0x020ada40, which cannot be
           this method (it reads only r0 and range-checks it as a scalar). The
           code that IS this method is ov002 0x020ada40, still carrying the
           placeholder name func_ov002_020ada40. See notes/overlay-residency.md,
           which independently resolves this address to ov002.

       (2) The arity below is refuted by three call sites, which materialise a
           FOURTH argument in r3 with three different Fix12 constants --
           ov062 0x02117cf0 (0x46000), ov084 0x02129f94 (0x41000) and
           ov084 0x02129fcc (0x96000) -- with no other consumer before the bl.
           The ov002 callee ignores r3, so the trailing parameter is accepted
           and dropped. A two-reference prototype cannot emit those moves. */
    void KillByInvincibleChar(const Vector3_16 & a1_, Player & a2_);
#endif
};

#endif
