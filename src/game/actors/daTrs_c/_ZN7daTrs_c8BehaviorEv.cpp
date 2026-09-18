//cpp
// @symbol _ZN7daTrs_c8BehaviorEv
/* daTrs_c::Behavior (slot 6): the chase. Re-syncs the 0xd3 follower, dies to
 * the invincible character (permanently when mDeathMode says so), hides while
 * its home area is off, runs the Yoshi-mouth/talk flow, measures the player,
 * dispatches the unk_5cf variant arm, integrates motion with a ground
 * raycast, and advances the animation and the collision.
 *
 * Codegen notes (all load-bearing, all measured):
 * - The 0x5xx fields are read through direct spellings so the compiler
 *   synthesizes the r+0x500 base itself; the 0x92 angle triple copies through
 *   an s16 array-pointer deref, and the show-hide block wants plain r2/fp
 *   pointer locals instead.
 * - `*(u16 *)&mStateTimer += 1` stays in cast form: the compound spelling
 *   colors the address/value pair the way the ROM has it.
 * - The closest-player copy stays memberwise through an s32*; the raycast
 *   y+0x32000 sum stays in its own local between the z and x loads.
 * - The mTalkStep/mFlags_5d4 RMWs stay through bp/fp pointer locals: 0x5d1
 *   and 0x5d4 are not ARM rotate-immediates, and the pointer form is what
 *   materializes the ROM's literal-pool addresses.
 *
 * deslop leftovers:
 * - dCapEnemy_c::GetCapState has no header declaration (shared-header
 *   addition is out of scope for this TU); the mangled call stays.
 * - mdCcAcPos_c.flags |= 1 stays a raw 0x19c RMW (same class as daBmb_c's
 *   load-bearing *(this+0x128)).
 * - func_0200f760 / func_0201267c / IsAreaShowing / Vec3_HorzAngle /
 *   Vec3_HorzDist / DecIfAbove0_Short have no header declarations; the
 *   TU-local externs stay. func_ov063_* are unowned state helpers.
 */
#include "daTrs_c.h"
#include "dBgCh_Gnd.h"
#include "Player.h"

extern "C" {
void func_0200f760(void *thiz, void *cyl);
int _ZN11dCapEnemy_c11GetCapStateEv(char *c);
void func_0201267c(u32 a, void *b);
u8 IsAreaShowing(s8 idx);
int func_ov063_02116190(char *c);
void func_ov063_02119ab0(char *c);
void func_ov063_021166ac(char *c);
s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
s32 Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
void func_ov063_021192d4(char *c);
void func_ov063_02116bf4(char *c);
void func_ov063_02116a1c(char *c);
void func_ov063_02116d38(char *c);
void func_ov063_0211ab68(char *c);
void func_ov063_021189f4(char *c);
void func_ov063_021172a8(char *c);
void func_ov063_02119274(char *c);
void func_ov063_02116fac(char *c);
u16 DecIfAbove0_Short(void *p);
}

int daTrs_c::Behavior()
{
    char *c = (char *)this;
    Vector3 pv;
    Vector3 v1;
    Vector3 v2;
    Vector3 ve;
    int t;
    void *p;
    char *q;
    char *r1;
    char *r2;
    s32 *p19c;
    u16 *fp;
    u8 *bp;
    s32 y;
    s32 w;
    s32 z;
    s32 x;
    int d1;
    int v;

    func_0200f760(c, &mdCcAcPos_c);
    if (mSpawnedActorID != 0) {
        mFoundActor = dActor_c::FindWithID(mSpawnedActorID);
        q = (char *)mFoundActor;
        if (q != 0) {
            *(s32 *)(q + 0x5c) = mPosX;
            *(s32 *)(q + 0x60) = mPosY;
            *(s32 *)(q + 0x64) = mPosZ;
        }
        mFoundActor = 0;
    }
    if (_ZN11dCapEnemy_c11GetCapStateEv(c) == 0)
        return 1;

    t = UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim, 0);
    if (t != 0) {
        if (t == 2) {
            PoofDust();
            if (mDeathMode != 0) {
                KillAndTrackInDeathTable();
                func_0201267c(0xd5, &mCamSpacePosX);
                if (((mCapId) & 0xf) < 6) {
                    mPosX = mHomePosX;
                    mPosY = mHomePosY;
                    mPosZ = mHomePosZ;
                    mAreaId = mAreaIdx;
                    mPrevAngleX = mHomeAngleX;
                    mPrevAngleY = mHomeAngleY;
                    mPrevAngleZ = mHomeAngleZ;
                    {
                        s16 (*ap)[3] = (s16 (*)[3])(c + 0x92);
                        mAngleX = (*ap)[0];
                        mAngleY = (*ap)[1];
                        mAngleZ = (*ap)[2];
                    }
                    p = RespawnIfHasCap();
                    if (p != 0) {
                        fp = (u16 *)((char *)p + 0x5d4);
                        *fp = (*fp) & (~2);
                    }
                }
            } else {
                unk_5cc = 5;
                mSubState = 0;
                p19c = (s32 *)(c + 0x19c);
                *p19c = (*p19c) | 1;
                fp = (u16 *)(c + 0x5d4);
                *fp = (*fp) & (~8);
            }
        }
        return 1;
    }

    mAreaId = -1;
    if (mSoundCount == 0) {
        /* b7 clear: not hidden yet. */
        if ((IsAreaShowing(mAreaIdx) == 0) || (func_ov063_02116190(c) != 0)) {
            if (mFlags_5d4.b7 == 0) {
                r2 = (char *)&mPrevAngleX;
                mPosX = mHomePosX;
                fp = (u16 *)(c + 0x5d4);
                mPosY = mHomePosY;
                mPosZ = mHomePosZ;
                mPrevAngleX = mHomeAngleX;
                mPrevAngleY = mHomeAngleY;
                mPrevAngleZ = mHomeAngleZ;
                mAngleX = *(s16 *)r2;
                mAngleY = *(s16 *)(r2 + 2);
                mAngleZ = *(s16 *)(r2 + 4);
                *fp = (*fp) | 0x10;
                unk_5cc = 0;
                mHorzSpeed = 0;
                mVertAccel = 0;
                mVertSpeed = 0;
                mSubState = 0;
                mAreaId = mAreaIdx;
            }
            return 1;
        }
    }
    Unk_02005d94();
    t = UpdateYoshiEat(mWithMeshClsn);
    if (t != 0) {
        if (t == 1) {
            ve.x = mCapPosX;
            ve.y = mCapPosY;
            ve.z = mCapPosZ;
            if (GetCapEatenOffIt(ve) != 0)
                return 1;
        }
        mVertAccel = -0x2000;
        func_ov063_02119ab0(c);
        if (mEatenByYoshi != 0) {
            u16 *p100 = (u16 *)&mStateTimer;
            if (p100[2] == 0) {
                mEatenByYoshi = 0;
                ((u16 *)&mStateTimer)[2] = 0;
                mVertAccel = 0;
                mVertSpeed = 0;
                goto block_39;
            }
        }
        v = (((mFlags) & 0x40000) ? 1 : 0);
        if (v != 0) {
            u8 st = mTalkStep;
            Player *pl = (Player *)mEatingPlayer;
            switch (st) {
            case 0:
                if (pl->ShowMessage(*this, 0x15a, 0, 0, 2) != 0) {
                    bp = (u8 *)(c + 0x5d1);
                    *bp = (*bp) + 1;
                    func_0201267c(0xf8, &mCamSpacePosX);
                }
                break;
            case 1:
                if (pl->GetTalkState() == -1) {
                    pl->DropActor();
                    bp = (u8 *)(c + 0x5d1);
                    *bp = (*bp) + 1;
                }
                break;
            }
        }
        func_ov063_021166ac(c);
        mdCcAcPos_c.Clear();
        return 1;
    }
block_39:
    mTalkStep = 0;

    {
        mClosestPlayer = ClosestPlayer();
        Player *plr = mClosestPlayer;
        if (plr != 0) {
            s32 *pp = (s32 *)(((char *)plr) + 0x5c);
            pv.x = pp[0];
            pv.y = pp[1];
            pv.z = pp[2];
            /* (Vector3 *)&mPosX pun: no shared overlay accessor exists. */
            mAngleToPlayer = Vec3_HorzAngle((const Vector3 *)&mPosX, &pv);
            mDistToPlayer = Vec3_HorzDist((const Vector3 *)&mPosX, &pv);
        } else {
            r1 = c + 0x500;
            *(s16 *)(r1 + 0xb0) = mAngleY;
            mDistToPlayer = 0x2710000;
        }
    }
    mPrevState = unk_5cc;
    switch (unk_5cf) {
    case 0:
    case 1:
    case 2:
        func_ov063_021192d4(c);
        break;
    case 3:
        func_ov063_02116bf4(c);
        break;
    case 4:
        func_ov063_02116a1c(c);
        break;
    case 5:
        func_ov063_02116d38(c);
        break;
    case 6:
    case 10:
        func_ov063_021192d4(c);
        break;
    case 7:
        func_ov063_0211ab68(c);
        break;
    case 12:
    case 13:
    case 14:
        func_ov063_021189f4(c);
        break;
    case 15:
        func_ov063_021172a8(c);
        break;
    case 8:
        func_ov063_02119274(c);
        break;
    case 9:
        func_ov063_021192d4(c);
        break;
    case 11:
        func_ov063_02116fac(c);
    }

    /* b2 set: copy the yaw through. */
    if (mFlags_5d4.b2 != 0)
        mAngleY = mPrevAngleY;
    *(u16 *)&mStateTimer += 1;
    DecIfAbove0_Short(&mTimer5c0);
    if (mPrevState != unk_5cc)
        mStateTimer = 0;
    if (unk_5cf != 3) {
        UpdatePos(&mdCcAcPos_c);
        func_ov063_02119ab0(c);
        /* b0 set: clamp z. */
        if ((mFlags_5d4.b0 != 0) && (mPosZ < -0x12c000))
            mPosZ = -0x12c000;
        dBgCh_Gnd rc1;
        y = mPosY;
        z = mPosZ;
        w = y + 0x32000;
        x = mPosX;
        v1.x = x;
        v1.y = w;
        v1.z = z;
        rc1.SetObjAndPos(v1, (dActor_c*)this);
        if (rc1.DetectClsn() != 0) {
            s32 ground = rc1.clsnY + 0x2000;
            if (mPosY < ground)
                mPosY = ground;
        }
        if ((unk_5cf != 4) && (unk_5cf != 0xb)) {
            dBgCh_Gnd rc2;
            y = mPosY;
            z = mPosZ;
            w = y + 0x32000;
            x = mPosX;
            v2.x = x;
            v2.y = w;
            v2.z = z;
            rc2.SetObjAndPos(v2, (dActor_c*)this);
            d1 = (int)(actorID == 0xd1);
            if (d1 != 0) {
                if (unk_5cf < 8) {
                    /* b5 set: ground already found. */
                    if (mFlags_5d4.b5 != 0) {
                        if ((rc2.DetectClsn() == 0) ||
                            ((mPosY - rc2.clsnY) > 0x12c000)) {
                            mPosX = mLastGroundPosX;
                            mPosY = mLastGroundPosY;
                            mPosZ = mLastGroundPosZ;
                        } else {
                            mLastGroundPosX = mPosX;
                            mLastGroundPosY = mPosY;
                            mLastGroundPosZ = mPosZ;
                        }
                    } else {
                        goto ray_e;
                    }
                } else {
                    goto ray_e;
                }
            } else {
            ray_e:
                if ((rc2.DetectClsn() != 0) &&
                    ((mPosY - rc2.clsnY) < 0x12c000)) {
                    fp = (u16 *)(c + 0x5d4);
                    *fp = (*fp) | 0x20;
                    mLastGroundPosX = mPosX;
                    mLastGroundPosY = mPosY;
                    mLastGroundPosZ = mPosZ;
                }
            }
            UpdateWMClsn(mWithMeshClsn, 0);
        }
    }
    if ((((unk_5cc != 3) && (unk_5cc != 3)) && (unk_5cc != 3)) && (unk_5cc != 3))
        mModelAnim.Advance();
    func_ov063_021166ac(c);
    mdCcAcPos_c.Clear();
    if (mOpacity == 0xff) {
        mdCcAcPos_c.SetPosRelativeToActor(*(const Vector3 *)&mClsnOffX);
        mdCcAcPos_c.Update();
    }
    return 1;
}
