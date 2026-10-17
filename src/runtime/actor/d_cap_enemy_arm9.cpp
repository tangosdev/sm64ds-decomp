//cpp
/* dCapEnemy_c, the arm9 half (0x02005d94..0x0200651c): the cap attach,
 * release, respawn and state helpers. src/runtime/actor/d_cap_enemy.cpp is the
 * ov002 half and already owns that path, so this file is the other module.
 * rombuild refuses one complete path in two modules; D2 and D0 stay in the
 * ov002 TU.
 *
 * D1 (0x0200651c) and C2 (0x02006554) keep their single-function files. The
 * destructor is this class's key function -- a member ~dCapEnemy_c() is the
 * only definition that emits _ZTV11dCapEnemy_c -- and a member dtor or ctor
 * in this multi-function object would also emit D0/D2/C1 as STB_GLOBAL,
 * homed to the ov002 TU, which plan_many cannot deadstrip.
 *
 * Source order is reverse ROM order.
 *
 * leftovers:
 *  - ModelBase::SetFile has no header member, so AddCap keeps the mangled
 *    extern for it; SharedFilePtr::Release's return value feeds AddCap's
 *    failure path, so that call keeps the int-returning mangled spelling
 *    too (the member returns void).
 *  - The 0x17f flags byte (mCapIcon.mFlags high byte) and the 0xf4 shadow
 *    flags word have no member names; the measured ldrb/launder forms stay.
 *    AddCap's mFlags &= ~1 and its mCapId store also keep the laundered
 *    pointer forms (`(long long)(int)&member`) -- plain member access there
 *    merges value-numbering classes and shortens the function by three
 *    words.
 *  - UpdateCapPos keeps CapVec/CapMtx POD shadows: Vector3's empty
 *    destructor and Matrix4x3's embedded Vector3 change the frame.
 */
#include "dCapEnemy_c.h"
#include "Player.h"
#include "SharedFilePtr.h"

extern "C" {
extern int _ZN13SharedFilePtr7ReleaseEv(void *sfp);
extern SharedFilePtr *data_ov002_020ff028[];

extern unsigned char data_0209f2d8;
extern unsigned char data_0208a0e0;
extern void *data_0209f394[];

extern void Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);

/* UpdateCapPos's stack temps are POD. Vector3's empty destructor and
   Matrix4x3's embedded Vector3 change this function's frame. */
struct CapVec { int x, y, z; };
struct CapMtx { int a[12]; };
extern void Vec3_Asr(CapVec *vF, const CapVec *v, int amount);
extern void Matrix4x3_FromTranslation(CapMtx *mF, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(CapMtx *mF, short x, short y, short z);
extern CapMtx data_020a0e68;
}

// @symbol _ZN11dCapEnemy_c14UnloadCapModelEv
void dCapEnemy_c::UnloadCapModel()
{
    s32 idx = mCapId & 7;
    if (idx >= 6) return;
    data_ov002_020ff028[idx]->Release();
}

// @symbol _ZN11dCapEnemy_c6AddCapEj
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

    void *file = Model::LoadFile(*data_ov002_020ff028[mCapId]);
    int ok = mModel.SetFile((BMD_File *)file, 1, -1);
    if (ok == 0) {
        mCapId = 6;
        return _ZN13SharedFilePtr7ReleaseEv(data_ov002_020ff028[mCapId]);
    }

    *(int *)(((long long)(int)&mFlags)) &= ~1;
    mCapIcon.func_ov001_020ab228((char *)this, mCapId, mHadBank1Cap, mCapBank != 0);

    int result = func_02005e28();
    if (result != 0) {
        unsigned char *p = (unsigned char *)(((long long)(int)&mCapId));
        result = *p | 0x80;
        *p = result;
    }
    return result;
}

// @symbol _ZN11dCapEnemy_c12UpdateCapPosERK7Vector3RK10Vector3_16
void dCapEnemy_c::UpdateCapPos(const Vector3& pos, const Vector3_16& rot)
{
    char* self = (char*)this;
    /* The flags byte is read through an inline cast on each access. A local
       CapFlags* at +0x17f makes mwcc materialize the (non-immediate) offset
       through the literal pool (ldr r3,[pc] / ldrb r0,[r5,r3]) and the
       function grows a word; the cartridge folds it (ldrb r0,[r5,#0x17f]). */
    struct CapFlags {
        unsigned char b0 : 1;
        unsigned char b1 : 1;
        unsigned char b2 : 1;
        unsigned char b3to7 : 5;
    };
    if (((CapFlags*)(self + 0x17f))->b2 != 0 || ((CapFlags*)(self + 0x17f))->b1 == 0) {
        if ((mCapId & 0xf) < 6) {
            mCapId |= 0x80;
        }
    } else {
        mCapId &= 7;
    }
    if (mCapId >= 6) return;

    CapVec sum, asr;
    Vec3_Add((Vector3 *)&sum, &mPos, &pos);
    Vec3_Asr(&asr, &sum, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, asr.x, asr.y, asr.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, rot.x, rot.y, rot.z);
    *(CapMtx *)&mModel.mat4x3 = data_020a0e68;
}

// @symbol _ZN11dCapEnemy_c14RenderCapModelEPK7Vector3
void dCapEnemy_c::RenderCapModel(const Vector3 * v)
{
    if (mCapId >= 6) return;
    mModel.Render(v);
}

// @symbol _ZN11dCapEnemy_c10ReleaseCapERK7Vector3
struct dActor_c *dCapEnemy_c::ReleaseCap(const Vector3 & v_)
{
    const struct Vector3 *v = &v_;
    struct dActor_c *ret = 0;

    if ((mCapId & 7) < 6) {
        func_02005ed8();
        if (mCapId < 6u) {
            CapVec out;
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

// @symbol _ZN11dCapEnemy_c16GetCapEatenOffItERK7Vector3
int dCapEnemy_c::GetCapEatenOffIt(const Vector3 & v_)
{
    const Vector3 *v = &v_;
    CapVec local;
    unsigned char *p;
    local.x = v->x;
    local.y = v->y;
    local.z = v->z;
    p = (unsigned char *)ReleaseCap(*(const Vector3 *)&local);
    if (p != 0 && p != (unsigned char *)this) {
        unsigned char idx;
        ((dEnemyBase_c *)p)->mEatingPlayer = mEatingPlayer;
        ((Player *)mEatingPlayer)->mObjInMouth = (s32)p;
        ((dActor_c *)p)->mFlags |= 0x20000;
        mFlags &= ~0x20000;
        mEatingPlayer = 0;
        if (mCapBank == 0) {
            idx = mCapId & 7;
            mCapIcon.func_ov001_020ab228((char *)this, idx, 0, 0);
        }
        return 1;
    }
    return 0;
}

// @symbol _ZN11dCapEnemy_c15RespawnIfHasCapEv
struct dActor_c * dCapEnemy_c::RespawnIfHasCap()
{
    struct dActor_c *r;
    func_02005ed8();
    if ((mCapId & 0xf) >= 6) return 0;
    r = dActor_c::Spawn(actorID, param1, mPos, &mAngle, mAreaId, -1);
    if (!r) return r;
    ((dCapEnemy_c *)r)->mIsDormant = 1;
    *(int *)((char *)r + 0xf4) = r->mFlags;
    ((dEnemyBase_c *)r)->unk_108 = 0;
    {
        r->mFlags &= ~1;
        r->mFlags &= ~0x10000000;
    }
    return r;
}

/* The u64 no-op mask sits on the plain 0xf4 read, not on the mFlags store.
   Demoting that load out of its fold-onto-[r4,#0xf4] value-numbering class is
   what lets the RMW-pool chain lead the tail. The masked mCapId RMW is the
   usual 6g launder. */
// @symbol _ZN11dCapEnemy_c11GetCapStateEv
int dCapEnemy_c::GetCapState() {
    if (mIsDormant == 0) {
        return 2;
    }

    unsigned char val = *(unsigned char *)&data_0209f2d8;
    int check = (val == 1) ? 1 : 0;
    if (check == 0) {
        Player *p = ClosestPlayer();
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
    mFlags = *(int *)((long long)(int)((char *)this + 0xf4));
    mCapId &= 7;
    return 1;
}

// @symbol _ZN11dCapEnemy_c21DestroyIfCapNotNeededEv
int dCapEnemy_c::DestroyIfCapNotNeeded()
{
  if (mCapId >= 6) return 1;
  if ((int)(data_0209f2d8 == 1) != 0) return 1;
  Player *p = ClosestPlayer();
  if (p == 0) { func_02005ed8(); return 0; }
  if (mCapId != p->mCharacter) return 1;
  func_02005ed8();
  return 0;
}

// @symbol _ZN11dCapEnemy_c13func_02005ed8Ev
void dCapEnemy_c::func_02005ed8() {
    if ( ((unsigned)((unsigned char *)this)[0x17f] << 29) >> 31 )
        return;
    mCapIcon.Unlink();
}

// @symbol _ZN11dCapEnemy_c13func_02005e28Ev
int dCapEnemy_c::func_02005e28()
{
    int b;
    int i;
    int n;
    b = (int)(data_0209f2d8 == 1);
    if (b != 0) {
        n = data_0208a0e0;
        for (i = 0; i < n; i++) {
            Player *p = (Player *)data_0209f394[i];
            if (p != 0) {
                if (p->param1 == mCapId) return 1;
            }
        }
        return 0;
    } else {
        Player *p = ClosestPlayer();
        int r1;
        if (p != 0) r1 = p->param1;
        else r1 = mCapId;
        return (r1 == mCapId) ? 1 : 0;
    }
}

// @symbol _ZN11dCapEnemy_c12Unk_02005d94Ev
void dCapEnemy_c::Unk_02005d94()
{
    int b1;
    int b2;
    if (!(mCapId & 0x80)) return;
    b1 = (mFlags & 8) ? 1 : 0;
    if (b1 == 0) {
        volatile unsigned char *q = &data_0209f2d8;
        b2 = (q[0] == 1) ? 1 : 0;
        if (b2 == 0) return;
    }
    mCapId &= 7;
    if (func_02005e28())
        mCapId |= 0x80;
}
