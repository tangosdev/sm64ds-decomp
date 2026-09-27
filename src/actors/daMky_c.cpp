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
 * - func_ov030_* keep the ROM labels. Of the 22 pointer-to-member records
 *   at 0x02115ac8, EnterState0..10 are the eleven that store mState. The
 *   other eleven are the ticks; nothing in the image names them. The rest
 *   of the func_ov030_* run is not a member.
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

// @symbol func_ov030_02111734
extern "C" {
void func_ov030_02111734(char* c)
{
    extern unsigned char DecIfAbove0_Byte(unsigned char* p);
    extern void *_ZN9dBgCh_LinC1Ev(void* self);
    extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void* self, void* a, void* b, void* actor);
    extern int _ZN9dBgCh_Lin10DetectClsnEv(void* self);
    extern void Vec3_Asr(void* d, void* s, int sh);
    extern int _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(void* m, void* v, void* w, int fix, void* out);
    extern void _ZN9dBgCh_LinD1Ev(void* self);
    extern void func_ov030_02112094(void* c);

    extern char data_0209f43c;
    extern char data_0209b3ec;
    struct Vector3 a, b, out, asr;
    char rc[0x7c];

    if (DecIfAbove0_Byte((unsigned char*)(c + 0x3cb)))
        return;

    _ZN9dBgCh_LinC1Ev(rc);

    a.x = ((daMky_c *)c)->mPosX;
    a.y = ((daMky_c *)c)->mPosY;
    a.z = ((daMky_c *)c)->mPosZ;
    a.y = a.y + 0x32000;
    b.x = ((daMky_c *)c)->mPosX;
    b.y = ((daMky_c *)c)->mPosY;
    b.z = ((daMky_c *)c)->mPosZ;
    b.y = b.y - 0x96000;
    ((dBgCh_Lin *)rc)->SetObjAndLine(a, b, (dActor_c *)c);

    if (((daMky_c *)c)->mPerchPosY - ((daMky_c *)c)->mPosY <= 0x96000) {
        if (!((dBgCh_Lin *)rc)->DetectClsn())
            goto done;
    }

    Vec3_Asr(&asr, (Vector3 *)&((daMky_c *)c)->mPerchPosX, 3);

    if (_ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(&data_0209f43c, &data_0209b3ec, &asr, 0x1f400, &out) <= 0xc350000)
        goto done;

    ((daMky_c *)c)->mPosX = ((daMky_c *)c)->mPerchPosX;
    ((daMky_c *)c)->mPosY = ((daMky_c *)c)->mPerchPosY;
    ((daMky_c *)c)->mPosZ = ((daMky_c *)c)->mPerchPosZ;
    ((daMky_c *)c)->mSpawnPosX = ((daMky_c *)c)->mPerchPosX;
    ((daMky_c *)c)->mSpawnPosY = ((daMky_c *)c)->mPerchPosY;
    ((daMky_c *)c)->mSpawnPosZ = ((daMky_c *)c)->mPerchPosZ;
    ((daMky_c *)c)->mPrevPosX = ((daMky_c *)c)->mPerchPosX;
    ((daMky_c *)c)->mPrevPosY = ((daMky_c *)c)->mPerchPosY;
    ((daMky_c *)c)->mPrevPosZ = ((daMky_c *)c)->mPerchPosZ;
    func_ov030_02112094(c);
    ((daMky_c *)c)->unk_3cb = 0x96;

done:
    _ZN9dBgCh_LinD1Ev(rc);
}
}

// @symbol func_ov030_02111890
extern "C" {
void func_ov030_02111890(char *c)
{
    extern void func_0201267c(unsigned int id, const Vector3 *pos);
    extern int data_ov030_02115d18[];
    int b = (int)((int)((daMky_c *)c)->mModelAnim.file == data_ov030_02115d18[1]);
    if (b == 0)
        return;
    int v = (short)((unsigned int)((daMky_c *)c)->mModelAnim.currFrame << 4 >> 16);
    if (v == 0xa || v == 0xc)
        func_0201267c(0xea, (const Vector3 *)&((daMky_c *)c)->mCamSpacePosX);
}
}

// @symbol func_ov030_02111908
extern "C" {
void func_ov030_02111908(char* c)
{
    extern int data_ov030_02115cf0[];
    extern int data_ov030_02115cd0[];
    extern int data_ov030_02115cf8[];
    extern void func_0201267c(unsigned int id, const Vector3 *pos);

    enum Bool { FALSE, TRUE };
    int frame = (short)(((unsigned)(((daMky_c *)c)->mModelAnim.currFrame << 4)) >> 16);
    int v = (int)((daMky_c *)c)->mModelAnim.file;
    enum Bool b;

    b = (enum Bool)(v == data_ov030_02115cf0[1]);
    if (b) {
        if (frame != 7) {
            if (frame != 0x28) return;
        }
        func_0201267c(0xeb, (const Vector3 *)&((daMky_c *)c)->mCamSpacePosX);
        return;
    }
    b = (enum Bool)(v == data_ov030_02115cd0[1]);
    if (b) {
        if (frame != 1) return;
        func_0201267c(0xf1, (const Vector3 *)&((daMky_c *)c)->mCamSpacePosX);
        func_0201267c(0xe8, (const Vector3 *)&((daMky_c *)c)->mCamSpacePosX);
        return;
    }
    b = (enum Bool)(v == data_ov030_02115cf8[1]);
    if (b) {
        if (frame != 8) return;
        func_0201267c(0xe9, (const Vector3 *)&((daMky_c *)c)->mCamSpacePosX);
    }
}
}

// @symbol func_ov030_02111a00
extern "C" {
int func_ov030_02111a00(char* c)
{
    extern void func_ov030_02111908(void* c);
    extern int _ZNK9Animation12WillHitFrameEi(void* a, int f);
    extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* m, void* f, int a, int b, unsigned int e);
    extern int data_ov030_02115cf0[];
    extern int data_ov030_02115cd0[];
    extern int data_ov030_02115cf8[];
    extern int data_ov030_02115cd8[];
    extern void** data_ov030_02115bc8[];
    func_ov030_02111908(c);
    if (((Animation *)(c + 0x124))->WillHitFrame( 0) == 0) {
        int v = (int)((daMky_c *)c)->mModelAnim.file;
        int b;
        b = (int)(v == data_ov030_02115cf0[1]); if (b != 0) goto fail;
        b = (int)(v == data_ov030_02115cd0[1]); if (b != 0) goto fail;
        b = (int)(v == data_ov030_02115cf8[1]); if (b != 0) goto fail;
        b = (int)(v == data_ov030_02115cd8[1]); if (b != 0) goto fail;
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &((daMky_c *)c)->mModelAnim, data_ov030_02115bc8[((daMky_c *)c)->mAnimIdx][1], 0, 0x1000, 0);
    {
        unsigned char* p = (unsigned char*)(((int)c + 0x3ca));
        ((daMky_c *)c)->mModelAnim.speed = 0x1000;
        (*p)++;
    }
    if (((daMky_c *)c)->mAnimIdx >= 0xb)
        ((daMky_c *)c)->mAnimIdx = 0;
    return 1;
fail:
    return 0;
}
}

// @symbol func_ov030_02111b20
extern "C" {
int func_ov030_02111b20(char* c) {
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
  _ZNK7PathPtr7GetNodeER7Vector3j(c+0x398, &v, *(unsigned int*)(c+0x3a0));
  d = Vec3_HorzDist((struct Vector3*)(c+0x5c), &v);
  ang = Vec3_HorzAngle((struct Vector3*)(c+0x5c), &v);
  _Z11UpdateAngleRssis((short*)(c+0x8e), ang, 2, 0x400);
  *(s16*)(c+0x94) = *(s16*)(c+0x8e);
  if (d < *(int*)(c+0x98)) {
    n = _ZNK7PathPtr8NumNodesEv(c+0x398);
    p = (int*)(c + 0x3a0);
    n = n - 1;
    *p = *p + 1;
    if (*(int*)(c+0x3a0) >= n) return 1;
  }
  return 0;
}
}

// @symbol func_ov030_02111bc4
extern "C" {
int func_ov030_02111bc4(void *thiz)
{
    extern int func_ov030_021141a8(void *a, int b);
    extern void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
    extern int _ZN6Player7TryGrabER8dActor_c(void *p, void *a);
    unsigned char *c = (unsigned char *)thiz;
    unsigned char *player;
    int b;

    b = (int)((((daMky_c *)c)->mFlags & 0x20000) != 0);
    if (b != 0 && ((daMky_c *)c)->mState != 2) {
        ((daMky_c *)c)->unk_3a8 = *(void **)(c + 0xd0);
        b = (int)(((daMky_c *)c)->actorID == 0x10b);
        if (b != 0) {
            func_ov030_021141a8(c, 5);
        } else {
            b = (int)(((daMky_c *)c)->actorID == 0x10c);
            if (b != 0)
                func_ov030_021141a8(c, 6);
        }
        return 1;
    }

    if (((daMky_c *)c)->mdCcAc_c.otherOwner == 0)
        return 0;

    if ((((daMky_c *)c)->mdCcAc_c.hitFlags & 0x40000) && ((daMky_c *)c)->mState != 2) {
        *(void **)(c + 0x3a8) = ((dActor_c *)c)->ClosestPlayer();
        ((daMky_c *)c)->mPrevState = ((daMky_c *)c)->mState;
        func_ov030_021141a8(c, 2);
        return 1;
    }

    player = (unsigned char *)dActor_c::FindWithID(((daMky_c *)c)->mdCcAc_c.otherOwner);
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

    if (((daMky_c *)c)->mdCcAc_c.hitFlags & 0x1000) {
        if (((Player *)player)->TryGrab(*(dActor_c *)c)) {
            *(void **)(c + 0x3a8) = player;
            b = (int)(((daMky_c *)c)->actorID == 0x10b);
            if (b != 0) {
                func_ov030_021141a8(c, 3);
            } else {
                b = (int)(((daMky_c *)c)->actorID == 0x10c);
                if (b != 0)
                    func_ov030_021141a8(c, 4);
            }
        }
    }
    return 1;
}
}

// @symbol func_ov030_02111dd0
extern "C" {
int func_ov030_02111dd0(char* c)
{
    extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* thiz);
    extern int func_02038ea4(void* thiz);
    if (((daMky_c *)c)->mWithMeshClsn.IsOnGround() != 0) {
        dBgCh_Gnd rg;
        Vector3 v;
        int y, z, x, s;
        y = ((daMky_c *)c)->mPosY;
        z = ((daMky_c *)c)->mPosZ;
        x = ((daMky_c *)c)->mPosX;
        s = y + 0x1e000;
        v.x = x;
        v.y = s;
        v.z = z;
        rg.SetObjAndPos(v, (dActor_c*)c);
        if (func_02038ea4(&rg) == 0 || ((daMky_c *)c)->mPosY - rg.clsnY > 0x2000) {
            ((daMky_c *)c)->mPosX = ((daMky_c *)c)->mSpawnPosX;
            ((daMky_c *)c)->mPosY = ((daMky_c *)c)->mSpawnPosY;
            ((daMky_c *)c)->mPosZ = ((daMky_c *)c)->mSpawnPosZ;
            return 1;
        }
        ((daMky_c *)c)->mSpawnPosX = ((daMky_c *)c)->mPosX;
        ((daMky_c *)c)->mSpawnPosY = ((daMky_c *)c)->mPosY;
        ((daMky_c *)c)->mSpawnPosZ = ((daMky_c *)c)->mPosZ;
    }
    return 0;
}
}

// @symbol func_ov030_02111ea4
extern "C" {
int func_ov030_02111ea4(char* thiz)
{

    extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);

    #define ABS(x) ((x) < 0 ? -(x) : (x))
    char* c = thiz;
    if (((daMky_c *)c)->mWithMeshClsn.IsOnGround() != 0) {
        dBgCh_Gnd rg;
        Vector3 pos;
        {
            int y = ((daMky_c *)c)->mPosY;
            int z = ((daMky_c *)c)->mPosZ;
            int x = ((daMky_c *)c)->mPosX;
            int y2 = y + 0x1e000;
            pos.x = x;
            pos.y = y2;
            pos.z = z;
        }
        rg.SetObjAndPos(pos, (dActor_c*)c);
        if (rg.DetectClsn() == 0 ||
            ABS(rg.clsnY - ((daMky_c *)c)->mPosY) > 0x1000) {
            ((daMky_c *)c)->mHorzSpeed = 0;
            ((daMky_c *)c)->mPosX = ((daMky_c *)c)->mPrevPosX;
            ((daMky_c *)c)->mPosY = ((daMky_c *)c)->mPrevPosY;
            ((daMky_c *)c)->mPosZ = ((daMky_c *)c)->mPrevPosZ;
            return 1;
        }
    }
    return 0;
}
}

// @symbol func_ov030_02111f6c
extern "C" {
void func_ov030_02111f6c(char* c, dBgCh_Actr* w){
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
  int b = (int)((((daMky_c *)c)->mFlags & 0x4000) != 0);
  if (b != 0) return;
  int bb = (int)(((daMky_c *)c)->actorID == 0x10b);
  if (bb != 0 && ((daMky_c *)c)->mState != 9) func_020383f0(&((daMky_c *)c)->mWithMeshClsn);
  else dBgCh_Actr_UpdateContinuous_Veneer(&((daMky_c *)c)->mWithMeshClsn);
  if (_ZNK10dBgCh_Actr10IsOnGroundEv(w) != 0) {
    Vector3 n;
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((SurfaceInfo*)((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(w) + 4), &n);
    if (n.y != 0) {
      int s = (int)(((long long)n.x * ((daMky_c *)c)->unk_0a4 + 0x800) >> 0xc)
            + (int)(((long long)n.z * ((daMky_c *)c)->unk_0ac + 0x800) >> 0xc);
      ((daMky_c *)c)->mVertSpeed = -(_ZN4cstd4fdivEii(s, n.y) + 0x8000);
    }
  }
  if (_ZNK10dBgCh_Actr8IsOnWallEv(w) != 0) {
    Vector3 wn;
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((SurfaceInfo*)((char*)_ZNK10dBgCh_Actr13GetWallResultEv(w) + 4), &wn);
  }
}
}

// @symbol func_ov030_02112094
extern "C" {
void func_ov030_02112094(void* self)
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
    void func_ov030_02112094(void* self);
    char* c = (char*)self;
    int idx;
    void* res;
    void* obj;
    unsigned int id;
    Bundle bnd;

    int a = (int)((((daMky_c *)c)->mFlags & 0x100) != 0);
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
        *(M4x3*)(&((daMky_c *)c)->mModelAnim.mat4x3) = *(M4x3*)res;
    } else {
        Matrix4x3_FromRotationY(&((daMky_c *)c)->mModelAnim.mat4x3, ((daMky_c *)c)->mAngleY);
        ((daMky_c *)c)->mModelAnim.mat4x3.t.x = ((daMky_c *)c)->mPosX >> 3;
        ((daMky_c *)c)->mModelAnim.mat4x3.t.y = ((daMky_c *)c)->mPosY >> 3;
        ((daMky_c *)c)->mModelAnim.mat4x3.t.z = ((daMky_c *)c)->mPosZ >> 3;
    }

    int b = (int)((((daMky_c *)c)->mFlags & 0x40000) != 0);
    if (!b) {
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, &((daMky_c *)c)->mShadowModel, &((daMky_c *)c)->mModelAnim.mat4x3, 0x5a000, 0x190000, 0xf);
    }

    id = ((daMky_c *)c)->mCapUniqueID;
    if (id == 0)
        return;

    obj = dActor_c::FindWithID(id);

    bnd.trans.x = 0xa00;
    bnd.trans.y = 0;
    bnd.trans.z = -0x2f00;
    bnd.rot.x = -0x3f00;
    bnd.rot.y = 0;
    bnd.rot.z = -0x4000;

    data_020a0e68 = *(M4x3*)(&((daMky_c *)c)->mModelAnim.mat4x3);
    MulMat4x3Mat4x3(*(char**)((char *)&((daMky_c *)c)->mModelAnim.data.transforms) + 0xf0, &data_020a0e68, &data_020a0e68);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68,
        *(volatile int*)&bnd.trans.x, *(volatile int*)&bnd.trans.y, *(volatile int*)&bnd.trans.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(volatile short*)&bnd.rot.x, *(volatile short*)&bnd.rot.y, *(volatile short*)&bnd.rot.z);
    *(M4x3*)(((daMky_c *)c)->mCapMtx) = data_020a0e68;

    *(int*)((char*)obj + 0xc8) = (int)(((daMky_c *)c)->mCapMtx);
    *(int*)((char*)obj + 0x5c) = ((daMky_c *)c)->mPosX;
    *(int*)((char*)obj + 0x60) = ((daMky_c *)c)->mPosY;
    *(int*)((char*)obj + 0x64) = ((daMky_c *)c)->mPosZ;
}
}

// @symbol func_ov030_021122b0
extern "C" {
int func_ov030_021122b0(dActor_c *self)
{
    extern void _Z14ApproachLinearRsss(short *dst, short target, short rate);
    extern unsigned char DecIfAbove0_Byte(unsigned char *p);
    extern void func_ov030_021141a8(char *c, int x);
    extern void func_ov030_02111dd0(char *c);
    extern void func_ov030_02111f6c(char *c, void *w);
    extern void func_ov030_02111bc4(char *c);
    extern void func_ov030_02111ea4(char *c);
    extern void func_ov030_02111890(char *c);
    char *s = (char*)self;
    short ang = self->HorzAngleToCPlayer() + 0x8000;
    _Z14ApproachLinearRsss((short*)(s + 0x8e), ang, 0xa28);
    ((daMky_c *)s)->mPrevAngleY = ((daMky_c *)s)->mAngleY;
    if (DecIfAbove0_Byte((unsigned char*)(s + 0x3c6)) == 0)
        func_ov030_021141a8(s, 0);
    ((Animation *)(s + 0x124))->Advance();
    int b = (int)(((daMky_c *)s)->actorID == 0x10b);
    if (b) {
        self->UpdatePos((dCc_c*)(&((daMky_c *)s)->mdCcAc_c));
        func_ov030_02111dd0(s);
        func_ov030_02111f6c(s, &((daMky_c *)s)->mWithMeshClsn);
        func_ov030_02111bc4(s);
    } else {
        self->UpdatePos((dCc_c*)(&((daMky_c *)s)->mdCcAc_c));
        func_ov030_02111f6c(s, &((daMky_c *)s)->mWithMeshClsn);
        func_ov030_02111bc4(s);
        func_ov030_02111ea4(s);
    }
    ((daMky_c *)s)->mdCcAc_c.Clear();
    ((daMky_c *)s)->mdCcAc_c.Update();
    func_ov030_02111890(s);
    return 1;
}
}

// @symbol _ZN7daMky_c12EnterState10Ev
int daMky_c::EnterState10(){
    struct S { int w[2]; };
    extern struct S data_ov030_02115d18;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void*)data_ov030_02115d18.w[1], 0, 0x1000, 0);
  ((daMky_c *)this)->mModelAnim.speed = 0x1000;
  mHorzSpeed = 0x13000;
  mActionTimer = 0x1e;
  mState = 0xa;
  return 1;
}

// @symbol func_ov030_02112400
extern "C" {
int func_ov030_02112400(char* c)
{
    typedef void* (*Vfn)();

    struct Vector3;

    extern void func_ov030_02111a00(void* c);
    extern void func_ov030_02111f6c(char* c, dBgCh_Actr* w);
    extern void func_ov030_02111bc4(void* c);
    extern int Vec3_Dist(const Vector3* a, const Vector3* b);
    extern void func_ov030_021141a8(void* c, int x);
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
    func_ov030_02111a00(c);
    ((Animation *)(c + 0x124))->Advance();
    ((daMky_c *)c)->UpdatePos(&((daMky_c *)c)->mdCcAc_c);
    func_ov030_02111f6c(c, (dBgCh_Actr*)(&((daMky_c *)c)->mWithMeshClsn));
    func_ov030_02111bc4(c);
    ((daMky_c *)c)->mdCcAc_c.Clear();
    ((daMky_c *)c)->mdCcAc_c.Update();

    int b = (int)(((daMky_c *)c)->actorID == 0x10c);
    if (b != 0) {
        if (Vec3_Dist((Vector3 *)&((daMky_c *)c)->mPerchPosX, (Vector3 *)&((daMky_c *)c)->mPosX) < 0x514000) {
            if (((daMky_c *)c)->mPosY > ((daMky_c *)c)->mPerchPosY - 0x12c000) {
                func_ov030_021141a8(c, 1);
            }
        }
    } else {
        if (((daMky_c *)c)->mWithMeshClsn.IsOnGround()) {
            char* r = func_0203567c((dBgCh_Actr*)(&((daMky_c *)c)->mWithMeshClsn));
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
                func_ov030_021141a8(c, 0);
            }
            _ZN5dBgPiD1Ev(&res);
        }
    }
    return 1;
}
}

// @symbol _ZN7daMky_c11EnterState9Ev
int daMky_c::EnterState9()
{
    char *p = (char *)this;
    mHorzSpeed = 0;
    mState = 9;
    return 1;
}

// @symbol func_ov030_02112578
extern "C" {
int func_ov030_02112578(void *arg0)
{
    int func_ov030_02111b20(void *self);
    void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *bca, int a, int fx, unsigned int f);
    void func_ov030_02111890(void *c);
    s16 Vec3_HorzAngle(const void *v0, const void *v1);
    int _Z14ApproachLinearRsss(s16 *v, s16 target, s16 step);
    s32 Vec3_Dist(const void *a, const void *b);
    int _ZN6Player9StartTalkER7fBase_cb(void *player, void *actor, int b);
    void func_ov030_02111908(void *c);
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
    void func_ov030_02111f6c(void *c, void *w);

    extern void *data_ov030_02115cd0[];
    extern void *data_ov030_02115cf8[];
    extern void *data_ov030_02115d08[];
    extern void *data_ov030_02115d10[];
    extern void *data_ov030_02115d18[];
    u8 *c = (u8 *)arg0;
    void *cage = dActor_c::FindWithActorID(0x67, 0);
    void *player = ((dActor_c *)arg0)->ClosestPlayer();
    s32 v[3];
    *(s32 *)((u8 *)v + 0) = 0x981;
    *(s32 *)((u8 *)v + 4) = 0x77a;
    *(s32 *)((u8 *)v + 8) = 0x501;

    switch (((daMky_c *)c)->unk_3c7) {
    case 0:
        if (func_ov030_02111b20(arg0) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&((daMky_c *)c)->mModelAnim, data_ov030_02115cd0[1], 0, 0x1000, 0);
            ((daMky_c *)c)->mModelAnim.speed = 0x1000;
            ((daMky_c *)c)->mHorzSpeed = 0;
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        func_ov030_02111890(arg0);
        break;
    case 1:
        _Z14ApproachLinearRsss((s16 *)(c + 0x8e), Vec3_HorzAngle((Vector3 *)&((daMky_c *)c)->mPosX, (u8 *)player + 0x5c), 0x300);
        if (Vec3_Dist((Vector3 *)&((daMky_c *)c)->mPosX, (u8 *)player + 0x5c) < 0x96000) {
            if (_ZN6Player9StartTalkER7fBase_cb(player, arg0, 1) != 0) {
                { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
            }
        }
        func_ov030_02111908(arg0);
        break;
    case 2: {
        s32 sp[3];
        sp[0] = ((daMky_c *)c)->mPosX;
        sp[1] = ((daMky_c *)c)->mPosY;
        sp[2] = ((daMky_c *)c)->mPosZ;
        sp[1] += 0x50000;
        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(player, arg0, 0xbd, sp, 1, 0) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&((daMky_c *)c)->mModelAnim, data_ov030_02115cf8[1], 0, 0x1000, 0);
            ((Animation *)(c + 0x124))->SetFlags( 0);
            func_0201267c(0xd1, (const Vector3 *)&((daMky_c *)c)->mCamSpacePosX);
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        break;
    }
    case 3:
        if (_ZN6Player12GetTalkStateEv() == 2) {
            _ZN6Player18HasFinishedTalkingEv(player);
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&((daMky_c *)c)->mModelAnim, data_ov030_02115d18[1], 0, 0x1000, 0);
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        break;
    case 4:
        if (_Z14ApproachLinearRsss((s16 *)(c + 0x8e), (s16)0xffffe04e, 0x400) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&((daMky_c *)c)->mModelAnim, data_ov030_02115d08[1], 0x40000000, 0x1000, 0);
            ((daMky_c *)c)->mPrevAngleY = ((daMky_c *)c)->mAngleY;
            ((daMky_c *)c)->mHorzSpeed = 0xf000;
            ((daMky_c *)c)->mVertSpeed = 0x2f000;
            func_0201267c(0xf1, (const Vector3 *)&((daMky_c *)c)->mCamSpacePosX);
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        break;
    case 5:
        if (((daMky_c *)c)->mWithMeshClsn.JustHitGround() != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&((daMky_c *)c)->mModelAnim, data_ov030_02115d10[1], 0x40000000, 0x1000, 0);
            ((daMky_c *)c)->mHorzSpeed = 0;
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        break;
    case 6:
        if (((Animation *)(c + 0x124))->Finished() != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&((daMky_c *)c)->mModelAnim, data_ov030_02115cd0[1], 0, 0x1000, 0);
            ((daMky_c *)c)->mModelAnim.speed = 0x1000;
            *(s32 *)((u8 *)cage + 0x98) = 0x400;
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x14, 0x7f, 0x15666, 0);
            ((daMky_c *)c)->mActionTimer = 0x78;
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        /* fallthrough */
    case 7: {
        s32 *pp = (s32 *)((unsigned int)c + 0x3bc);
        *pp = *pp + 0x400;
        if (((daMky_c *)c)->unk_3bc > 0x17ffd) {
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
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&((daMky_c *)c)->mModelAnim, data_ov030_02115d08[1], 0x40000000, 0x1000, 0);
        *(s32 *)((u8 *)cage + 0x9c) = -0x2000;
        *(s32 *)((u8 *)cage + 0xa0) = -0x3c000;
        { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        break;
    case 10:
        if (dActor_c::FindWithActorID(0x67, 0) == 0) {
            _ZN7fBase_c18MarkForDestructionEv(arg0);
        }
        break;
    default:
        break;
    }

    ((Animation *)(c + 0x124))->Advance();
    _ZN8dActor_c9UpdatePosEP5dCc_c(arg0, &((daMky_c *)c)->mdCcAc_c);
    func_ov030_02111f6c(arg0, &((daMky_c *)c)->mWithMeshClsn);
    ((daMky_c *)c)->mdCcAc_c.Clear();
    return 1;
}
}

// @symbol _ZN7daMky_c11EnterState8Ev
int daMky_c::EnterState8() {
    char *c = (char *)this;
    struct G { void *a; void *b; };
    extern struct G data_ov030_02115d18;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&((daMky_c *)c)->mModelAnim, data_ov030_02115d18.b, 0, 0x1000, 0);
    ((daMky_c *)c)->mModelAnim.speed = 0x1000;
    ((PathPtr *)(&((daMky_c *)c)->mPathPtr))->FromID(*(int*)(c+8) & 0xff);
    mPathNode = 1;
    unk_3c7 = 0;
    ((daMky_c *)c)->mHorzSpeed = 0x6000;
    mState = 8;
    return 1;
}

// @symbol func_ov030_02112a84
extern "C" {
int func_ov030_02112a84(char *a)
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
    extern void func_ov030_021141a8(void *a, int m);
    extern void _ZN5dBgPiD1Ev(void *r);
    extern int Vec3_Dist(void *a, void *b);
    extern void _ZN9Animation7AdvanceEv(void *p);
    extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *a, void *c);
    extern void func_ov030_02111bc4(void *a);
    extern void *data_02099368[];
    dBgPi res;

    dBgCh_Actr_UpdateContinuous_Veneer(&((daMky_c *)a)->mWithMeshClsn);
    if (((daMky_c *)a)->mWithMeshClsn.JustHitGround() || ((daMky_c *)a)->mWithMeshClsn.IsOnGround()) {
        int b;
        u16 id;

        b = 0;
        ((daMky_c *)a)->mVertSpeed = 0;
        id = ((daMky_c *)a)->actorID;
        if (id == 0x10b)
            b = 1;
        if (b) {
            char *r = (char *)func_0203567c(&((daMky_c *)a)->mWithMeshClsn);
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
                func_ov030_021141a8(a, 9);
            else
                func_ov030_021141a8(a, ((daMky_c *)a)->mPrevState);
            _ZN5dBgPiD1Ev(&res);
        } else {
            int t = (int)(id == 0x10c);
            if (t != 0) {
                if (Vec3_Dist((Vector3 *)&((daMky_c *)a)->mPerchPosX, (Vector3 *)&((daMky_c *)a)->mPosX) < 0x514000
                    && ((daMky_c *)a)->mPosY > ((daMky_c *)a)->mPerchPosY - 0x12c000) {
                    func_ov030_021141a8(a, ((daMky_c *)a)->mPrevState);
                } else {
                    func_ov030_021141a8(a, 9);
                }
            }
        }
    }

    ((Animation *)(a + 0x124))->Advance();
    ((daMky_c *)a)->UpdatePos(&((daMky_c *)a)->mdCcAc_c);
    func_ov030_02111bc4(a);
    ((daMky_c *)a)->mdCcAc_c.Clear();
    return 1;
}
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
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&((daMky_c *)c)->mModelAnim, data_ov030_02115d08[1], 0x40000000, 0x1000, 0);
  *((int *) (c + 0x98)) = 0xa000;
  *((int *) (c + 0xa8)) = 0;
  other = *((u8 **) (c + 0x3a8));
  pos = (int *) ((int) (((s64) ((int) ((Vector3 *)&((daMky_c *)c)->mPosX)))));
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
    *((int *) ((Vector3 *)&((daMky_c *)c)->mPosX)) = t0;
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

// @symbol func_ov030_02112da0
extern "C" {
int func_ov030_02112da0(char *a) {
    extern void func_ov030_021141a8(void *a, int m);
    extern int Vec3_Dist(void *a, void *b);
    extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void *a, void *self, unsigned int, void *, unsigned int, unsigned int);
    extern void func_0201267c(unsigned int id, const Vector3 *pos);
    extern int _ZN6Player12GetTalkStateEv(void *p);
    extern int _ZN6Player9DropActorEv(void *p);
    extern u8 DecIfAbove0_Byte(u8 *p);
    extern u8 data_0209d684;
    int b = (int)((((daMky_c *)a)->mFlags & 0x40000) != 0);
    if (b != 0) {
        int p = (int)(*(char **)(a + 0x3a8) + 0x5c);
        ((daMky_c *)a)->mPosX = *(int *)p;
        ((daMky_c *)a)->mPosY = *(int *)(p + 4);
        ((daMky_c *)a)->mPosZ = *(int *)(p + 8);
    }

    {
        u32 flags = ((daMky_c *)a)->mFlags;
        b = (int)((flags & 0x80000) != 0);
        if (b != 0) {
            ((daMky_c *)a)->mPrevState = 1;
            func_ov030_021141a8(a, 7);
            return 1;
        }

        switch (((daMky_c *)a)->unk_3c7) {
        case 0: {
            int b2 = (int)((flags & 0x40000) != 0);
            if (b2 != 0) {
                char *s = *(char **)(a + 0x3a8);
                int off = 0x3c7;
                int *p = (int *)(s + 0x5c);
                int x = *p;
                u8 *st = (u8 *)((int)a + off);
                ((daMky_c *)a)->mPosX = x;
                ((daMky_c *)a)->mPosY = p[1];
                ((daMky_c *)a)->mPosZ = p[2];
                (*st)++;
            } else {
                int b3 = (int)((flags & 0x20000) != 0);
                if (b3 != 0) break;
                if (b2 != 0) break;
                *(int *)(a + 0xd0) = 0;
                func_ov030_021141a8(a, ((daMky_c *)a)->mPrevState);
            }
            break;
        }
        case 1:
            if (Vec3_Dist((Vector3 *)&((daMky_c *)a)->mPerchPosX, (Vector3 *)&((daMky_c *)a)->mPosX) < 0x514000 &&
                ((daMky_c *)a)->mPosY > ((daMky_c *)a)->mPerchPosY - 0x12c000) {
                if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(char **)(a + 0x3a8), a, 0xc1, 0, 0, 0) != 0) {
                    func_0201267c(0xd1, (const Vector3 *)&((daMky_c *)a)->mCamSpacePosX);
                    (*(u8 *)((int)a + 0x3c7))++;
                }
            }
            {
                char *s = *(char **)(a + 0x3a8);
                u8 val = 0x3c;
                int *p = (int *)(s + 0x5c);
                ((daMky_c *)a)->mPosX = *p;
                ((daMky_c *)a)->mPosY = p[1];
                ((daMky_c *)a)->mPosZ = p[2];
                ((daMky_c *)a)->mActionTimer = val;
            }
            break;
        case 2:
            if (_ZN6Player12GetTalkStateEv(*(char **)(a + 0x3a8)) == -1) {
                u8 g = data_0209d684;
                if (g == 1) {
                    _ZN6Player9DropActorEv(*(char **)(a + 0x3a8));
                    ((daMky_c *)a)->mPrevState = 8;
                    func_ov030_021141a8(a, 7);
                } else if (g == 2) {
                    (*(u8 *)((int)a + 0x3c7))++;
                }
            }
            break;
        case 3:
            if (DecIfAbove0_Byte((u8 *)((int)a + 0x3c6)) == 0) {
                ((daMky_c *)a)->unk_3c7 = 1;
            }
            break;
        }
    }
    return 1;
}
}

// @symbol _ZN7daMky_c11EnterState6Ev
int daMky_c::EnterState6()
{
    char *c = (char *)this;
    ((daMky_c *)c)->mFlags &= ~0x80000;
    if (Vec3_Dist((Vector3 *)&((daMky_c *)c)->mPerchPosX, (Vector3 *)&((daMky_c *)c)->mPosX) < 0x514000 &&
        mPosY > mPerchPosY - 0x12c000) {
        unk_3c7 = 0;
        ((dActor_c *)c)->SpawnSoundObj(1);
    } else {
        unk_3c7 = 3;
    }
    mHorzSpeed = 0;
    mActionTimer = 0x3c;
    ((daMky_c *)c)->mdCcAc_c.Clear();
    mPrevState = mState;
    mState = 6;
    return 1;
}

// @symbol func_ov030_02113094
extern "C" {
int func_ov030_02113094(char* self)
{
    struct dActor_c;


    extern struct dActor_c* _ZN8dActor_c10FindWithIDEj(u32 id);
    extern void func_ov030_021141a8(char* self, int a);
    extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(char* p, char* self, u32 msg, const struct Vector3* pos, u32 a, u32 b);
    extern void func_0201267c(unsigned int id, const Vector3 *pos);
    extern int _ZN6Player12GetTalkStateEv(char* p);
    extern int _ZN6Player9DropActorEv(char* p);
    extern struct dActor_c* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 id, u32 param, const struct Vector3* pos, const struct Vector3_16* rot, int a, int b);
    extern void _ZN7fBase_c18MarkForDestructionEv(char* self);
    {
        int b = (int)((((daMky_c *)self)->mFlags & 0x40000) != 0);
        if (b != 0) {
            int p = (int)((((int)*(char**)(self + 0x3a8)) + 0x5c));
            ((daMky_c *)self)->mPosX = *(int*)p;
            ((daMky_c *)self)->mPosY = *(int*)(p + 4);
            ((daMky_c *)self)->mPosZ = *(int*)(p + 8);
        }
    }

    switch (((daMky_c *)self)->unk_3c7) {
    case 0: {
        int b2 = (int)((((daMky_c *)self)->mFlags & 0x40000) != 0);
        if (b2 != 0) {
            if (((daMky_c *)self)->mHasSpawnedCap != 0) {
                struct dActor_c* a = (struct dActor_c *)::dActor_c::FindWithID(((daMky_c *)self)->mCapUniqueID);
                *(char**)((char*)a + 0xd0) = *(char**)(self + 0x3a8);
                *(u32*)(((int)a + 0xb0)) |= 0x40000;
            }
            (*(u8*)(((int)self + 0x3c7)))++;
        } else {
            int b3 = (int)((((daMky_c *)self)->mFlags & 0x20000) != 0);
            if (b3 != 0) break;
            if (b2 != 0) break;
            *(int*)(self + 0xd0) = 0;
            func_ov030_021141a8(self, ((daMky_c *)self)->mPrevState);
        }
        break;
    }
    case 1: {
        int msg = (((daMky_c *)self)->mHasSpawnedCap != 0) ? 0xc2 : 0xc3;
        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(char**)(self + 0x3a8), self, (s16)msg, 0, 0, 0) != 0) {
            func_0201267c(0xd1, (const Vector3 *)&((daMky_c *)self)->mCamSpacePosX);
            (*(u8*)(((int)self + 0x3c7)))++;
        }
        {
            int b4 = (int)((((daMky_c *)self)->mFlags & 0x80000) != 0);
            if (b4 != 0) {
                func_ov030_021141a8(self, 7);
            }
        }
        break;
    }
    case 2:
        if (_ZN6Player12GetTalkStateEv(*(char**)(self + 0x3a8)) == -1) {
            _ZN6Player9DropActorEv(*(char**)(self + 0x3a8));
            (*(u8*)(((int)self + 0x3c7)))++;
        }
        break;
    case 3: {
        int b5 = (int)((((daMky_c *)self)->mFlags & 0x80000) != 0);
        if (b5 != 0) {
            if (((daMky_c *)self)->mHasSpawnedCap != 0) {
                _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x10d, (((daMky_c *)self)->mCapPlayerNo << 8) | 5, (struct Vector3*)(*(char**)(self + 0x3a8) + 0x5c), 0, ((daMky_c *)self)->mAreaId, -1);
                ((fBase_c *)::dActor_c::FindWithID(((daMky_c *)self)->mCapUniqueID))->MarkForDestruction();
                {
                    u32 z = 0;
                    ((daMky_c *)self)->mCapUniqueID = z;
                    ((daMky_c *)self)->mHasSpawnedCap = (u8)z;
                    ((daMky_c *)self)->mPrevState = 0xa;
                }
            }
            func_ov030_021141a8(self, 7);
        }
        break;
    }
    }

    return 1;
}
}

// @symbol _ZN7daMky_c11EnterState5Ev
int daMky_c::EnterState5() {
    char* c = (char*)this;
    int* p = (int*)((int)c + 0xb0);
    int tmp = *p;
    *p = tmp & ~0x80000;
    unk_3c7 = 0;
    void* clsn = (void*)(&((daMky_c *)c)->mdCcAc_c);
    ((daMky_c *)c)->mHorzSpeed = 0;
    ((dCc_c *)clsn)->Clear();
    ((daMky_c *)c)->mWithMeshClsn.ClearGroundFlag();
    mPrevState = mState;
    mState = 5;
    return 1;
}

// @symbol func_ov030_02113324
extern "C" {
int func_ov030_02113324(void* thiz)
{
    extern int Vec3_Dist(const Vector3* a, const Vector3* b);
    extern void func_ov030_021141a8(char* c, int v);
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
    char *c = (char*)thiz;

    ((daMky_c *)c)->mAngleY = *(short*)((char*)(*(void**)(c + 0x3a8)) + 0x8e);
    ((daMky_c *)c)->mPrevAngleY = ((daMky_c *)c)->mAngleY;

    {
        unsigned int flags = ((daMky_c *)c)->mFlags;
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
        ((dActor_c *)c)->DetectRaycastClsn(v, *(Vector3 *)&((daMky_c *)c)->mPosX, 1);

        if (Vec3_Dist((Vector3 *)&((daMky_c *)c)->mPerchPosX, (Vector3 *)&((daMky_c *)c)->mPosX) < 0x514000 &&
            ((daMky_c *)c)->mPosY > ((daMky_c *)c)->mPerchPosY - 0x12c000) {
            func_ov030_021141a8(c, 1);
        } else {
            func_ov030_021141a8(c, 9);
        }
        *(void**)(c + 0x3a8) = 0;
        return 1;
    }
skip_raycast:
    switch (((daMky_c *)c)->unk_3c7) {
    case 0:
        if (Vec3_Dist((Vector3 *)&((daMky_c *)c)->mPerchPosX, (Vector3 *)&((daMky_c *)c)->mPosX) < 0x514000 &&
            ((daMky_c *)c)->mPosY > ((daMky_c *)c)->mPerchPosY - 0x12c000) {
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
                msgPos.y = ((daMky_c *)c)->mPosY + 0x64000;

                if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(void**)(c + 0x3a8), c, 0xc0, &msgPos, 0, 2) != 0) {
                    func_0201267c(0xd1, (const Vector3 *)&((daMky_c *)c)->mCamSpacePosX);
                    {
                        unsigned char *p = (unsigned char*)(c + 0x3c7);
                        (*p)++;
                    }
                }
            }
        }
        ((daMky_c *)c)->mActionTimer = 0x3c;
        break;
    case 1:
        if (_ZN6Player12GetTalkStateEv(*(void**)(c + 0x3a8)) == -1) {
            unsigned char g = data_0209d684;
            if (g == 1) {
                _ZN6Player9DropActorEv(*(void**)(c + 0x3a8));
                func_ov030_021141a8(c, 8);
            } else if (g == 2) {
                unsigned char *p = (unsigned char*)(c + 0x3c7);
                (*p)++;
            }
        }
        break;
    case 2:
        if (DecIfAbove0_Byte((unsigned char*)(c + 0x3c6)) == 0)
            ((daMky_c *)c)->unk_3c7 = 0;
        break;
    }

    ((Animation *)(c + 0x124))->Advance();
    ((daMky_c *)c)->mdCcAc_c.Clear();
    return 1;
}
}

// @symbol _ZN7daMky_c11EnterState4Ev
int daMky_c::EnterState4(){
    char* c = (char*)this;
    extern int data_ov030_02115ce0[];
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&((daMky_c *)c)->mModelAnim, (void *)data_ov030_02115ce0[1], 0, 0x1000, 0);
    ((daMky_c *)c)->mModelAnim.speed = 0x1000;
    if (Vec3_Dist((Vector3 *)&((daMky_c *)c)->mPerchPosX, (Vector3 *)&((daMky_c *)c)->mPosX) < 0x514000
        && ((daMky_c *)c)->mPosY > mPerchPosY - 0x12c000) {
        unk_3c7 = 0;
        ((dActor_c *)c)->SpawnSoundObj(1);
    } else {
        unk_3c7 = 2;
    }
    mActionTimer = 0x3c;
    mState = 4;
    return 1;
}

// @symbol func_ov030_021136b0
extern "C" {
int func_ov030_021136b0(char *c)
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
    extern void func_ov030_021141a8(char *c, int v);
    extern void _ZN9Animation7AdvanceEv(void *self);

    extern void *data_0209f318;
    extern Matrix4x3 data_020a0e68;
    int msg;
    s16 a = *(s16 *)(*(char **)(c + 0x3a8) + 0x8e);
    ((daMky_c *)c)->mAngleY = a;
    ((daMky_c *)c)->mPrevAngleY = ((daMky_c *)c)->mAngleY;

    switch (((daMky_c *)c)->unk_3c7) {
    case 0:
        if (((daMky_c *)c)->mHasSpawnedCap != 0) {
            {
                Player *p = (Player *)*(char **)(c + 0x3a8);
                int t = (p->mCharacter == p->param1);
                t = (t != 0);
                ((daMky_c *)c)->pad_3c9 = t;
            }
            if (((daMky_c *)c)->pad_3c9 == 0) {
                Player *p = (Player *)*(char **)(c + 0x3a8);
                p->mHasNoCap = 1;
            } else {
                SaveData::PlayerLoseCap();
            }
            {
                Player *p = (Player *)*(char **)(c + 0x3a8);
                void *spawned;
                ((daMky_c *)c)->mCapPlayerNo = p->param1;
                msg = ((daMky_c *)c)->mAreaId;
                spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0x10d,
                    (((daMky_c *)c)->mCapPlayerNo << 8) | 2,
                    (Vector3 *)&((daMky_c *)c)->mPosX,
                    0,
                    msg,
                    -1);
                ((daMky_c *)c)->mCapUniqueID = ((u32 *)spawned)[1];
            }
        } else {
            if (((daMky_c *)c)->pad_3c9 != 0)
                func_02012790(0xa);
        }
        {
            u8 *st = (u8 *)(c + 0x3c7);
            (*st)++;
        }
        /* fall through */
    case 1: {
        s16 ang;
        Vector3 camPos;
        Vector3 msgPos;
        {
            u8 fl = ((daMky_c *)c)->mHasSpawnedCap;
            msg = fl ? 0xbe : 0xbf;
            void *camBase = data_0209f318;
            Vector3 *src = (Vector3 *)((char *)camBase + 0x8c);
            camPos.x = src->x;
            camPos.y = src->y;
            camPos.z = src->z;
            ang = Vec3_HorzAngle(&camPos, (Vector3 *)(*(char **)(c + 0x3a8) + 0x5c));
        }
        {
            int *op = (int *)(*(char **)(c + 0x3a8) + 0x5c);
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
            msgPos.y = ((daMky_c *)c)->mPosY + 0x64000;
            if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
                    *(void **)(c + 0x3a8), c, msgArg, &msgPos, 0, 2) != 0) {
                func_0201267c(0xd1, (const Vector3 *)&((daMky_c *)c)->mCamSpacePosX);
                {
                    u8 *st = (u8 *)(c + 0x3c7);
                    (*st)++;
                }
            }
        }
        break;
    }
    case 2:
        if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x3a8)) == -1) {
            if (((daMky_c *)c)->mHasSpawnedCap != 0) {
                {
                    u32 *fl = (u32 *)(c + 0xb0);
                    *fl &= ~0x200u;
                }
                _ZN6Player9DropActorEv(*(void **)(c + 0x3a8));
                {
                    u32 *fl = (u32 *)(c + 0xb0);
                    *fl |= 0x200u;
                }
            } else {
                _ZN6Player9DropActorEv(*(void **)(c + 0x3a8));
            }
            {
                u8 *st = (u8 *)(c + 0x3c7);
                (*st)++;
            }
        }
        break;
    case 3: {
        int f1 = (int)((((daMky_c *)c)->mFlags & 0x100) != 0);
        if (f1 == 0) {
            if (((daMky_c *)c)->mHasSpawnedCap != 0) {
                if (((daMky_c *)c)->pad_3c9 == 0) {
                    Player *p = (Player *)*(char **)(c + 0x3a8);
                    p->SetNewHatCharacter(p->mCharacter, 0, 0);
                }
                ((daMky_c *)c)->mPrevState = 1;
                func_ov030_021141a8(c, 2);
            } else {
                void *act = dActor_c::FindWithID(((daMky_c *)c)->mCapUniqueID);
                int z = 0;
                *(int *)((char *)act + 0xc8) = z;
                {
                    char *p = *(char **)(c + 0x3a8);
                    int *src = (int *)(p + 0x5c);
                    *(int *)((char *)act + 0x5c) = src[0];
                    *(int *)((char *)act + 0x60) = src[1];
                    *(int *)((char *)act + 0x64) = src[2];
                }
                ((daMky_c *)c)->mCapUniqueID = (u32)z;
                func_ov030_021141a8(c, 0xa);
            }
            *(void **)(c + 0x3a8) = 0;
        }
        break;
    }
    case 4: {
        Player *p = (Player *)*(char **)(c + 0x3a8);
        if (p->mIsMetal == 0 &&
            p->mIsVanish == 0 &&
            p->mHasWings == 0) {
            ((daMky_c *)c)->unk_3c7 = 0;
            {
                u8 *f = (u8 *)(c + 0x3c8);
                *f ^= 1;
            }
            break;
        }
        /* fall through */
    }
    case 5: {
        u32 flags = ((daMky_c *)c)->mFlags;
        int f1 = (int)((flags & 0x100) != 0);
        if (f1 != 0) {
            int f2 = (int)((flags & 0x2000) != 0);
            if (f2 == 0)
                break;
        }
        func_ov030_021141a8(c, 0xa);
        *(void **)(c + 0x3a8) = 0;
        break;
    }
    default:
        break;
    }

    ((Animation *)(c + 0x124))->Advance();
    ((daMky_c *)c)->mdCcAc_c.Clear();
    return 1;
}
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
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&((daMky_c *)c)->mModelAnim, *(void **)(data_ov030_02115ce0 + 4), 0, 0x1000, 0);
    ((daMky_c *)c)->mModelAnim.speed = 0x1000;
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

// @symbol func_ov030_02113b38
extern "C" {
int func_ov030_02113b38(char* c){
    typedef int Fix12i;
    extern void _ZN9Animation7AdvanceEv(char* a);
    extern void _ZN8dActor_c9UpdatePosEP5dCc_c(char* c, char* cl);
    extern void func_ov030_02111f6c(char* c, char* w);
    extern int _ZNK10dBgCh_Actr13JustHitGroundEv(char* w);
    extern void _ZN10dBgCh_Actr15ClearLimMovFlagEv(char* w);
    extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* ma, void* f, int b, Fix12i c, unsigned int d);
    extern void func_ov030_021141a8(char* c, int v);
    extern void func_ov030_02111bc4(char* c);
    extern int data_ov030_02115d18[];
  ((Animation *)(c + 0x124))->Advance();
  ((daMky_c *)c)->UpdatePos(&((daMky_c *)c)->mdCcAc_c);
  func_ov030_02111f6c(c, &((daMky_c *)c)->mWithMeshClsn);
  if (((daMky_c *)c)->mWithMeshClsn.JustHitGround() != 0) {
    ((daMky_c *)c)->mWithMeshClsn.ClearLimMovFlag();
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&((daMky_c *)c)->mModelAnim, (void*)data_ov030_02115d18[1], 0, 0x1000, 0);
    ((daMky_c *)c)->mModelAnim.speed = 0x1000;
    ((daMky_c *)c)->mSpawnPosX = ((daMky_c *)c)->mPosX;
    ((daMky_c *)c)->mSpawnPosY = ((daMky_c *)c)->mPosY;
    ((daMky_c *)c)->mSpawnPosZ = ((daMky_c *)c)->mPosZ;
    func_ov030_021141a8(c, ((daMky_c *)c)->mPrevState);
  }
  ((daMky_c *)c)->mdCcAc_c.Clear();
  func_ov030_02111bc4(c);
  return 1;
}
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

    b = (int)(((daMky_c *)c)->actorID == 0x10b);
    if (b != 0) {
        ((daMky_c *)c)->mAngleY = Vec3_HorzAngle((char *)(Vector3 *)&((daMky_c *)c)->mPosX, (char *)(Vector3 *)&((daMky_c *)c)->mPerchPosX);
    } else if (Vec3_Dist((char *)(Vector3 *)&((daMky_c *)c)->mPerchPosX, (char *)(Vector3 *)&((daMky_c *)c)->mPosX) < 0x514000 &&
               ((daMky_c *)c)->mPosY > ((daMky_c *)c)->mPerchPosY - 0x12c000) {
        ((daMky_c *)c)->mAngleY = Vec3_HorzAngle((char *)(Vector3 *)&((daMky_c *)c)->mPosX, (char *)p + 0x5c);
    } else {
        ((daMky_c *)c)->mAngleY = Vec3_HorzAngle((char *)(Vector3 *)&((daMky_c *)c)->mPosX, (char *)(Vector3 *)&((daMky_c *)c)->mPerchPosX);
    }

    ((daMky_c *)c)->mPrevAngleY = ((daMky_c *)c)->mAngleY;
    ((daMky_c *)c)->mHorzSpeed = 0xd000;
    b = (int)(((daMky_c *)c)->actorID == 0x10b);
    ((daMky_c *)c)->mVertSpeed = b ? 0x23000 : 0x1e000;

    ((daMky_c *)c)->mWithMeshClsn.SetLimMovFlag();
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)&((daMky_c *)c)->mModelAnim,
        ((void **)&data_ov030_02115d08)[1], 0x40000000, 0x1000, 0);
    func_0201267c(0xd1, (const ::Vector3 *)((char *)c + 0x74));
    func_0201267c(0xf1, (const ::Vector3 *)((char *)c + 0x74));
    ((daMky_c *)c)->mState = 2;
    return 1;
}

// @symbol func_ov030_02113d20
extern "C" {
int func_ov030_02113d20(void *c) {
    typedef int Fix12i;
    typedef short s16;

    extern Fix12i Vec3_Dist(const void *a, const void *b);
    extern s16 Vec3_HorzAngle(const void *v0, const void *v1);
    extern void _Z14ApproachLinearRsss(s16 *dst, s16 target, s16 step);
    extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *anim, void *file, int n, Fix12i speed, unsigned int flags);
    extern void func_ov030_02111a00(void *c);
    extern void _ZN9Animation7AdvanceEv(void *c);
    extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *c, void *clsn);
    extern int func_ov030_02111dd0(void *c);
    extern int _ZNK10dBgCh_Actr8IsOnWallEv(void *c);
    extern void func_ov030_02111f6c(void *c, void *w);
    extern void func_ov030_02111bc4(void *c);
    extern int func_ov030_02111ea4(void *c);
    extern int _ZN6Player7IsInAirEv(void *p);
    extern void func_ov030_021141a8(void *c, int mode);
    extern void _ZN5dCc_c6UpdateEv(void *c);
    extern void func_ov030_02111890(void *c);
    extern void *data_ov030_02115d18;
    void *p;
    Fix12i dist;
    s16 angle;
    int onWall;
    int b;

    p = *(void **)((char *)c + 0x3a8);
    if (p == 0)
        p = ((dActor_c *)c)->ClosestPlayer();

    dist = Vec3_Dist((char *)(Vector3 *)&((daMky_c *)c)->mPosX, (char *)p + 0x5c);

    b = (int)(((daMky_c *)c)->actorID == 0x10c);
    if ((b && Vec3_Dist((char *)(Vector3 *)&((daMky_c *)c)->mPerchPosX, (char *)p + 0x5c) > 0x514000) ||
        *(int *)((char *)p + 0x60) < ((daMky_c *)c)->mPerchPosY - 0x12c000) {
        ((daMky_c *)c)->unk_3c7 = 2;
    }

    switch (((daMky_c *)c)->unk_3c7) {
    case 0:
        angle = Vec3_HorzAngle((char *)(Vector3 *)&((daMky_c *)c)->mPosX, (char *)p + 0x5c) + 0x8000;
        ((daMky_c *)c)->mHorzSpeed = 0x13000;
        if (dist >= 0x1f4000)
            ((daMky_c *)c)->unk_3c7 = 2;
        break;
    case 1:
        angle = Vec3_HorzAngle((char *)(Vector3 *)&((daMky_c *)c)->mPosX, (char *)p + 0x5c);
        ((daMky_c *)c)->mHorzSpeed = 0xc000;
        if (dist < 0x190000)
            ((daMky_c *)c)->unk_3c7 = 0;
        else if (dist < 0x1f4000)
            ((daMky_c *)c)->unk_3c7 = 2;
        break;
    case 2:
        angle = Vec3_HorzAngle((char *)(Vector3 *)&((daMky_c *)c)->mPosX, (char *)p + 0x5c);
        ((daMky_c *)c)->mHorzSpeed = 0;
        if (dist < 0x190000)
            ((daMky_c *)c)->unk_3c7 = 0;
        else if (dist >= 0x258000)
            ((daMky_c *)c)->unk_3c7 = 1;
        break;
    }

    _Z14ApproachLinearRsss((s16 *)((char *)c + 0x8e), angle, 0xa28);
    ((daMky_c *)c)->mPrevAngleY = ((daMky_c *)c)->mAngleY;
    if (((daMky_c *)c)->mHorzSpeed != 0)
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)&((daMky_c *)c)->mModelAnim,
            ((void **)&data_ov030_02115d18)[1], 0, 0x1000, 0);
    else
        func_ov030_02111a00(c);

    _ZN9Animation7AdvanceEv((char *)c + 0x124);

    b = (int)(((daMky_c *)c)->actorID == 0x10b);
    if (b) {
        _ZN8dActor_c9UpdatePosEP5dCc_c(c, (char *)&((daMky_c *)c)->mdCcAc_c);
        onWall = (func_ov030_02111dd0(c) | _ZNK10dBgCh_Actr8IsOnWallEv((char *)&((daMky_c *)c)->mWithMeshClsn)) != 0;
        func_ov030_02111f6c(c, (char *)&((daMky_c *)c)->mWithMeshClsn);
        func_ov030_02111bc4(c);
    } else {
        _ZN8dActor_c9UpdatePosEP5dCc_c(c, (char *)&((daMky_c *)c)->mdCcAc_c);
        func_ov030_02111f6c(c, (char *)&((daMky_c *)c)->mWithMeshClsn);
        func_ov030_02111bc4(c);
        onWall = (func_ov030_02111ea4(c) | _ZNK10dBgCh_Actr8IsOnWallEv((char *)&((daMky_c *)c)->mWithMeshClsn)) != 0;
    }

    if (((daMky_c *)c)->mState == 1 && onWall && dist < 0x12c000) {
        if (*(int *)((char *)p + 0x98) > 0x9000 || _ZN6Player7IsInAirEv(p) != 0) {
            ((daMky_c *)c)->mPrevState = 1;
            func_ov030_021141a8(c, 2);
        }
    }

    ((dCc_c *)((char *)&((daMky_c *)c)->mdCcAc_c))->Clear();
    if (((daMky_c *)c)->mState == 1)
        _ZN5dCc_c6UpdateEv((char *)&((daMky_c *)c)->mdCcAc_c);
    func_ov030_02111890(c);
    return 1;
}
}

// @symbol _ZN7daMky_c11EnterState1Ev
int daMky_c::EnterState1(){
  unk_3c7=0;
  mState=1;
  return 1;
}

// @symbol func_ov030_02113ff0
extern "C" {
int func_ov030_02113ff0(char* c){
    typedef int Fix12i;
    typedef short s16;
    extern int _ZN8dActor_c13DistToCPlayerEv(char* c);
    extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* ma, void* f, int b, Fix12i c, unsigned int d);
    extern void func_ov030_02111a00(char* c);
    extern s16 _ZN8dActor_c18HorzAngleToCPlayerEv(char* c);
    extern void _Z14ApproachLinearRsss(s16* p, s16 target, s16 step);
    extern void _ZN9Animation7AdvanceEv(char* a);
    extern void _ZN8dActor_c9UpdatePosEP5dCc_c(char* c, char* cl);
    extern void func_ov030_02111dd0(char* c);
    extern void func_ov030_02111f6c(char* c, char* w);
    extern void func_ov030_02111bc4(char* c);
    extern void func_ov030_02111ea4(char* c);
    extern void _ZN5dCc_c6UpdateEv(char* cl);
    extern void func_ov030_02111890(char* c);
    extern int data_ov030_02115d18[];
  int dist = ((daMky_c *)c)->DistToCPlayer();
  int b;
  if (dist > 0x15e000) {
    ((daMky_c *)c)->mHorzSpeed = 0xc000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&((daMky_c *)c)->mModelAnim, (void*)data_ov030_02115d18[1], 0, 0x1000, 0);
  } else if (dist <= 0xfa000) {
    ((daMky_c *)c)->mHorzSpeed = 0;
    func_ov030_02111a00(c);
  }
  _Z14ApproachLinearRsss((s16*)(c+0x8e), ((daMky_c *)c)->HorzAngleToCPlayer(), 0x514);
  ((daMky_c *)c)->mPrevAngleY = ((daMky_c *)c)->mAngleY;
  ((daMky_c *)c)->mModelAnim.speed = 0x1000;
  ((Animation *)(c + 0x124))->Advance();
  b = (((daMky_c *)c)->actorID == 0x10b);
  if (b) {
    ((daMky_c *)c)->UpdatePos(&((daMky_c *)c)->mdCcAc_c);
    func_ov030_02111dd0(c);
    func_ov030_02111f6c(c, &((daMky_c *)c)->mWithMeshClsn);
    func_ov030_02111bc4(c);
  } else {
    ((daMky_c *)c)->UpdatePos(&((daMky_c *)c)->mdCcAc_c);
    func_ov030_02111f6c(c, &((daMky_c *)c)->mWithMeshClsn);
    func_ov030_02111bc4(c);
    func_ov030_02111ea4(c);
  }
  ((daMky_c *)c)->mdCcAc_c.Clear();
  ((daMky_c *)c)->mdCcAc_c.Update();
  func_ov030_02111890(c);
  return 1;
}
}

// @symbol _ZN7daMky_c11EnterState0Ev
int daMky_c::EnterState0()
{
    mState = 0;
    return 1;
}

// @symbol func_ov030_02114134
typedef void (daMky_c::*PMF)();
extern "C" {
void func_ov030_02114134(daMky_c *self)
{
    PMF *p = (PMF *)self->mStateDesc + 1;
    (self->* *p)();
}
}

// @symbol func_ov030_02114170
extern "C" {
void func_ov030_02114170(daMky_c *self)
{
    PMF *p = (PMF *)self->mStateDesc;
    (self->* *p)();
}
}

// @symbol func_ov030_021141a8
extern "C" {
void func_ov030_021141a8(char *c, int idx)
{
    typedef struct { int a, b, c, d; } Item16;
    extern Item16 data_ov030_02115e0c[];
    daMky_c *self = (daMky_c *)c;

    self->mStateDesc = &data_ov030_02115e0c[idx];
    func_ov030_02114170(self);
}
}

/* ==========================================================================
 * The TU's second file-scope extern "C" region.
 *
 * func_ov030_021141a8, directly above, is ROM ordinal 37 and the last of this
 * TU's 24 func_ov030_* free functions; every one of them carries its own
 * declarations at block scope, where an extern in an extern "C" region still
 * gets C linkage.  Everything below this line is a daMky_c:: member, and
 * mwccarm 2004/b56 will not accept a linkage specification inside a function
 * body -- a block-scope `extern` written in a C++-named member gets C++
 * linkage and the reference mangles a second time.  So the six members below
 * share these declarations, and the region is placed here so that no free
 * function is affected by them.
 *
 * The eleven EnterState* members are C++-named too, but they are laid out
 * ROM-ascending among the free functions and so cannot be reached from here.
 * Their declarations live in the region at the TOP of the file instead, which
 * for that reason IS in view of every free function; the comment there records
 * what that cost and what it did not.
 *
 * Where two of the six recovered one symbol differently, one spelling is kept
 * and the call sites are cast; the disagreements were ClosestPlayer and Spawn
 * (pointer type only) and the four SharedFilePtr data objects (spelt `char` in
 * InitResources, `SharedFilePtr` in CleanupResources).  Members of this TU are
 * NOT declared here at all -- func_ov030_02111734, _02112094, _02114134 and
 * _021141a8 are all defined above and bind to their definitions.
 * ========================================================================== */
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
    char *c = (char *)this;
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
                func_ov030_021141a8(c, 1);
            }
        }
        func_ov030_02111734(c);
    } else {
        func_ov030_02114134(this);
        mModelAnim.UpdateVerts();
        func_ov030_02112094(c);
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
    char *c = (char *)this;

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
                func_ov030_021141a8(c, 1);
                goto ov030_cap_done;
            }
ov030_no_spawn:
            func_ov030_021141a8(c, 0);
ov030_cap_done:
            ;
        } else {
            func_ov030_021141a8(c, 0);
        }
    } else {
        int t = (int)(h == 0x10c);
        if (t != 0)
            func_ov030_021141a8(c, 1);
    }
    func_ov030_02112094(c);
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

