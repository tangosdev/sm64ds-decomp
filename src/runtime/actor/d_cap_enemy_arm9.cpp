//cpp
/* dCapEnemy_c, the arm9 half (0x02005d94..0x02006588): the cap attach,
 * release, respawn and state helpers, then D1 and C2.
 * src/runtime/actor/d_cap_enemy.cpp is the ov002 half and already owns that
 * path, so this file is the other module. rombuild refuses one complete path
 * in two modules; D2 and D0 stay in the ov002 TU.
 *
 * The constructor and the destructor are the real members. The destructor is
 * inline, so D1 and D0 come out vague and D2 is not emitted. D0 duplicates
 * the ov002 body and is discarded; C1, emitted beside C2, has no ROM home
 * and is discarded. PinD1 is the use that places D1 before C2 -- without it
 * the vague destructor trails the constructor. That destructor is also what
 * emits _ZTV11dCapEnemy_c.
 *
 * Source order is reverse ROM order. The constructor is written before D1.
 *
 * leftovers:
 *  - The 0x17f flags byte (mCapIcon.mFlags high byte) and the 0xf4 shadow
 *    flags word have no member names; the measured ldrb/launder forms stay.
 *    AddCap's mFlags &= ~1 and its mCapId store go through plain `&member`
 *    pointers.
 *  - UpdateCapPos keeps a POD VecFx32 local: Vector3's empty destructor
 *    changes the frame. Its matrix copy goes through Mtx43; a plain
 *    Matrix4x3 assignment is memberwise and 0x1c bytes longer.
 */
#include "dCapEnemy_c.h"
#include "daPly_c.h"
#include "SharedFilePtr.h"
#include "decl_Enemy.h"
#include "nitro/fx/fx.h"
#include "private/mtx43.h"
#include "math/MtxStack.h"
#include "math/CalcLib.h"

extern SharedFilePtr *data_ov002_020ff028[];

extern unsigned char gGameMode;
extern unsigned char data_0208a0e0;
extern daPly_c *gPlayers[];
extern "C" {

extern void Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);
}

dCapEnemy_c::dCapEnemy_c() {}

/* The call is not in the ROM. It is the odr-use that makes mwcc emit the
 * inline destructor's D1 section here, ahead of C2, instead of trailing it. */
void PinD1(dCapEnemy_c *self) {
    self->~dCapEnemy_c();
}

#pragma never_inline on
inline dCapEnemy_c::~dCapEnemy_c() {}
#pragma never_inline off

void dCapEnemy_c::UnloadCapModel()
{
    s32 idx = mCapId & 7;
    if (idx >= 6) return;
    data_ov002_020ff028[idx]->Release();
}

int dCapEnemy_c::AddCap(unsigned int param)
{
    mIsDormant = 0;
    if (param >= 6) {
        mCapId = 6;
        return 6;
    }

    mCapId = (int)param % 3;

    if (param >= 3) {
        mCapBank = 1;
        if (mHadBank1Cap == 0) mHadBank1Cap = 1;
    } else {
        mCapBank = 0;
    }

    BMD_File *file = dExtSimpleModel_c::LoadFile(*data_ov002_020ff028[mCapId]);
    int ok = mModel.SetFile(file, 1, -1);
    if (ok == 0) {
        mCapId = 6;
        /* Release is void. The value it leaves in r0 is AddCap's result. */
        data_ov002_020ff028[mCapId]->Release();
        return;
    }

    *(int *)(&mFlags) &= ~1;
    mCapIcon.Link((char *)this, mCapId, mHadBank1Cap, mCapBank != 0);

    int result = func_02005e28();
    if (result != 0) {
        unsigned char *p = (unsigned char *)(&mCapId);
        result = *p | 0x80;
        *p = result;
    }
    return result;
}

void dCapEnemy_c::UpdateCapPos(const Vector3& pos, const Vector3_16& rot)
{
    /* mCapIcon.mFlags (+0x17f) is read through an inline cast on each
       access. A local CapFlags* there makes mwcc materialize the (non-immediate) offset
       through the literal pool (ldr r3,[pc] / ldrb r0,[r5,r3]) and the
       function grows a word; the cartridge folds it (ldrb r0,[r5,#0x17f]). */
    struct CapFlags {
        unsigned char bank : 1;
        unsigned char real : 1;
        unsigned char unlinked : 1;
        unsigned char rest : 5;
    };
    if (((CapFlags *)&mCapIcon.mFlags)->unlinked != 0 ||
        ((CapFlags *)&mCapIcon.mFlags)->real == 0) {
        if ((mCapId & 0xf) < 6) {
            mCapId |= 0x80;
        }
    } else {
        mCapId &= 7;
    }
    if (mCapId >= 6) return;

    VecFx32 sum, asr;
    Vec3_Add((Vector3 *)&sum, &mPos, &pos);
    Vec3_Asr((Vector3 *)&asr, (Vector3 *)&sum, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, asr.x, asr.y, asr.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, rot.x, rot.y, rot.z);
    *(Mtx43 *)&mModel.mat4x3 = *(Mtx43 *)&data_020a0e68;
}

void dCapEnemy_c::RenderCapModel(const Vector3 * v)
{
    if (mCapId >= 6) return;
    mModel.Render(v);
}

struct dActor_c *dCapEnemy_c::ReleaseCap(const Vector3 & v_)
{
    const struct Vector3 *v = &v_;
    struct dActor_c *ret = 0;

    if ((mCapId & 7) < 6) {
        func_02005ed8();
        if (mCapId < 6u) {
            VecFx32 out;
            Vec3_Add((Vector3 *)&out, &mPos, v);
            ret = dActor_c::Spawn(0x10d, 0x1012 | (mCapId << 8), *(const Vector3 *)&out, &mAngle,
                                  mAreaId, -1);
            if (mCapBank != 0) {
                mCapId |= 8;
            } else {
                mCapId |= 0x80;
            }
        } else {
            ret = this;
        }
    }
    return ret;
}

int dCapEnemy_c::GetCapEatenOffIt(const Vector3 & v_)
{
    const Vector3 *v = &v_;
    VecFx32 local;
    unsigned char *p;
    local.x = v->x;
    local.y = v->y;
    local.z = v->z;
    p = (unsigned char *)ReleaseCap(*(const Vector3 *)&local);
    if (p != 0 && p != (unsigned char *)this) {
        unsigned char idx;
        ((dEnemyBase_c *)p)->mEatingPlayer = mEatingPlayer;
        mEatingPlayer->mObjInMouth = (dActor_c *)p;
        ((dActor_c *)p)->mFlags |= 0x20000;
        mFlags &= ~0x20000;
        mEatingPlayer = 0;
        if (mCapBank == 0) {
            idx = mCapId & 7;
            mCapIcon.Link((char *)this, idx, 0, 0);
        }
        return 1;
    }
    return 0;
}

struct dActor_c * dCapEnemy_c::RespawnIfHasCap()
{
    struct dActor_c *r;
    func_02005ed8();
    if ((mCapId & 0xf) >= 6) return 0;
    r = dActor_c::Spawn(actorID, param1, mPos, &mAngle, mAreaId, -1);
    if (!r) return r;
    ((dCapEnemy_c *)r)->mIsDormant = 1;
    ((dCapEnemy_c *)r)->mDormantFlags = r->mFlags;
    ((dEnemyBase_c *)r)->mCoinType = 0;
    {
        r->mFlags &= ~1;
        r->mFlags &= ~0x10000000;
    }
    return r;
}

/* The u64 no-op mask sits on the mDormantFlags read, not on the mFlags store.
   Demoting that load out of its fold-onto-[r4,#0xf4] value-numbering class is
   what lets the RMW-pool chain lead the tail; the plain member read schedules
   the 0xf4 load ahead of the mCapId pool load (4 words, re-measured #821). The masked mCapId RMW is the
   usual 6g launder. */
int dCapEnemy_c::GetCapState() {
    if (mIsDormant == 0) {
        return 2;
    }

    unsigned char val = *(unsigned char *)&gGameMode;
    int check = (val == 1) ? 1 : 0;
    if (check == 0) {
        daPly_c *p = ClosestPlayer();
        if (p != 0) {
            int s = mCapId & 7;
            int ps = p->param1;
            if (s == ps) {
                return 0;
            }
        }
    }

    unsigned int b = mCapIcon.mFlags;
    b = (b << 0x1e) >> 0x1f;
    if (b == 0) {
        return 0;
    }

    mIsDormant = 0;
    mFlags = *(int *)((long long)(int)&mDormantFlags);
    mCapId &= 7;
    return 1;
}

int dCapEnemy_c::DestroyIfCapNotNeeded() {
    if (mCapId >= 6)
        return 1;
    if ((int)(gGameMode == 1) != 0)
        return 1;
    daPly_c *p = ClosestPlayer();
    if (p == 0) {
        func_02005ed8();
        return 0;
    }
    if (mCapId != p->mCharacter)
        return 1;
    func_02005ed8();
    return 0;
}

void dCapEnemy_c::func_02005ed8() {
    if ( ((unsigned)((unsigned char *)this)[0x17f] << 29) >> 31 )
        return;
    mCapIcon.Unlink();
}

int dCapEnemy_c::func_02005e28()
{
    int b;
    int i;
    int n;
    b = (int)(gGameMode == 1);
    if (b != 0) {
        n = data_0208a0e0;
        for (i = 0; i < n; i++) {
            daPly_c *p = gPlayers[i];
            if (p != 0) {
                if (p->param1 == mCapId) return 1;
            }
        }
        return 0;
    } else {
        daPly_c *p = ClosestPlayer();
        int r1;
        if (p != 0) r1 = p->param1;
        else r1 = mCapId;
        return (r1 == mCapId) ? 1 : 0;
    }
}

void dCapEnemy_c::Unk_02005d94()
{
    int b1;
    int b2;
    if (!(mCapId & 0x80)) return;
    b1 = (mFlags & 8) ? 1 : 0;
    if (b1 == 0) {
        volatile unsigned char *q = &gGameMode;
        b2 = (q[0] == 1) ? 1 : 0;
        if (b2 == 0) return;
    }
    mCapId &= 7;
    if (func_02005e28())
        mCapId |= 0x80;
}
