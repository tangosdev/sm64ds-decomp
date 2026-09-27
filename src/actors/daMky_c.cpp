//cpp
/* Tall Tall Mountain Ukiki. UKIKI_THIEF is actor 0x10b and UKIKI_STAR is
 * actor 0x10c. ov030:0x02115b78 is the bytes 7daMky_c. The star monkey's
 * talk (func_ov030_02112578) finds UKIKI_CAGE, actor 0x67, and shoves it.
 *
 * ROM-ascending under `#pragma defer_codegen off`, so ~daMky_c emits D1
 * then D0 (0x02111688, 0x021116d0). D2 has no ROM home. The destructor is
 * the key function. Factories stay in d_a_mky_monkey_*.c.
 *
 * Still address-shaped, measured on this TU:
 * - func_ov030_* are daMky_c methods. The cartridge does not spell them,
 *   so the method name is the address. Of the 22 pointer-to-member
 *   records at 0x02115ac8, EnterState0..10 store mState. The other
 *   eleven are the ticks, also methods. Helpers those ticks call take
 *   the same Ukiki in r0, so they are methods too.
 * - ModelAnim::SetAnim, dCcAc_c::Init, dBgCh_Actr::Init,
 *   DropShadowRadHeight, IsTooFarAwayFromPlayer, Clipper::Func_02015560
 *   and Sound::PlaySub pass Fix12<int> by value. The method form changes
 *   the call. dBgCh_Actr::Init's header is Fix12i, which mangles as i;
 *   the ROM symbol is Fix12<int>.
 * - func_0201267c is the bank-3 wrapper at 0x0201267c. PlayBank3 is the
 *   other wrapper, at 0x02012664. func_02012790 is Sound::Play2D(2, id).
 * - func_020383f0 and dBgCh_Actr_UpdateContinuous_Veneer are tail-call
 *   veneers. The bl targets the veneer, not UpdateContinuousNoLava or
 *   UpdateContinuous.
 * - func_0203567c returns its argument plus 0x30. GetFloorResult is the
 *   previous symbol, 0x0203566c, and dBgCh_Actr.h does not declare it.
 *   func_02038ea4 is not dBgCh_Gnd::DetectClsn: 02111ea4 calls DetectClsn
 *   and 02111dd0 calls func_02038ea4.
 * - func_02037f44 returns word 8 of the record it is handed. The polygon
 *   fill in 02112400 and 02112a84 is a field copy. A dBgPi local would
 *   emit C1, which those bodies do not call.
 * - The cap spawned as actor 0x10d stores &mCapMtx at +0xc8. That word is
 *   inside dActor_c::pad_0c5, and daObjMarioCap_c does not name it.
 *   Player::param1 (+8) is the character number.
 * - data_ov030_* file handles stay address labels. decl_common.h types
 *   four of them as void*[] where this TU's blocks say int[]; a second
 *   data declaration is rejected. g_profile is not this TU's data.
 * - unk_3c7 / mAnimIdx increments that take the address in a register stay
 *   in that form. func_ov030_02112094 copies the model matrix through a
 *   12-word POD: Matrix4x3 embeds Vector3, and Vector3 has a destructor.
 */
#pragma defer_codegen off

#include "types.h"
#include "daMky_c.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "decl_PathPtr.h"
#include "decl_SaveData.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_Actr.h"
#include "SaveData.h"
#include "Player.h"
#include "SurfaceInfo.h"
#include "dBgCh_Lin.h"

/* EnterState* are C++ members, so they cannot hold an extern "C" block.
 * These four are the spellings those members call. A free function below
 * may redeclare one of them; -gccext,on keeps the second spelling. Data
 * objects are not hoisted: two data spellings of one name are rejected. */
extern "C" {

void  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int idx, int speed, unsigned int flags);
short Vec3_HorzAngle(const void *a, const void *b);
int   Vec3_Dist(const void *a, const void *b);
void func_0201267c(unsigned int id, const Vector3 *pos);

}

/* ~daMky_c is the first virtual DECLARED in include/daMky_c.h and it is
 * defined here, out of line, so it is this class's key function: this TU owns
 * _ZTV7daMky_c, _ZTI7daMky_c and _ZTS7daMky_c.
 *
 * Under `#pragma defer_codegen off` an out-of-line destructor emits D1, then
 * D0, then D2, in that order and at this position.  The cartridge puts D1 at
 * 0x02111688 and D0 at 0x021116d0, the two lowest addresses in the run, which
 * is exactly what this produces.  D2 has no ROM home anywhere and is
 * deadstripped.
 *
 * The body is empty on purpose.  The compiler writes the whole of both
 * variants from the class definition: the vptr store, then ModelAnim,
 * ShadowModel, dCcAc_c and dBgCh_Actr destroyed in reverse declaration order
 * (PathPtr is trivial and skipped), then ~dActor_c; D0 additionally inlines
 * all of that and tail-calls Memory::Deallocate. */
// @symbol _ZN7daMky_cD1Ev
// @symbol _ZN7daMky_cD0Ev
daMky_c::~daMky_c()
{
}

// @symbol _ZN7daMky_c13OnYoshiTryEatEv
/* Vtable slot 18.  Two instructions: mov r0,#7; bx lr. */
s32 daMky_c::OnYoshiTryEat()
{
    return 7;
}

// @symbol _ZN7daMky_c19func_ov030_02111734Ev
void daMky_c::func_ov030_02111734()
{
    extern unsigned char DecIfAbove0_Byte(unsigned char* p);
    extern void *_ZN9dBgCh_LinC1Ev(void* self);
    extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void* self, void* a, void* b, void* actor);
    extern int _ZN9dBgCh_Lin10DetectClsnEv(void* self);
    extern void Vec3_Asr(void* d, void* s, int sh);
    extern int _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(void* m, void* v, void* w, int fix, void* out);
    extern void _ZN9dBgCh_LinD1Ev(void* self);

    extern char data_0209f43c;
    extern char data_0209b3ec;
    struct Vector3 a, b, out, asr;
    char rc[0x7c];

    if (DecIfAbove0_Byte((unsigned char*)((char *)this + 0x3cb)))
        return;

    _ZN9dBgCh_LinC1Ev(rc);

    a.x = this->mPosX;
    a.y = this->mPosY;
    a.z = this->mPosZ;
    a.y = a.y + 0x32000;
    b.x = this->mPosX;
    b.y = this->mPosY;
    b.z = this->mPosZ;
    b.y = b.y - 0x96000;
    ((dBgCh_Lin *)rc)->SetObjAndLine(a, b, (dActor_c *)(char *)this);

    if (this->mPerchPosY - this->mPosY <= 0x96000) {
        if (!((dBgCh_Lin *)rc)->DetectClsn())
            goto done;
    }

    Vec3_Asr(&asr, (Vector3 *)&this->mPerchPosX, 3);

    if (_ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(&data_0209f43c, &data_0209b3ec, &asr, 0x1f400, &out) <= 0xc350000)
        goto done;

    this->mPosX = this->mPerchPosX;
    this->mPosY = this->mPerchPosY;
    this->mPosZ = this->mPerchPosZ;
    this->mSpawnPosX = this->mPerchPosX;
    this->mSpawnPosY = this->mPerchPosY;
    this->mSpawnPosZ = this->mPerchPosZ;
    this->mPrevPosX = this->mPerchPosX;
    this->mPrevPosY = this->mPerchPosY;
    this->mPrevPosZ = this->mPerchPosZ;
    func_ov030_02112094();
    this->unk_3cb = 0x96;

done:
    _ZN9dBgCh_LinD1Ev(rc);
}
// @symbol _ZN7daMky_c19func_ov030_02111890Ev
void daMky_c::func_ov030_02111890()
{
    extern void func_0201267c(unsigned int id, const Vector3 *pos);
    extern int data_ov030_02115d18[];
    int b = (int)((int)this->mModelAnim.file == data_ov030_02115d18[1]);
    if (b == 0)
        return;
    int v = (short)((unsigned int)this->mModelAnim.currFrame << 4 >> 16);
    if (v == 0xa || v == 0xc)
        func_0201267c(0xea, (const Vector3 *)&this->mCamSpacePosX);
}
// @symbol _ZN7daMky_c19func_ov030_02111908Ev
void daMky_c::func_ov030_02111908()
{
    extern int data_ov030_02115cf0[];
    extern int data_ov030_02115cd0[];
    extern int data_ov030_02115cf8[];
    extern void func_0201267c(unsigned int id, const Vector3 *pos);

    enum Bool { FALSE, TRUE };
    int frame = (short)(((unsigned)(this->mModelAnim.currFrame << 4)) >> 16);
    int v = (int)this->mModelAnim.file;
    enum Bool b;

    b = (enum Bool)(v == data_ov030_02115cf0[1]);
    if (b) {
        if (frame != 7) {
            if (frame != 0x28) return;
        }
        func_0201267c(0xeb, (const Vector3 *)&this->mCamSpacePosX);
        return;
    }
    b = (enum Bool)(v == data_ov030_02115cd0[1]);
    if (b) {
        if (frame != 1) return;
        func_0201267c(0xf1, (const Vector3 *)&this->mCamSpacePosX);
        func_0201267c(0xe8, (const Vector3 *)&this->mCamSpacePosX);
        return;
    }
    b = (enum Bool)(v == data_ov030_02115cf8[1]);
    if (b) {
        if (frame != 8) return;
        func_0201267c(0xe9, (const Vector3 *)&this->mCamSpacePosX);
    }
}
// @symbol _ZN7daMky_c19func_ov030_02111a00Ev
int daMky_c::func_ov030_02111a00()
{
    extern int _ZNK9Animation12WillHitFrameEi(void* a, int f);
    extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* m, void* f, int a, int b, unsigned int e);
    extern int data_ov030_02115cf0[];
    extern int data_ov030_02115cd0[];
    extern int data_ov030_02115cf8[];
    extern int data_ov030_02115cd8[];
    extern void** data_ov030_02115bc8[];
    func_ov030_02111908();
    if (((Animation *)((char *)this + 0x124))->WillHitFrame( 0) == 0) {
        int v = (int)this->mModelAnim.file;
        int b;
        b = (int)(v == data_ov030_02115cf0[1]); if (b != 0) goto fail;
        b = (int)(v == data_ov030_02115cd0[1]); if (b != 0) goto fail;
        b = (int)(v == data_ov030_02115cf8[1]); if (b != 0) goto fail;
        b = (int)(v == data_ov030_02115cd8[1]); if (b != 0) goto fail;
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &this->mModelAnim, data_ov030_02115bc8[this->mAnimIdx][1], 0, 0x1000, 0);
    {
        unsigned char* p = (unsigned char*)(((int)(char *)this + 0x3ca));
        this->mModelAnim.speed = 0x1000;
        (*p)++;
    }
    if (this->mAnimIdx >= 0xb)
        this->mAnimIdx = 0;
    return 1;
fail:
    return 0;
}
// @symbol _ZN7daMky_c19func_ov030_02111b20Ev
int daMky_c::func_ov030_02111b20()
{
    typedef short s16;

    extern void _ZNK7PathPtr7GetNodeER7Vector3j(void* self, struct Vector3* v, unsigned int i);
    extern int Vec3_HorzDist(const struct Vector3* a, const struct Vector3* b);
    extern s16 Vec3_HorzAngle(const struct Vector3* a, const struct Vector3* b);
    extern void _Z11UpdateAngleRssis(short* p, short a, int b, short c);
  struct Vector3 v;
  int d;
  s16 ang;
  int *p;
  int n;
  /* GetNode's header takes Vector3&. This body was matched as a pointer
     call, and the member form is four words off. mPathNode is unsigned;
     the compare here is a signed word. */
  _ZNK7PathPtr7GetNodeER7Vector3j((char *)this+0x398, &v, *(unsigned int*)((char *)this+0x3a0));
  d = Vec3_HorzDist((struct Vector3*)((char *)this+0x5c), &v);
  ang = Vec3_HorzAngle((struct Vector3*)((char *)this+0x5c), &v);
  _Z11UpdateAngleRssis((short*)((char *)this+0x8e), ang, 2, 0x400);
  *(s16*)((char *)this+0x94) = *(s16*)((char *)this+0x8e);
  if (d < *(int*)((char *)this+0x98)) {
    n = _ZNK7PathPtr8NumNodesEv((char *)this+0x398);
    p = (int*)((char *)this + 0x3a0);
    n = n - 1;
    *p = *p + 1;
    if (*(int*)((char *)this+0x3a0) >= n) return 1;
  }
  return 0;
}
// @symbol _ZN7daMky_c19func_ov030_02111bc4Ev
int daMky_c::func_ov030_02111bc4()
{
    extern void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
    extern int _ZN6Player7TryGrabER8dActor_c(void *p, void *a);
    unsigned char *player;
    int b;

    b = (int)((this->mFlags & 0x20000) != 0);
    if (b != 0 && this->mState != 2) {
        this->unk_3a8 = *(void **)((char *)this + 0xd0);
        b = (int)(this->actorID == 0x10b);
        if (b != 0) {
            func_ov030_021141a8(5);
        } else {
            b = (int)(this->actorID == 0x10c);
            if (b != 0)
                func_ov030_021141a8(6);
        }
        return 1;
    }

    if (this->mdCcAc_c.otherOwner == 0)
        return 0;

    if ((this->mdCcAc_c.hitFlags & 0x40000) && this->mState != 2) {
        this->unk_3a8 = this->ClosestPlayer();
        this->mPrevState = this->mState;
        func_ov030_021141a8(2);
        return 1;
    }

    player = (unsigned char *)dActor_c::FindWithID(this->mdCcAc_c.otherOwner);
    if (player == 0 || (b = (int)(((Player *)player)->actorID == 0xbf)) == 0)
        return 0;

    if (((Player *)player)->mIsUnderwater != 0)
        return 0;
    if (((Player *)player)->mIsMetal != 0)
        return 0;
    if (((Player *)player)->mIsVanish != 0)
        return 0;
    if (((Player *)player)->mHasWings != 0)
        return 0;

    if (this->mdCcAc_c.hitFlags & 0x1000) {
        if (((Player *)player)->TryGrab(*(dActor_c *)this)) {
            *(void **)((char *)this + 0x3a8) = player;
            b = (int)(this->actorID == 0x10b);
            if (b != 0) {
                func_ov030_021141a8(3);
            } else {
                b = (int)(this->actorID == 0x10c);
                if (b != 0)
                    func_ov030_021141a8(4);
            }
        }
    }
    return 1;
}
// @symbol _ZN7daMky_c19func_ov030_02111dd0Ev
int daMky_c::func_ov030_02111dd0()
{
    extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* thiz);
    extern int func_02038ea4(void* thiz);
    if (this->mWithMeshClsn.IsOnGround() != 0) {
        dBgCh_Gnd rg;
        Vector3 v;
        int y, z, x, s;
        y = this->mPosY;
        z = this->mPosZ;
        x = this->mPosX;
        s = y + 0x1e000;
        v.x = x;
        v.y = s;
        v.z = z;
        rg.SetObjAndPos(v, (dActor_c*)(char *)this);
        if (func_02038ea4(&rg) == 0 || this->mPosY - rg.clsnY > 0x2000) {
            this->mPosX = this->mSpawnPosX;
            this->mPosY = this->mSpawnPosY;
            this->mPosZ = this->mSpawnPosZ;
            return 1;
        }
        this->mSpawnPosX = this->mPosX;
        this->mSpawnPosY = this->mPosY;
        this->mSpawnPosZ = this->mPosZ;
    }
    return 0;
}
// @symbol _ZN7daMky_c19func_ov030_02111ea4Ev
int daMky_c::func_ov030_02111ea4()
{

    extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);

    #define ABS(x) ((x) < 0 ? -(x) : (x))
    char* c = (char *)this;
    if (this->mWithMeshClsn.IsOnGround() != 0) {
        dBgCh_Gnd rg;
        Vector3 pos;
        {
            int y = this->mPosY;
            int z = this->mPosZ;
            int x = this->mPosX;
            int y2 = y + 0x1e000;
            pos.x = x;
            pos.y = y2;
            pos.z = z;
        }
        rg.SetObjAndPos(pos, (dActor_c*)c);
        if (rg.DetectClsn() == 0 ||
            ABS(rg.clsnY - this->mPosY) > 0x1000) {
            this->mHorzSpeed = 0;
            this->mPosX = this->mPrevPosX;
            this->mPosY = this->mPrevPosY;
            this->mPosZ = this->mPrevPosZ;
            return 1;
        }
    }
    return 0;
}
// @symbol _ZN7daMky_c19func_ov030_02111f6cEP10dBgCh_Actr
void daMky_c::func_ov030_02111f6c(dBgCh_Actr* w)
{
    typedef struct { int x, y, z; } Vector3;
    typedef struct dBgCh_Actr dBgCh_Actr;
    typedef struct SurfaceInfo SurfaceInfo;
    void func_020383f0(void* p);
    void dBgCh_Actr_UpdateContinuous_Veneer(void* p);
    int _ZNK10dBgCh_Actr10IsOnGroundEv(dBgCh_Actr* w);
    void* _ZNK10dBgCh_Actr14GetFloorResultEv(dBgCh_Actr* w);
    void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(SurfaceInfo* s, Vector3* v);
    int _ZN4cstd4fdivEii(int a, int b);
    int _ZNK10dBgCh_Actr8IsOnWallEv(dBgCh_Actr* w);
    void* _ZNK10dBgCh_Actr13GetWallResultEv(dBgCh_Actr* w);
  int b = (int)((this->mFlags & 0x4000) != 0);
  if (b != 0) return;
  int bb = (int)(this->actorID == 0x10b);
  if (bb != 0 && this->mState != 9) func_020383f0(&this->mWithMeshClsn);
  else dBgCh_Actr_UpdateContinuous_Veneer(&this->mWithMeshClsn);
  if (_ZNK10dBgCh_Actr10IsOnGroundEv(w) != 0) {
    Vector3 n;
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((SurfaceInfo*)((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(w) + 4), &n);
    if (n.y != 0) {
      int s = (int)(((long long)n.x * this->unk_0a4 + 0x800) >> 0xc)
            + (int)(((long long)n.z * this->unk_0ac + 0x800) >> 0xc);
      this->mVertSpeed = -(_ZN4cstd4fdivEii(s, n.y) + 0x8000);
    }
  }
  if (_ZNK10dBgCh_Actr8IsOnWallEv(w) != 0) {
    Vector3 wn;
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((SurfaceInfo*)((char*)_ZNK10dBgCh_Actr13GetWallResultEv(w) + 4), &wn);
  }
}
// @symbol _ZN7daMky_c19func_ov030_02112094Ev
void daMky_c::func_ov030_02112094()
{
    /* from include/decl_common.h:255, which this TU does not include. */
    extern char data_ov030_02115ddc[];
    typedef struct M4x3 { int w[12]; } M4x3;


    struct Bundle { Vector3_16 rot; short _p; Vector3 trans; int _tail[2]; };

    extern int _ZN6Player14IsFrontSlidingEv(void*);
    extern int _ZN6Player17LostGrabbedObjectEv(void*);
    extern void* _ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3(void*, void*, void*);
    extern void Matrix4x3_FromRotationY(void* m, int angle);
    extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        void* self, void* shadow, void* mtx, int rad, int height, unsigned int flags);
    extern void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
    extern void MulMat4x3Mat4x3(void* a, void* b, void* out);
    extern void Matrix4x3_ApplyInPlaceToTranslation(void* m, int x, int y, int z);
    extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
    extern M4x3 data_020a0e68;
    char* c = (char*)this;
    int idx;
    void* res;
    void* obj;
    unsigned int id;
    Bundle bnd;

    int a = (int)((this->mFlags & 0x100) != 0);
    if (a && *(void**)(c + 0x3a8)
        && *(int*)(*(char**)(c + 0x3a8) + 0xc8)) {
        idx = 0;
        if (_ZN6Player14IsFrontSlidingEv(*(void**)(c + 0x3a8))
            || _ZN6Player17LostGrabbedObjectEv(*(void**)(c + 0x3a8))) {
            idx = 1;
        }
        if (*(int*)(*(char**)(c + 0x3a8) + 8) == 2) {
            idx = (idx + 2) & 0xff;
        }
        res = _ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3(c, *(void**)(c + 0x3a8),
            data_ov030_02115ddc + idx * 0xc);
        *(M4x3*)(&this->mModelAnim.mat4x3) = *(M4x3*)res;
    } else {
        Matrix4x3_FromRotationY(&this->mModelAnim.mat4x3, this->mAngleY);
        this->mModelAnim.mat4x3.t.x = this->mPosX >> 3;
        this->mModelAnim.mat4x3.t.y = this->mPosY >> 3;
        this->mModelAnim.mat4x3.t.z = this->mPosZ >> 3;
    }

    int b = (int)((this->mFlags & 0x40000) != 0);
    if (!b) {
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, &this->mShadowModel, &this->mModelAnim.mat4x3, 0x5a000, 0x190000, 0xf);
    }

    id = this->mCapUniqueID;
    if (id == 0)
        return;

    obj = dActor_c::FindWithID(id);

    bnd.trans.x = 0xa00;
    bnd.trans.y = 0;
    bnd.trans.z = -0x2f00;
    bnd.rot.x = -0x3f00;
    bnd.rot.y = 0;
    bnd.rot.z = -0x4000;

    data_020a0e68 = *(M4x3*)(&this->mModelAnim.mat4x3);
    MulMat4x3Mat4x3(*(char**)((char *)&this->mModelAnim.data.transforms) + 0xf0, &data_020a0e68, &data_020a0e68);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68,
        *(volatile int*)&bnd.trans.x, *(volatile int*)&bnd.trans.y, *(volatile int*)&bnd.trans.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(volatile short*)&bnd.rot.x, *(volatile short*)&bnd.rot.y, *(volatile short*)&bnd.rot.z);
    *(M4x3*)(this->mCapMtx) = data_020a0e68;

    *(int*)((char*)obj + 0xc8) = (int)(this->mCapMtx);
    *(int*)((char*)obj + 0x5c) = this->mPosX;
    *(int*)((char*)obj + 0x60) = this->mPosY;
    *(int*)((char*)obj + 0x64) = this->mPosZ;
}
// @symbol _ZN7daMky_c19func_ov030_021122b0Ev
int daMky_c::func_ov030_021122b0()
{
    extern void _Z14ApproachLinearRsss(short *dst, short target, short rate);
    extern unsigned char DecIfAbove0_Byte(unsigned char *p);
    char *s = (char*)this;
    short ang = this->HorzAngleToCPlayer() + 0x8000;
    _Z14ApproachLinearRsss((short*)(s + 0x8e), ang, 0xa28);
    this->mPrevAngleY = this->mAngleY;
    if (DecIfAbove0_Byte((unsigned char*)(s + 0x3c6)) == 0)
        func_ov030_021141a8(0);
    ((Animation *)(s + 0x124))->Advance();
    int b = (int)(this->actorID == 0x10b);
    if (b) {
        this->UpdatePos((dCc_c*)(&this->mdCcAc_c));
        func_ov030_02111dd0();
        func_ov030_02111f6c(&this->mWithMeshClsn);
        func_ov030_02111bc4();
    } else {
        this->UpdatePos((dCc_c*)(&this->mdCcAc_c));
        func_ov030_02111f6c(&this->mWithMeshClsn);
        func_ov030_02111bc4();
        func_ov030_02111ea4();
    }
    this->mdCcAc_c.Clear();
    this->mdCcAc_c.Update();
    func_ov030_02111890();
    return 1;
}
// @symbol _ZN7daMky_c12EnterState10Ev
int daMky_c::EnterState10(){
    struct S { int w[2]; };
    extern struct S data_ov030_02115d18;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void*)data_ov030_02115d18.w[1], 0, 0x1000, 0);
  this->mModelAnim.speed = 0x1000;
  mHorzSpeed = 0x13000;
  mActionTimer = 0x1e;
  mState = 0xa;
  return 1;
}

// @symbol _ZN7daMky_c19func_ov030_02112400Ev
int daMky_c::func_ov030_02112400()
{
    typedef void* (*Vfn)();

    struct Vector3;

    extern int Vec3_Dist(const Vector3* a, const Vector3* b);
    extern void* data_02099368[];


    typedef struct { int a, b; } P2;
    struct ClsnResultTmp {
        Vfn* vtb;
        P2 v;
        int a2, a3, a4;
        u16 h0, h1;
        int t0, t1, t2;
    };
    extern char* func_0203567c(dBgCh_Actr* w);
    extern int func_02037f44(ClsnResultTmp* r);
    extern unsigned _ZNK5dBgPi9GetClsnIDEv(const ClsnResultTmp* r);
    extern void _ZN5dBgPiD1Ev(ClsnResultTmp* r);
    func_ov030_02111a00();
    ((Animation *)((char *)this + 0x124))->Advance();
    this->UpdatePos(&this->mdCcAc_c);
    func_ov030_02111f6c((dBgCh_Actr*)(&this->mWithMeshClsn));
    func_ov030_02111bc4();
    this->mdCcAc_c.Clear();
    this->mdCcAc_c.Update();

    int b = (int)(this->actorID == 0x10c);
    if (b != 0) {
        if (Vec3_Dist((Vector3 *)&this->mPerchPosX, (Vector3 *)&this->mPosX) < 0x514000) {
            if (this->mPosY > this->mPerchPosY - 0x12c000) {
                func_ov030_021141a8(1);
            }
        }
    } else {
        if (this->mWithMeshClsn.IsOnGround()) {
            char* r = func_0203567c((dBgCh_Actr*)(&this->mWithMeshClsn));
            ClsnResultTmp res;
            int* d = (int*)&res.v;
            *(double*)d = *(double*)(r + 4);
            d[2] = *(int*)(r + 0xc);
            d[3] = *(int*)(r + 0x10);
            d[4] = *(int*)(r + 0x14);
            res.vtb = (Vfn*)data_02099368;
            res.h0 = *(u16*)(r + 0x18);
            res.h1 = *(u16*)(r + 0x1a);
            res.t0 = *(int*)(r + 0x1c);
            res.t1 = *(int*)(r + 0x20);
            res.t2 = *(int*)(r + 0x24);
            if (_ZNK5dBgPi9GetClsnIDEv(&res) == -1 || func_02037f44(&res) == 0) {
                func_ov030_021141a8(0);
            }
            _ZN5dBgPiD1Ev(&res);
        }
    }
    return 1;
}
// @symbol _ZN7daMky_c11EnterState9Ev
int daMky_c::EnterState9()
{
    mHorzSpeed = 0;
    mState = 9;
    return 1;
}

// @symbol _ZN7daMky_c19func_ov030_02112578Ev
int daMky_c::func_ov030_02112578()
{
    void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *bca, int a, int fx, unsigned int f);
    s16 Vec3_HorzAngle(const void *v0, const void *v1);
    int _Z14ApproachLinearRsss(s16 *v, s16 target, s16 step);
    s32 Vec3_Dist(const void *a, const void *b);
    int _ZN6Player9StartTalkER7fBase_cb(void *player, void *actor, int b);
    int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void *player, void *actor, unsigned int id, const void *pos, unsigned int a, unsigned int b);
    void _ZN9Animation8SetFlagsEi(void *thiz, int flags);
    void func_0201267c(unsigned int id, const Vector3 *pos);
    int _ZN6Player12GetTalkStateEv(void);
    void _ZN6Player18HasFinishedTalkingEv(void *player);
    int _ZNK10dBgCh_Actr13JustHitGroundEv(const void *thiz);
    int _ZN9Animation8FinishedEv(void *thiz);
    bool _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int fx, int e);
    u8 DecIfAbove0_Byte(u8 *p);
    void _ZN7fBase_c18MarkForDestructionEv(void *thiz);
    void _ZN9Animation7AdvanceEv(void *thiz);
    void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *clsn);

    extern void *data_ov030_02115cd0[];
    extern void *data_ov030_02115cf8[];
    extern void *data_ov030_02115d08[];
    extern void *data_ov030_02115d10[];
    extern void *data_ov030_02115d18[];
    u8 *c = (u8 *)this;
    void *cage = dActor_c::FindWithActorID(0x67, 0);
    void *player = ((dActor_c *)this)->ClosestPlayer();
    s32 v[3];
    *(s32 *)((u8 *)v + 0) = 0x981;
    *(s32 *)((u8 *)v + 4) = 0x77a;
    *(s32 *)((u8 *)v + 8) = 0x501;

    switch (this->unk_3c7) {
    case 0:
        if (func_ov030_02111b20() != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&this->mModelAnim, data_ov030_02115cd0[1], 0, 0x1000, 0);
            this->mModelAnim.speed = 0x1000;
            this->mHorzSpeed = 0;
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        func_ov030_02111890();
        break;
    case 1:
        _Z14ApproachLinearRsss((s16 *)(c + 0x8e), Vec3_HorzAngle((Vector3 *)&this->mPosX, (u8 *)player + 0x5c), 0x300);
        if (Vec3_Dist((Vector3 *)&this->mPosX, (u8 *)player + 0x5c) < 0x96000) {
            if (_ZN6Player9StartTalkER7fBase_cb(player, this, 1) != 0) {
                { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
            }
        }
        func_ov030_02111908();
        break;
    case 2: {
        s32 sp[3];
        sp[0] = this->mPosX;
        sp[1] = this->mPosY;
        sp[2] = this->mPosZ;
        sp[1] += 0x50000;
        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(player, this, 0xbd, sp, 1, 0) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&this->mModelAnim, data_ov030_02115cf8[1], 0, 0x1000, 0);
            ((Animation *)(c + 0x124))->SetFlags( 0);
            func_0201267c(0xd1, (const Vector3 *)&this->mCamSpacePosX);
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        break;
    }
    case 3:
        if (_ZN6Player12GetTalkStateEv() == 2) {
            _ZN6Player18HasFinishedTalkingEv(player);
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&this->mModelAnim, data_ov030_02115d18[1], 0, 0x1000, 0);
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        break;
    case 4:
        if (_Z14ApproachLinearRsss((s16 *)(c + 0x8e), (s16)0xffffe04e, 0x400) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&this->mModelAnim, data_ov030_02115d08[1], 0x40000000, 0x1000, 0);
            this->mPrevAngleY = this->mAngleY;
            this->mHorzSpeed = 0xf000;
            this->mVertSpeed = 0x2f000;
            func_0201267c(0xf1, (const Vector3 *)&this->mCamSpacePosX);
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        break;
    case 5:
        if (this->mWithMeshClsn.JustHitGround() != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&this->mModelAnim, data_ov030_02115d10[1], 0x40000000, 0x1000, 0);
            this->mHorzSpeed = 0;
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        break;
    case 6:
        if (((Animation *)(c + 0x124))->Finished() != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&this->mModelAnim, data_ov030_02115cd0[1], 0, 0x1000, 0);
            this->mModelAnim.speed = 0x1000;
            *(s32 *)((u8 *)cage + 0x98) = 0x400;
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x14, 0x7f, 0x15666, 0);
            this->mActionTimer = 0x78;
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        /* fallthrough */
    case 7: {
        s32 *pp = (s32 *)((unsigned int)c + 0x3bc);
        *pp = *pp + 0x400;
        if (this->unk_3bc > 0x17ffd) {
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
    }
        /* fallthrough */
    case 8:
        if (DecIfAbove0_Byte((u8 *)((unsigned int)c + 0x3c6)) == 0) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x7f, 0, 0x15666, 0);
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        break;
    case 9:
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&this->mModelAnim, data_ov030_02115d08[1], 0x40000000, 0x1000, 0);
        *(s32 *)((u8 *)cage + 0x9c) = -0x2000;
        *(s32 *)((u8 *)cage + 0xa0) = -0x3c000;
        { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        break;
    case 10:
        if (dActor_c::FindWithActorID(0x67, 0) == 0) {
            _ZN7fBase_c18MarkForDestructionEv(this);
        }
        break;
    default:
        break;
    }

    ((Animation *)(c + 0x124))->Advance();
    _ZN8dActor_c9UpdatePosEP5dCc_c(this, &this->mdCcAc_c);
    func_ov030_02111f6c(&this->mWithMeshClsn);
    this->mdCcAc_c.Clear();
    return 1;
}
// @symbol _ZN7daMky_c11EnterState8Ev
int daMky_c::EnterState8() {
    char *c = (char *)this;
    struct G { void *a; void *b; };
    extern struct G data_ov030_02115d18;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&this->mModelAnim, data_ov030_02115d18.b, 0, 0x1000, 0);
    this->mModelAnim.speed = 0x1000;
    ((PathPtr *)(&this->mPathPtr))->FromID(*(int*)(c+8) & 0xff);
    mPathNode = 1;
    unk_3c7 = 0;
    this->mHorzSpeed = 0x6000;
    mState = 8;
    return 1;
}

// @symbol _ZN7daMky_c19func_ov030_02112a84Ev
int daMky_c::func_ov030_02112a84()
{
    typedef struct P2 { int w[2]; } P2;  /* array member: C++ would scalarise a two-int struct copy that C block-moved */

    typedef struct dBgPi {
        void *vtb;
        int s0, s1, s2, s3, s4;
        u16 f, g;
        int h, i, j;
    } dBgPi;

    extern void dBgCh_Actr_UpdateContinuous_Veneer(void *p);
    extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void *p);
    extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *p);
    extern void *func_0203567c(void *p);
    extern u32 _ZNK5dBgPi9GetClsnIDEv(void *r);
    extern void _ZN5dBgPiD1Ev(void *r);
    extern int Vec3_Dist(void *a, void *b);
    extern void _ZN9Animation7AdvanceEv(void *p);
    extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *a, void *c);
    extern void *data_02099368[];
    dBgPi res;

    dBgCh_Actr_UpdateContinuous_Veneer(&this->mWithMeshClsn);
    if (this->mWithMeshClsn.JustHitGround() || this->mWithMeshClsn.IsOnGround()) {
        int b;
        u16 id;

        b = 0;
        this->mVertSpeed = 0;
        id = this->actorID;
        if (id == 0x10b)
            b = 1;
        if (b) {
            char *r = (char *)func_0203567c(&this->mWithMeshClsn);
            int *d = &res.s0;
            *(P2 *)d = *(P2 *)(r + 4);
            d[2] = *(int *)(r + 0xc);
            d[3] = *(int *)(r + 0x10);
            d[4] = *(int *)(r + 0x14);
            res.vtb = data_02099368;
            res.f = *(u16 *)(r + 0x18);
            res.g = *(u16 *)(r + 0x1a);
            res.h = *(int *)(r + 0x1c);
            res.i = *(int *)(r + 0x20);
            res.j = *(int *)(r + 0x24);
            if (_ZNK5dBgPi9GetClsnIDEv(&res) != 0xffffffff)
                func_ov030_021141a8(9);
            else
                func_ov030_021141a8(this->mPrevState);
            _ZN5dBgPiD1Ev(&res);
        } else {
            int t = (int)(id == 0x10c);
            if (t != 0) {
                if (Vec3_Dist((Vector3 *)&this->mPerchPosX, (Vector3 *)&this->mPosX) < 0x514000
                    && this->mPosY > this->mPerchPosY - 0x12c000) {
                    func_ov030_021141a8(this->mPrevState);
                } else {
                    func_ov030_021141a8(9);
                }
            }
        }
    }

    ((Animation *)((char *)this + 0x124))->Advance();
    this->UpdatePos(&this->mdCcAc_c);
    func_ov030_02111bc4();
    this->mdCcAc_c.Clear();
    return 1;
}
// @symbol _ZN7daMky_c11EnterState7Ev
int daMky_c::EnterState7()
{
    void *self = (void *)this;
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef short s16;
    typedef long long s64;
    struct Vector3
    {
      int x;
      int y;
      int z;
    };
    extern short data_02082214[];
    extern void *data_ov030_02115d08[];
  u8 *new_var;
  u8 *c = (u8 *) self;
  int *pos;
  int *py;
  int *pz;
  struct Vector3 v;
  u8 *other;
  u16 ang;
  s16 s;
  int mul = 0x4b000;
  int rnd = 0x800;
  mFlags &= ~0x80000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&this->mModelAnim, data_ov030_02115d08[1], 0x40000000, 0x1000, 0);
  *((int *) (c + 0x98)) = 0xa000;
  *((int *) (c + 0xa8)) = 0;
  other = *((u8 **) (c + 0x3a8));
  pos = (int *) ((int) (((s64) ((int) ((Vector3 *)&this->mPosX)))));
  s = *((s16 *) (other + 0x8e));
  *((s16 *) (c + 0x8e)) = s;
  s = *((s16 *) (c + 0x8e));
  py = (int *) ((int) (((s64) ((int) (c + 0x60)))));
  pz = (int *) ((int) (((s64) ((int) (c + 0x64)))));
  *((s16 *) (c + 0x94)) = s;
  other = *((u8 **) (c + 0x3a8));
  {
    int *op = (int *) ((int) (((s64) ((int) (other + 0x5c)))));
    int t0 = op[0];
    *((int *) ((Vector3 *)&this->mPosX)) = t0;
    int t1 = op[1];
    *((int *) (c + 0x60)) = t1;
    int t2 = op[2];
    *((int *) (c + 0x64)) = t2;
  }
  ang = *((u16 *) (c + 0x8e));
  s = data_02082214[(ang >> 4) * 2];
  *pos = (*pos) + ((int) ((((((s64) s) * mul) + rnd) >> 1) >> 11));
  *py = (*py) + 0x50000;
  ang = *((u16 *) (c + 0x8e));
  s = data_02082214[((ang >> 4) * 2) + 1];
  *pz = (*pz) + ((int) (((((s64) s) * mul) + rnd) >> 12));
  other = *((u8 **) (c + 0x3a8));
  {
    int oy = *((int *) (other + 0x60));
    int oz = *((int *) (new_var = other + 0x64));
    mul = oy + 0x50000;
    v.x = *((int *) (other + 0x5c));
    v.y = mul;
    v.z = oz;
  }
  ((dActor_c *)c)->DetectRaycastClsn(*(::Vector3 *)&v, *(::Vector3 *)pos, 1);
  *((int *) (c + 0xd0)) = 0;
  *((int *) (c + 0x3a8)) = 0;
  *((int *) (c + 0x3b4)) = 7;
  return 1;
}

// @symbol _ZN7daMky_c19func_ov030_02112da0Ev
int daMky_c::func_ov030_02112da0()
{
    extern int Vec3_Dist(void *a, void *b);
    extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void *a, void *self, unsigned int, void *, unsigned int, unsigned int);
    extern void func_0201267c(unsigned int id, const Vector3 *pos);
    extern int _ZN6Player12GetTalkStateEv(void *p);
    extern int _ZN6Player9DropActorEv(void *p);
    extern u8 DecIfAbove0_Byte(u8 *p);
    extern u8 data_0209d684;
    int b = (int)((this->mFlags & 0x40000) != 0);
    if (b != 0) {
        int p = (int)(*(char **)((char *)this + 0x3a8) + 0x5c);
        this->mPosX = *(int *)p;
        this->mPosY = *(int *)(p + 4);
        this->mPosZ = *(int *)(p + 8);
    }

    {
        u32 flags = this->mFlags;
        b = (int)((flags & 0x80000) != 0);
        if (b != 0) {
            this->mPrevState = 1;
            func_ov030_021141a8(7);
            return 1;
        }

        switch (this->unk_3c7) {
        case 0: {
            int b2 = (int)((flags & 0x40000) != 0);
            if (b2 != 0) {
                char *s = *(char **)((char *)this + 0x3a8);
                int off = 0x3c7;
                int *p = (int *)(s + 0x5c);
                int x = *p;
                u8 *st = (u8 *)((int)(char *)this + off);
                this->mPosX = x;
                this->mPosY = p[1];
                this->mPosZ = p[2];
                (*st)++;
            } else {
                int b3 = (int)((flags & 0x20000) != 0);
                if (b3 != 0) break;
                if (b2 != 0) break;
                *(int *)((char *)this + 0xd0) = 0;
                func_ov030_021141a8(this->mPrevState);
            }
            break;
        }
        case 1:
            if (Vec3_Dist((Vector3 *)&this->mPerchPosX, (Vector3 *)&this->mPosX) < 0x514000 &&
                this->mPosY > this->mPerchPosY - 0x12c000) {
                if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(char **)((char *)this + 0x3a8), (char *)this, 0xc1, 0, 0, 0) != 0) {
                    func_0201267c(0xd1, (const Vector3 *)&this->mCamSpacePosX);
                    (*(u8 *)((int)(char *)this + 0x3c7))++;
                }
            }
            {
                char *s = *(char **)((char *)this + 0x3a8);
                u8 val = 0x3c;
                int *p = (int *)(s + 0x5c);
                this->mPosX = *p;
                this->mPosY = p[1];
                this->mPosZ = p[2];
                this->mActionTimer = val;
            }
            break;
        case 2:
            if (_ZN6Player12GetTalkStateEv(*(char **)((char *)this + 0x3a8)) == -1) {
                u8 g = data_0209d684;
                if (g == 1) {
                    _ZN6Player9DropActorEv(*(char **)((char *)this + 0x3a8));
                    this->mPrevState = 8;
                    func_ov030_021141a8(7);
                } else if (g == 2) {
                    (*(u8 *)((int)(char *)this + 0x3c7))++;
                }
            }
            break;
        case 3:
            if (DecIfAbove0_Byte((u8 *)((int)(char *)this + 0x3c6)) == 0) {
                this->unk_3c7 = 1;
            }
            break;
        }
    }
    return 1;
}
// @symbol _ZN7daMky_c11EnterState6Ev
int daMky_c::EnterState6()
{
    char *c = (char *)this;
    this->mFlags &= ~0x80000;
    if (Vec3_Dist((Vector3 *)&this->mPerchPosX, (Vector3 *)&this->mPosX) < 0x514000 &&
        mPosY > mPerchPosY - 0x12c000) {
        unk_3c7 = 0;
        ((dActor_c *)c)->SpawnSoundObj(1);
    } else {
        unk_3c7 = 3;
    }
    mHorzSpeed = 0;
    mActionTimer = 0x3c;
    this->mdCcAc_c.Clear();
    mPrevState = mState;
    mState = 6;
    return 1;
}

// @symbol _ZN7daMky_c19func_ov030_02113094Ev
int daMky_c::func_ov030_02113094()
{
    struct dActor_c;


    extern struct dActor_c* _ZN8dActor_c10FindWithIDEj(u32 id);
    extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(char* p, char* self, u32 msg, const struct Vector3* pos, u32 a, u32 b);
    extern void func_0201267c(unsigned int id, const Vector3 *pos);
    extern int _ZN6Player12GetTalkStateEv(char* p);
    extern int _ZN6Player9DropActorEv(char* p);
    extern struct dActor_c* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 id, u32 param, const struct Vector3* pos, const struct Vector3_16* rot, int a, int b);
    extern void _ZN7fBase_c18MarkForDestructionEv(char* self);
    {
        int b = (int)((this->mFlags & 0x40000) != 0);
        if (b != 0) {
            int p = (int)((((int)*(char**)((char *)this + 0x3a8)) + 0x5c));
            this->mPosX = *(int*)p;
            this->mPosY = *(int*)(p + 4);
            this->mPosZ = *(int*)(p + 8);
        }
    }

    switch (this->unk_3c7) {
    case 0: {
        int b2 = (int)((this->mFlags & 0x40000) != 0);
        if (b2 != 0) {
            if (this->mHasSpawnedCap != 0) {
                struct dActor_c* a = (struct dActor_c *)::dActor_c::FindWithID(this->mCapUniqueID);
                *(char**)((char*)a + 0xd0) = *(char**)((char *)this + 0x3a8);
                *(u32*)(((int)a + 0xb0)) |= 0x40000;
            }
            (*(u8*)(((int)(char *)this + 0x3c7)))++;
        } else {
            int b3 = (int)((this->mFlags & 0x20000) != 0);
            if (b3 != 0) break;
            if (b2 != 0) break;
            *(int*)((char *)this + 0xd0) = 0;
            func_ov030_021141a8(this->mPrevState);
        }
        break;
    }
    case 1: {
        int msg = (this->mHasSpawnedCap != 0) ? 0xc2 : 0xc3;
        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(char**)((char *)this + 0x3a8), (char *)this, (s16)msg, 0, 0, 0) != 0) {
            func_0201267c(0xd1, (const Vector3 *)&this->mCamSpacePosX);
            (*(u8*)(((int)(char *)this + 0x3c7)))++;
        }
        {
            int b4 = (int)((this->mFlags & 0x80000) != 0);
            if (b4 != 0) {
                func_ov030_021141a8(7);
            }
        }
        break;
    }
    case 2:
        if (_ZN6Player12GetTalkStateEv(*(char**)((char *)this + 0x3a8)) == -1) {
            _ZN6Player9DropActorEv(*(char**)((char *)this + 0x3a8));
            (*(u8*)(((int)(char *)this + 0x3c7)))++;
        }
        break;
    case 3: {
        int b5 = (int)((this->mFlags & 0x80000) != 0);
        if (b5 != 0) {
            if (this->mHasSpawnedCap != 0) {
                _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x10d, (this->mCapPlayerNo << 8) | 5, (struct Vector3*)(*(char**)((char *)this + 0x3a8) + 0x5c), 0, this->mAreaId, -1);
                ((fBase_c *)::dActor_c::FindWithID(this->mCapUniqueID))->MarkForDestruction();
                {
                    u32 z = 0;
                    this->mCapUniqueID = z;
                    this->mHasSpawnedCap = (u8)z;
                    this->mPrevState = 0xa;
                }
            }
            func_ov030_021141a8(7);
        }
        break;
    }
    }

    return 1;
}
// @symbol _ZN7daMky_c11EnterState5Ev
int daMky_c::EnterState5() {
    char* c = (char*)this;
    int* p = (int*)((int)c + 0xb0);
    int tmp = *p;
    *p = tmp & ~0x80000;
    unk_3c7 = 0;
    void* clsn = (void*)(&this->mdCcAc_c);
    this->mHorzSpeed = 0;
    ((dCc_c *)clsn)->Clear();
    this->mWithMeshClsn.ClearGroundFlag();
    mPrevState = mState;
    mState = 5;
    return 1;
}

// @symbol _ZN7daMky_c19func_ov030_02113324Ev
int daMky_c::func_ov030_02113324()
{
    extern int Vec3_Dist(const Vector3* a, const Vector3* b);
    extern int _ZN6Player9StartTalkER7fBase_cb(void* self, void* actor, int b);
    extern short Vec3_HorzAngle(const Vector3* v0, const Vector3* v1);
    extern void Matrix4x3_FromTranslation(Matrix4x3* m, int x, int y, int z);
    extern void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3* m, short angY);
    extern void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3* m, int x, int y, int z);
    extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void* self, void* actor, unsigned int msgId, const Vector3* pos, unsigned int d, unsigned int e);
    extern void func_0201267c(unsigned int id, const Vector3 *pos);
    extern int _ZN6Player12GetTalkStateEv(void* self);
    extern int _ZN6Player9DropActorEv(void* self);
    extern void _ZN9Animation7AdvanceEv(void* self);
    extern unsigned char DecIfAbove0_Byte(unsigned char* p);

    extern void* data_0209f318;
    extern Matrix4x3 data_020a0e68;
    extern unsigned char data_0209d684;
    char *c = (char*)this;

    this->mAngleY = *(short*)((char*)(*(void**)(c + 0x3a8)) + 0x8e);
    this->mPrevAngleY = this->mAngleY;

    {
        unsigned int flags = this->mFlags;
        int f1 = (flags & 0x100) != 0;
        if (!f1)
            goto do_raycast;
        {
            int f2 = (flags & 0x2000) != 0;
            if (f2)
                goto do_raycast;
        }
    }
    goto skip_raycast;
do_raycast:
    {
        char *other = (char*)(*(void**)(c + 0x3a8));
        Vector3 v;
        int oy = *(int*)(other + 0x60);
        int oz = *(int*)(other + 0x64);
        int vy = oy + 0x32000;
        v.x = *(int*)(other + 0x5c);
        v.y = vy;
        v.z = oz;
        ((dActor_c *)c)->DetectRaycastClsn(v, *(Vector3 *)&this->mPosX, 1);

        if (Vec3_Dist((Vector3 *)&this->mPerchPosX, (Vector3 *)&this->mPosX) < 0x514000 &&
            this->mPosY > this->mPerchPosY - 0x12c000) {
            func_ov030_021141a8(1);
        } else {
            func_ov030_021141a8(9);
        }
        *(void**)(c + 0x3a8) = 0;
        return 1;
    }
skip_raycast:
    switch (this->unk_3c7) {
    case 0:
        if (Vec3_Dist((Vector3 *)&this->mPerchPosX, (Vector3 *)&this->mPosX) < 0x514000 &&
            this->mPosY > this->mPerchPosY - 0x12c000) {
            if (_ZN6Player9StartTalkER7fBase_cb(*(void**)(c + 0x3a8), c, 1) != 0) {
                Vector3 camPos;
                {
                    Vector3 *src = (Vector3*)((char*)data_0209f318 + 0x8c);
                    camPos.x = src->x;
                    camPos.y = src->y;
                    camPos.z = src->z;
                }
                short ang = Vec3_HorzAngle(&camPos, (Vector3*)((char*)(*(void**)(c + 0x3a8)) + 0x5c));
                {
                    Vector3 *op = (Vector3*)((char*)(*(void**)(c + 0x3a8)) + 0x5c);
                    Matrix4x3_FromTranslation(&data_020a0e68, op->x, op->y, op->z);
                }
                Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, ang);
                Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, 0, -0x64000);

                Vector3 msgPos;
                msgPos.x = data_020a0e68.t.x;
                msgPos.y = data_020a0e68.t.y;
                msgPos.z = data_020a0e68.t.z;
                msgPos.y = this->mPosY + 0x64000;

                if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(void**)(c + 0x3a8), c, 0xc0, &msgPos, 0, 2) != 0) {
                    func_0201267c(0xd1, (const Vector3 *)&this->mCamSpacePosX);
                    {
                        unsigned char *p = (unsigned char*)(c + 0x3c7);
                        (*p)++;
                    }
                }
            }
        }
        this->mActionTimer = 0x3c;
        break;
    case 1:
        if (_ZN6Player12GetTalkStateEv(*(void**)(c + 0x3a8)) == -1) {
            unsigned char g = data_0209d684;
            if (g == 1) {
                _ZN6Player9DropActorEv(*(void**)(c + 0x3a8));
                func_ov030_021141a8(8);
            } else if (g == 2) {
                unsigned char *p = (unsigned char*)(c + 0x3c7);
                (*p)++;
            }
        }
        break;
    case 2:
        if (DecIfAbove0_Byte((unsigned char*)(c + 0x3c6)) == 0)
            this->unk_3c7 = 0;
        break;
    }

    ((Animation *)(c + 0x124))->Advance();
    this->mdCcAc_c.Clear();
    return 1;
}
// @symbol _ZN7daMky_c11EnterState4Ev
int daMky_c::EnterState4(){
    char* c = (char*)this;
    extern int data_ov030_02115ce0[];
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&this->mModelAnim, (void *)data_ov030_02115ce0[1], 0, 0x1000, 0);
    this->mModelAnim.speed = 0x1000;
    if (Vec3_Dist((Vector3 *)&this->mPerchPosX, (Vector3 *)&this->mPosX) < 0x514000
        && this->mPosY > mPerchPosY - 0x12c000) {
        unk_3c7 = 0;
        ((dActor_c *)c)->SpawnSoundObj(1);
    } else {
        unk_3c7 = 2;
    }
    mActionTimer = 0x3c;
    mState = 4;
    return 1;
}

// @symbol _ZN7daMky_c19func_ov030_021136b0Ev
int daMky_c::func_ov030_021136b0()
{
    typedef short s16;
    typedef signed char s8;
    typedef unsigned char u8;
    typedef unsigned int u32;
    typedef int s32;


    extern void _ZN8SaveData13PlayerLoseCapEv(void);
    /* The arm9 symbols.txt row spells this Vector3_16 (not Vector3s) — a wrong mangling here left the reloc BLIND and broke mwldarm. */
    extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        u32 actorID, u32 param1, const Vector3 *pos,
        const void *rot, int areaID, int deathTableID);
    extern unsigned int func_02012790(unsigned int arg);
    extern s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
    extern void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
    extern void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, s16 angY);
    extern void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *m, int x, int y, int z);
    extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
        void *self, void *actor, unsigned int msgId, const Vector3 *pos,
        unsigned int d, unsigned int e);
    extern void func_0201267c(unsigned int id, const Vector3 *pos);
    extern int _ZN6Player12GetTalkStateEv(void *self);
    extern int _ZN6Player9DropActorEv(void *self);
    extern void _ZN6Player18SetNewHatCharacterEjjb(void *self, unsigned int a, unsigned int b, int c);
    extern void *_ZN8dActor_c10FindWithIDEj(u32 id);
    extern void _ZN9Animation7AdvanceEv(void *self);

    extern void *data_0209f318;
    extern Matrix4x3 data_020a0e68;
    int msg;
    s16 a = *(s16 *)(*(char **)((char *)this + 0x3a8) + 0x8e);
    this->mAngleY = a;
    this->mPrevAngleY = this->mAngleY;

    switch (this->unk_3c7) {
    case 0:
        if (this->mHasSpawnedCap != 0) {
            {
                Player *p = (Player *)*(char **)((char *)this + 0x3a8);
                int t = (p->mCharacter == p->param1);
                t = (t != 0);
                this->pad_3c9 = t;
            }
            if (this->pad_3c9 == 0) {
                Player *p = (Player *)*(char **)((char *)this + 0x3a8);
                p->mHasNoCap = 1;
            } else {
                SaveData::PlayerLoseCap();
            }
            {
                Player *p = (Player *)*(char **)((char *)this + 0x3a8);
                void *spawned;
                this->mCapPlayerNo = p->param1;
                msg = this->mAreaId;
                spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0x10d,
                    (this->mCapPlayerNo << 8) | 2,
                    (Vector3 *)&this->mPosX,
                    0,
                    msg,
                    -1);
                this->mCapUniqueID = ((u32 *)spawned)[1];
            }
        } else {
            if (this->pad_3c9 != 0)
                func_02012790(0xa);
        }
        {
            u8 *st = (u8 *)((char *)this + 0x3c7);
            (*st)++;
        }
        /* fall through */
    case 1: {
        s16 ang;
        Vector3 camPos;
        Vector3 msgPos;
        {
            u8 fl = this->mHasSpawnedCap;
            msg = fl ? 0xbe : 0xbf;
            void *camBase = data_0209f318;
            Vector3 *src = (Vector3 *)((char *)camBase + 0x8c);
            camPos.x = src->x;
            camPos.y = src->y;
            camPos.z = src->z;
            ang = Vec3_HorzAngle(&camPos, (Vector3 *)(*(char **)((char *)this + 0x3a8) + 0x5c));
        }
        {
            int *op = (int *)(*(char **)((char *)this + 0x3a8) + 0x5c);
            Matrix4x3_FromTranslation(&data_020a0e68, op[0], op[1], op[2]);
        }
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, ang);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, 0, -0x64000);
        {
            int msgArg = ((int)msg << 16) >> 16;
            int my = data_020a0e68.t.y;
            int mx = data_020a0e68.t.x;
            int mz = data_020a0e68.t.z;
            msgPos.x = mx;
            msgPos.y = my;
            msgPos.z = mz;
            msgPos.y = this->mPosY + 0x64000;
            if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
                    *(void **)((char *)this + 0x3a8), (char *)this, msgArg, &msgPos, 0, 2) != 0) {
                func_0201267c(0xd1, (const Vector3 *)&this->mCamSpacePosX);
                {
                    u8 *st = (u8 *)((char *)this + 0x3c7);
                    (*st)++;
                }
            }
        }
        break;
    }
    case 2:
        if (_ZN6Player12GetTalkStateEv(*(void **)((char *)this + 0x3a8)) == -1) {
            if (this->mHasSpawnedCap != 0) {
                {
                    u32 *fl = (u32 *)((char *)this + 0xb0);
                    *fl &= ~0x200u;
                }
                _ZN6Player9DropActorEv(*(void **)((char *)this + 0x3a8));
                {
                    u32 *fl = (u32 *)((char *)this + 0xb0);
                    *fl |= 0x200u;
                }
            } else {
                _ZN6Player9DropActorEv(*(void **)((char *)this + 0x3a8));
            }
            {
                u8 *st = (u8 *)((char *)this + 0x3c7);
                (*st)++;
            }
        }
        break;
    case 3: {
        int f1 = (int)((this->mFlags & 0x100) != 0);
        if (f1 == 0) {
            if (this->mHasSpawnedCap != 0) {
                if (this->pad_3c9 == 0) {
                    Player *p = (Player *)*(char **)((char *)this + 0x3a8);
                    p->SetNewHatCharacter(p->mCharacter, 0, 0);
                }
                this->mPrevState = 1;
                func_ov030_021141a8(2);
            } else {
                void *act = dActor_c::FindWithID(this->mCapUniqueID);
                int z = 0;
                *(int *)((char *)act + 0xc8) = z;
                {
                    char *p = *(char **)((char *)this + 0x3a8);
                    int *src = (int *)(p + 0x5c);
                    *(int *)((char *)act + 0x5c) = src[0];
                    *(int *)((char *)act + 0x60) = src[1];
                    *(int *)((char *)act + 0x64) = src[2];
                }
                this->mCapUniqueID = (u32)z;
                func_ov030_021141a8(0xa);
            }
            *(void **)((char *)this + 0x3a8) = 0;
        }
        break;
    }
    case 4: {
        Player *p = (Player *)*(char **)((char *)this + 0x3a8);
        if (p->mIsMetal == 0 &&
            p->mIsVanish == 0 &&
            p->mHasWings == 0) {
            this->unk_3c7 = 0;
            {
                u8 *f = (u8 *)((char *)this + 0x3c8);
                *f ^= 1;
            }
            break;
        }
        /* fall through */
    }
    case 5: {
        u32 flags = this->mFlags;
        int f1 = (int)((flags & 0x100) != 0);
        if (f1 != 0) {
            int f2 = (int)((flags & 0x2000) != 0);
            if (f2 == 0)
                break;
        }
        func_ov030_021141a8(0xa);
        *(void **)((char *)this + 0x3a8) = 0;
        break;
    }
    default:
        break;
    }

    ((Animation *)((char *)this + 0x124))->Advance();
    this->mdCcAc_c.Clear();
    return 1;
}
// @symbol _ZN7daMky_c11EnterState3Ev
int daMky_c::EnterState3()
{
    char* c = (char*)this;
    struct BCA_File;
    struct ModelAnim {
        void SetAnim(BCA_File*, int, int, unsigned int);
    };
    /* Signature deliberately copied from the local declaration above: the
       ROM name carries by-value class parameters (e.g. Fix12<int>), which
       mwccarm passes differently at the call site, so declaring the true
       types breaks the byte match. See notes/mwccarm-codegen.md 6az. */

    extern char data_ov030_02115ce0[];
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&this->mModelAnim, *(void **)(data_ov030_02115ce0 + 4), 0, 0x1000, 0);
    this->mModelAnim.speed = 0x1000;
    if (mHasSpawnedCap == 0 && SaveData::HasPlayerLostCap()) {
        unk_3c7 = 5;
    } else {
        Player *p = (Player *)*(char **)(c + 0x3a8);
        if (p->mIsMetal != 0 ||
            p->mIsVanish != 0 ||
            p->mHasWings != 0) {
            unk_3c7 = 4;
        } else {
            unk_3c7 = 0;
            unsigned char* f = (unsigned char*)((unsigned long long)((int)(c) + 0x3c8));
            *f ^= 1;
        }
    }
    mState = 3;
    return 1;
}

// @symbol _ZN7daMky_c19func_ov030_02113b38Ev
int daMky_c::func_ov030_02113b38()
{
    typedef int Fix12i;
    extern void _ZN9Animation7AdvanceEv(char* a);
    extern void _ZN8dActor_c9UpdatePosEP5dCc_c(char* c, char* cl);
    extern int _ZNK10dBgCh_Actr13JustHitGroundEv(char* w);
    extern void _ZN10dBgCh_Actr15ClearLimMovFlagEv(char* w);
    extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* ma, void* f, int b, Fix12i c, unsigned int d);
    extern int data_ov030_02115d18[];
  ((Animation *)((char *)this + 0x124))->Advance();
  this->UpdatePos(&this->mdCcAc_c);
  func_ov030_02111f6c(&this->mWithMeshClsn);
  if (this->mWithMeshClsn.JustHitGround() != 0) {
    this->mWithMeshClsn.ClearLimMovFlag();
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&this->mModelAnim, (void*)data_ov030_02115d18[1], 0, 0x1000, 0);
    this->mModelAnim.speed = 0x1000;
    this->mSpawnPosX = this->mPosX;
    this->mSpawnPosY = this->mPosY;
    this->mSpawnPosZ = this->mPosZ;
    func_ov030_021141a8(this->mPrevState);
  }
  this->mdCcAc_c.Clear();
  func_ov030_02111bc4();
  return 1;
}
// @symbol _ZN7daMky_c11EnterState2Ev
int daMky_c::EnterState2() {
    void *c = (void *)this;
    typedef int Fix12i;
    typedef short s16;

    extern void *data_ov030_02115d08;
    void *p;
    int b;

    p = *(void **)((char *)c + 0x3a8);
    if (p == 0)
        p = ((dActor_c *)c)->ClosestPlayer();

    b = (int)(this->actorID == 0x10b);
    if (b != 0) {
        this->mAngleY = Vec3_HorzAngle((char *)(Vector3 *)&this->mPosX, (char *)(Vector3 *)&this->mPerchPosX);
    } else if (Vec3_Dist((char *)(Vector3 *)&this->mPerchPosX, (char *)(Vector3 *)&this->mPosX) < 0x514000 &&
               this->mPosY > this->mPerchPosY - 0x12c000) {
        this->mAngleY = Vec3_HorzAngle((char *)(Vector3 *)&this->mPosX, (char *)p + 0x5c);
    } else {
        this->mAngleY = Vec3_HorzAngle((char *)(Vector3 *)&this->mPosX, (char *)(Vector3 *)&this->mPerchPosX);
    }

    this->mPrevAngleY = this->mAngleY;
    this->mHorzSpeed = 0xd000;
    b = (int)(this->actorID == 0x10b);
    this->mVertSpeed = b ? 0x23000 : 0x1e000;

    this->mWithMeshClsn.SetLimMovFlag();
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)&this->mModelAnim,
        ((void **)&data_ov030_02115d08)[1], 0x40000000, 0x1000, 0);
    func_0201267c(0xd1, (const ::Vector3 *)((char *)c + 0x74));
    func_0201267c(0xf1, (const ::Vector3 *)((char *)c + 0x74));
    this->mState = 2;
    return 1;
}

// @symbol _ZN7daMky_c19func_ov030_02113d20Ev
int daMky_c::func_ov030_02113d20()
{
    typedef int Fix12i;
    typedef short s16;

    extern Fix12i Vec3_Dist(const void *a, const void *b);
    extern s16 Vec3_HorzAngle(const void *v0, const void *v1);
    extern void _Z14ApproachLinearRsss(s16 *dst, s16 target, s16 step);
    extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *anim, void *file, int n, Fix12i speed, unsigned int flags);
    extern void _ZN9Animation7AdvanceEv(void *c);
    extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *c, void *clsn);
    extern int _ZNK10dBgCh_Actr8IsOnWallEv(void *c);
    extern int _ZN6Player7IsInAirEv(void *p);
    extern void _ZN5dCc_c6UpdateEv(void *c);
    extern void *data_ov030_02115d18;
    void *p;
    Fix12i dist;
    s16 angle;
    int onWall;
    int b;

    p = *(void **)((char *)this + 0x3a8);
    if (p == 0)
        p = ((dActor_c *)this)->ClosestPlayer();

    dist = Vec3_Dist((char *)(Vector3 *)&this->mPosX, (char *)p + 0x5c);

    b = (int)(this->actorID == 0x10c);
    if ((b && Vec3_Dist((char *)(Vector3 *)&this->mPerchPosX, (char *)p + 0x5c) > 0x514000) ||
        *(int *)((char *)p + 0x60) < this->mPerchPosY - 0x12c000) {
        this->unk_3c7 = 2;
    }

    switch (this->unk_3c7) {
    case 0:
        angle = Vec3_HorzAngle((char *)(Vector3 *)&this->mPosX, (char *)p + 0x5c) + 0x8000;
        this->mHorzSpeed = 0x13000;
        if (dist >= 0x1f4000)
            this->unk_3c7 = 2;
        break;
    case 1:
        angle = Vec3_HorzAngle((char *)(Vector3 *)&this->mPosX, (char *)p + 0x5c);
        this->mHorzSpeed = 0xc000;
        if (dist < 0x190000)
            this->unk_3c7 = 0;
        else if (dist < 0x1f4000)
            this->unk_3c7 = 2;
        break;
    case 2:
        angle = Vec3_HorzAngle((char *)(Vector3 *)&this->mPosX, (char *)p + 0x5c);
        this->mHorzSpeed = 0;
        if (dist < 0x190000)
            this->unk_3c7 = 0;
        else if (dist >= 0x258000)
            this->unk_3c7 = 1;
        break;
    }

    _Z14ApproachLinearRsss((s16 *)((char *)this + 0x8e), angle, 0xa28);
    this->mPrevAngleY = this->mAngleY;
    if (this->mHorzSpeed != 0)
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)&this->mModelAnim,
            ((void **)&data_ov030_02115d18)[1], 0, 0x1000, 0);
    else
        func_ov030_02111a00();

    _ZN9Animation7AdvanceEv((char *)this + 0x124);

    b = (int)(this->actorID == 0x10b);
    if (b) {
        _ZN8dActor_c9UpdatePosEP5dCc_c(this, (char *)&this->mdCcAc_c);
        onWall = (func_ov030_02111dd0() | _ZNK10dBgCh_Actr8IsOnWallEv((char *)&this->mWithMeshClsn)) != 0;
        func_ov030_02111f6c(&this->mWithMeshClsn);
        func_ov030_02111bc4();
    } else {
        _ZN8dActor_c9UpdatePosEP5dCc_c(this, (char *)&this->mdCcAc_c);
        func_ov030_02111f6c(&this->mWithMeshClsn);
        func_ov030_02111bc4();
        onWall = (func_ov030_02111ea4() | _ZNK10dBgCh_Actr8IsOnWallEv((char *)&this->mWithMeshClsn)) != 0;
    }

    if (this->mState == 1 && onWall && dist < 0x12c000) {
        if (*(int *)((char *)p + 0x98) > 0x9000 || _ZN6Player7IsInAirEv(p) != 0) {
            this->mPrevState = 1;
            func_ov030_021141a8(2);
        }
    }

    ((dCc_c *)((char *)&this->mdCcAc_c))->Clear();
    if (this->mState == 1)
        _ZN5dCc_c6UpdateEv((char *)&this->mdCcAc_c);
    func_ov030_02111890();
    return 1;
}
// @symbol _ZN7daMky_c11EnterState1Ev
int daMky_c::EnterState1(){
  unk_3c7=0;
  mState=1;
  return 1;
}

// @symbol _ZN7daMky_c19func_ov030_02113ff0Ev
int daMky_c::func_ov030_02113ff0()
{
    typedef int Fix12i;
    typedef short s16;
    extern int _ZN8dActor_c13DistToCPlayerEv(char* c);
    extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* ma, void* f, int b, Fix12i c, unsigned int d);
    extern s16 _ZN8dActor_c18HorzAngleToCPlayerEv(char* c);
    extern void _Z14ApproachLinearRsss(s16* p, s16 target, s16 step);
    extern void _ZN9Animation7AdvanceEv(char* a);
    extern void _ZN8dActor_c9UpdatePosEP5dCc_c(char* c, char* cl);
    extern void _ZN5dCc_c6UpdateEv(char* cl);
    extern int data_ov030_02115d18[];
  int dist = this->DistToCPlayer();
  int b;
  if (dist > 0x15e000) {
    this->mHorzSpeed = 0xc000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&this->mModelAnim, (void*)data_ov030_02115d18[1], 0, 0x1000, 0);
  } else if (dist <= 0xfa000) {
    this->mHorzSpeed = 0;
    func_ov030_02111a00();
  }
  _Z14ApproachLinearRsss((s16*)((char *)this+0x8e), this->HorzAngleToCPlayer(), 0x514);
  this->mPrevAngleY = this->mAngleY;
  this->mModelAnim.speed = 0x1000;
  ((Animation *)((char *)this + 0x124))->Advance();
  b = (this->actorID == 0x10b);
  if (b) {
    this->UpdatePos(&this->mdCcAc_c);
    func_ov030_02111dd0();
    func_ov030_02111f6c(&this->mWithMeshClsn);
    func_ov030_02111bc4();
  } else {
    this->UpdatePos(&this->mdCcAc_c);
    func_ov030_02111f6c(&this->mWithMeshClsn);
    func_ov030_02111bc4();
    func_ov030_02111ea4();
  }
  this->mdCcAc_c.Clear();
  this->mdCcAc_c.Update();
  func_ov030_02111890();
  return 1;
}
// @symbol _ZN7daMky_c11EnterState0Ev
int daMky_c::EnterState0()
{
    mState = 0;
    return 1;
}

// @symbol func_ov030_02114134
typedef void (daMky_c::*PMF)();
void daMky_c::func_ov030_02114134()
{
    PMF *p = (PMF *)this->mStateDesc + 1;
    (this->* *p)();
}
// @symbol _ZN7daMky_c19func_ov030_02114170Ev
void daMky_c::func_ov030_02114170()
{
    PMF *p = (PMF *)this->mStateDesc;
    (this->* *p)();
}
// @symbol _ZN7daMky_c19func_ov030_021141a8Ei
void daMky_c::func_ov030_021141a8(int idx)
{
    typedef struct { int a, b, c, d; } Item16;
    extern Item16 data_ov030_02115e0c[];
    daMky_c *self = (daMky_c *)(char *)this;

    self->mStateDesc = &data_ov030_02115e0c[idx];
    func_ov030_02114170();
}
/* File-scope extern "C" for the six virtuals below. A block-scope extern
 * inside a daMky_c method gets C++ linkage and the reference mangles a
 * second time, so these callees are declared here. The func_ov030_*
 * methods above are members of this class and are not redeclared. */
extern "C" {

void  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int idx, int speed, u32 flags);
void  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int a, int b, u32 c, u32 d);
void  _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int a, int b, void *v0, void *v1);
int   _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void *self, int d);

extern SharedFilePtr  data_ov002_0210da40;
extern SharedFilePtr  data_ov002_0210d9a0;
extern SharedFilePtr  data_ov002_0210d9c0;
extern SharedFilePtr  data_ov030_02115d00;
extern SharedFilePtr *data_ov030_02114824[10];
extern void          *data_ov030_02115cf0[];

}

/* Slot 3 of whatever object sits at +0xd4, reached through a raw cast in
   Behavior; the model's own header is not what that call goes through. */
struct VObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
};

// @symbol _ZN7daMky_c16CleanupResourcesEv
/* Vtable slot 3.  Releases the four shared files the class holds plus its
 * ten-entry table; it never touches `this`, which is why the legacy C form
 * could declare itself nullary and still reproduce. */
s32 daMky_c::CleanupResources()
{
    int i;
    data_ov002_0210da40.Release();
    data_ov002_0210d9a0.Release();
    data_ov002_0210d9c0.Release();
    data_ov030_02115d00.Release();
    for (i = 0; i < 10; i++)
        data_ov030_02114824[i]->Release();
    return 1;
}

// @symbol _ZN7daMky_c16OnPendingDestroyEv
/* Vtable slot 12.  The ROM body is one `bx lr`: the override exists only to
 * occupy the slot. */
void daMky_c::OnPendingDestroy()
{
}

// @symbol _ZN7daMky_c6RenderEv
/* Vtable slot 9. */
s32 daMky_c::Render()
{
    int b = (mFlags & 0x40000) != 0;
    if (b) return 1;
    mModelAnim.Model::Render(0);
    return 1;
}

// @symbol _ZN7daMky_c8BehaviorEv
/* Vtable slot 6. */
s32 daMky_c::Behavior()
{
    if (_ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(this, 0x5dc000) != 0 &&
        mState != 8) {
        int b = (actorID == 0x10b);
        if (b != 0 && mHasSpawnedCap == 0 && SaveData::HasPlayerLostCap() != 0) {
            Player *pl = ClosestPlayer();
            unsigned cp = pl->param1;
            if (cp < 3) {
                dActor_c *spawned;
                mCapPlayerNo = cp;
                spawned = Spawn(
                    0x10d,
                    (mCapPlayerNo << 8) | 2,
                    *(const Vector3 *)&mPosX,
                    0,
                    mAreaId,
                    -1);
                mCapUniqueID = spawned->uniqueID;
                mHasSpawnedCap = 1;
                func_ov030_021141a8(1);
            }
        }
        func_ov030_02111734();
    } else {
        func_ov030_02114134();
        mModelAnim.UpdateVerts();
        func_ov030_02112094();
    }
    return 1;
}

// @symbol _ZN7daMky_c13InitResourcesEv
/* Vtable slot 0. */
s32 daMky_c::InitResources()
{
    int i;
    int b;
    u16 h;

    Model::LoadFile(data_ov002_0210da40);
    Model::LoadFile(data_ov002_0210d9a0);
    Model::LoadFile(data_ov002_0210d9c0);
    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov030_02115d00), 1, 1);
    for (i = 0; i < 10; i++)
        Animation::LoadFile(*data_ov030_02114824[i]);
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov030_02115cf0[1], 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x28000, 0x64000, 0x800004, 0x49000);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mPerchPosX = mPosX;
    mPerchPosY = mPosY;
    mPerchPosZ = mPosZ;
    mPerchPosY += 0x64000;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    unk_3a8 = 0;
    mCapUniqueID = 0;
    unk_3cb = 0x96;
    h = actorID;
    b = 0;
    if (h == 0x10b)
        b = 1;
    if (b != 0) {
        if (SaveData::HasPlayerLostCap() != 0) {
            Player *player = ClosestPlayer();
            if (player->param1 >= 3)
                goto ov030_no_spawn;
            {
                dActor_c *spawned;
                mCapPlayerNo = player->param1;
                spawned = Spawn(0x10d, (mCapPlayerNo << 8) | 2, *(const Vector3 *)&mPosX, 0, mAreaId, -1);
                mCapUniqueID = spawned->uniqueID;
                mHasSpawnedCap = 1;
                func_ov030_021141a8(1);
                goto ov030_cap_done;
            }
ov030_no_spawn:
            func_ov030_021141a8(0);
ov030_cap_done:
            ;
        } else {
            func_ov030_021141a8(0);
        }
    } else {
        int t = (int)(h == 0x10c);
        if (t != 0)
            func_ov030_021141a8(1);
    }
    func_ov030_02112094();
    return 1;
}

// @symbol _ZN7daMky_c13OnTurnIntoEggER6Player
/* Vtable slot 19.  The cartridge's twelve bytes at this address are a linker
 * long-branch veneer to _ZN7fBase_c18MarkForDestructionEv in arm9 -- r0 still
 * holds `this` at the jump and the Player& is dropped -- so the source form
 * that reproduces it is a one-line forwarding member with no return. */
void daMky_c::OnTurnIntoEgg(Player &player)
{
    MarkForDestruction();
}

