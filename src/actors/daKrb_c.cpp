//cpp
/* daKrb_c -- Goomba (KURIBO 200 / KURIBO_S 201 / KURIBO_L 202), ov084.
 *
 * ov084 is mixed (BOB_OMB_BUDDY / GOOMBA / PIRANHA_PLANT). RTTI names this
 * class daKrb_c; the debug table names KURIBO / KURIBO_S / KURIBO_L. One
 * class, three C-linkage factories. Base is dCapEnemy_c.
 *
 * Factories are `return new daKrb_c()`. `#pragma defer_codegen off` is
 * load-bearing: out-of-line D1 then D0 then homeless D2 matches the
 * cartridge (deferred codegen emits D2, D0, D1).
 *
 * Leftover, remeasured on this TU:
 * - ModelAnim::SetAnim, MaterialChanger::SetFile, dCcAc_c::Init,
 *   DropShadowRadHeight and KillByInvincibleChar take Fix12<int> by value.
 *   The header call size-DIFFs (0212a6f8, 02129a00, 0212a580, 02129ed4,
 *   InitResources). The bridges below pass those bits as scalars and still
 *   name the 5Fix12IiE symbols.
 * - dBgCh_Actr::Init stays the mangled free call. types.h makes Fix12i a
 *   plain s32, so the header method mangles as int and the link fails.
 * - Player::Hurt, Player::Bounce and dActor_c::IsTooFarAwayFromPlayer are
 *   not declared on those classes. The scalar bridges stay in this file.
 * - dBgCh_Actr::GetFloorResult and dCapEnemy_c::UpdateCapPos have no header
 *   member. UpdateCapPos still needs the unsigned Vector3_16_local and the
 *   equal-arm ternary so r2 is set up before r1.
 * - Animation::Advance is called on the subobject at +0x3c0. mModelAnim.Advance()
 *   this-adjusts.
 * - func_ov084_021290d4 recomputes the chase radius at +0x448. A named
 *   temporary size-DIFFs, and mTimer458 spelled the same way on both sides
 *   of the decrement CSEs the halfword.
 * - OnTurnIntoEgg and the reward helper load param1 through a raw word.
 *   Spelling both sides as the member CSEs +8.
 * - +0x448 chase radius, +0x452 bounce countdown and +0x45c target heading
 *   are unnamed in the class. mWanderRerollTimer is s16 in the header and
 *   this TU loads it unsigned.
 * - Flags / Flag stay. The bitfield test is lsl/lsrs; `&= ~n` on the u8
 *   member compiles to and, and the ROM has bic.
 * - data_ov084_02130cf8 is the BMD. 02130278[7] is the BCA table.
 *   02130ce0/ce8/cf0/cc0/cc8/cd0 are BCA handles (SetAnim reads [1]).
 *   0213089c / 0213088c are BMA. 02130258 / 02130208 / 02130228 /
 *   02130238 / 02130248 / 02130268 / 02130204 are the per-type tables.
 *   g_profile_KURIBO / _S / _L stay outside this text TU.
 * - Render keeps a volatile Vector3 backup of the scale (frame slot).
 * - func_ov002_020aea30 takes the receiver, the attacker, and a nullable
 *   collision pointer.
 */

#pragma defer_codegen off

#include "daKrb_c.h"
#include "common.h"
#include "dBgCh_Gnd.h"
#include "types.h"
#include "dBgCh_Actr.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "decl_dCapEnemy_c.h"
#include "MaterialChanger.h"
#include "Player.h"

namespace cstd { int fdiv(int a, int b); }

typedef struct {
    unsigned char b0 : 1;
    unsigned char flag : 1;
} Flags;

struct Vector3_16_local { unsigned short x, y, z; };

typedef struct { unsigned char b0 : 1; } Flag;

struct BMD_File;
struct BMA_File;

#define I(p,o)    (*(s32*)((char*)(p)+(o)))
#define U16f(p,o) (*(u16*)((char*)(p)+(o)))
#define S16f(p,o) (*(s16*)((char*)(p)+(o)))
#define U8f(p,o)  (*(u8*)((char*)(p)+(o)))
/* SharedFilePtr.h declares no fields; the loaded pointer is the second word. */
#define SHARED_FILE(h) (((void **)&(h))[1])

extern "C" {
/* data_ov084_02130cf8 is the BMD this TU LoadFile / Release. The other
   three handles are never passed to a SharedFilePtr method; this TU
   indexes [1] as the loaded BCA. */
extern void *data_ov084_02130ce0[];
extern void *data_ov084_02130ce8[];
extern void *data_ov084_02130cf0[];
extern SharedFilePtr data_ov084_02130cf8;
int func_02037e20(int* p);
void func_ov084_02129498(char* r0);
extern "C" void func_ov084_02129238(char* c);
extern void func_02012694(unsigned int id, const Vector3 *pos);
extern char* _ZNK10dBgCh_Actr14GetFloorResultEv(void*);
extern int SurfaceInfo_TestFlag0x20(int* p);
extern void func_ov084_021296cc(char *c);
extern int func_02037e38(unsigned int* p);
extern int func_02037e84(int* p);
extern void _ZN5dBgPiD1Ev(void*);
extern int data_02099368[];
extern void LinkSilverStarAndStarMarker(char *a, char *b);
extern u8 data_0209f208[];
extern u8 *data_0209f344;
extern void func_ov084_02129168(char *c, char *actor);
extern void _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(void *m, void *f, int a, int fix, unsigned int j);
/* Three-register reconstructed interface; death-state stores also use r3. */
extern void func_ov002_020aea30(void *self, void *actor, void *collision);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *m, void *f, int a, int fix, unsigned int j);
extern void func_ov084_021294d0(char *self);
extern int *data_ov084_0213088c;
extern int data_ov084_02130248[];
extern Fix12i Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);
extern short Vec3_HorzAngle(const struct Vector3 *a, const struct Vector3 *b);
extern void _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(void* thiz, s16* v, void* r6, s32 flag);
extern void _ZN6Player6BounceE5Fix12IiE(void* p, s32 f);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const Vector3* v, u32 a, s32 f, u8 b, u8 cc, u8 d);
extern void* data_ov084_02130cd0[];
extern u8 data_ov084_02130204[];
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* sm, void* mtx, int fix, int t, unsigned int j);
extern short data_02082214[];
extern "C" void _ZN11dCapEnemy_c12UpdateCapPosERK7Vector3RK10Vector3_16(void *, const Vector3&, const Vector3_16_local&);
extern char data_ov084_0213089c;
extern void func_ov084_02129c9c(char *c);
extern void func_ov084_02129cf4(char *self, int a);
extern int _Z14ApproachLinearRiii(int *a, int b, int c);
extern int RandomIntInternal(int *seed);
extern int data_ov084_02130228[];
extern int data_ov084_02130268[];
extern int data_0209e650;
extern void func_ov074_0212087c(Vector3 *out, void *player, u8 flag);
extern int ApproachAngle(s16 *cur, s16 target, int divisor, int band, int maxStep);
extern int Vec3_HorzDist(void *a, void *b);
extern unsigned char DecIfAbove0_Byte(void *p);
extern unsigned short DecIfAbove0_Short(void *p);
extern int Math_Function_0203b14c(int *v, int target, int a, int b, int c);
extern int data_ov084_02130cc8[];
extern void func_ov084_0212af74(char *c);
extern void func_ov084_0212abd4(char *c);
extern void UnloadBlueCoinModel(void* p);
extern s8 data_0209f2f8;
extern int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(char* c, int f);
void LoadSilverStarAndNumber(void);
void LoadBlueCoinModel(void* c);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, void* a, Fix12i r, Fix12i h, unsigned int e, unsigned int g);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, void* a, Fix12i b, Fix12i cc, void* d, Fix12i e);
void func_ov084_021290d4(char *c);
extern SharedFilePtr* data_ov084_02130278[7];
extern int data_ov084_02130258[];
extern int data_ov084_02130208[];
extern int data_ov084_02130238[];
}

// @symbol _ZN7daKrb_cD1Ev
// @symbol _ZN7daKrb_cD0Ev
daKrb_c::~daKrb_c()
{
}

// @symbol func_ov084_021290d4
/* Chase radius lives at +0x448 (four unnamed bytes ahead of mSavedParam).
 * The address has to be recomputed at each store: a named temporary, or
 * mTimer458 spelled the same way on both sides of the decrement, lets mwcc
 * CSE the field and the function stops matching. */
extern "C" {
inline char *chaseRadius(char *goomba)
{
    return goomba + 0x448;
}

void func_ov084_021290d4(char *c)
{
    int timerOff;
    char *self2;
    char *base = c + 0x400;
    int state;
    timerOff = 0x458;
    if ((*((unsigned short *)(base + 0x58))) != 0)
    {
        /* Comparison stays in the `if`: assigning `== 1` to an int is a bool
           in C++ and materialises moveq/movne. */
        state = (int)(*((int *)(c + 0x434)));
        if (state == 1)
        {
            *((unsigned short *)((c + 0x400) + 0x58)) = 0;
        }
        else
        {
            *((unsigned short *)((((int)c) + timerOff))) = (*((unsigned short *)((((int)c) + 0x458)))) - 1;
        }
        return;
    }
    if ((*((unsigned char *)((self2 = c) + 0x113))) < 6)
    {
        *((int *)chaseRadius(self2)) = 0x1f4000;
        return;
    }
    unsigned short stuck = *((unsigned short *)((c + 0x400) + 0x56));
    if (stuck > 0xa)
    {
        *((int *)chaseRadius(self2)) = 0x1f4000 - (((stuck - 0xa) * 0x14) << 12);
        if ((*((int *)chaseRadius(self2))) < 0xa000)
        {
            *((int *)chaseRadius(self2)) = 0xa000;
        }
        return;
    }
    *((int *)chaseRadius(self2)) = 0x1f4000;
}
}

// @symbol func_ov084_02129168
#include "decl_dBgCh_Actr.h"
extern "C" {

extern int Vec3_HorzLen(void* v);
extern short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* f, int a, int b, unsigned int e);
extern void func_02012694(unsigned int id, const Vector3 *pos);

void func_ov084_02129168(char* c, char* actor)
{
    daKrb_c *goomba = (daKrb_c *)c;
    /* +0x452 is the two unnamed bytes between mHeadingHoldTimer and
       mWanderRerollTimer. A short store there is the bounce countdown. */
    *(short *)((char *)goomba + 0x452) = 0x3c;
    goomba->mVertSpeed = cstd::fdiv(0xd000 - goomba->mVertAccel, goomba->mFloorNormalY);
    goomba->mHorzSpeed = Vec3_HorzLen((char *)&goomba->unk_0a4) * -1;
    if (actor != 0)
        goomba->mPrevAngleY = Vec3_HorzAngle((Vector3 *)&goomba->mPosX, (Vector3 *)(actor + 0x5c));
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&goomba->mModelAnim, *(void **)((char *)data_ov084_02130cc0 + 4), 0, 0x1000, 0);
    goomba->mState = 3;
    goomba->mEatenByYoshi = 0;
    goomba->mWithMeshClsn.SetLimMovFlag();
    _ZN10dBgCh_Actr12Unk_0203589cEv(&goomba->mWithMeshClsn);
    goomba->mWithMeshClsn.ClearJustHitGroundFlag();
    goomba->mWithMeshClsn.ClearGroundFlag();
    func_02012694(0x13a, (const Vector3 *)&goomba->mCamSpacePosX);
    goomba->unk_467 = 0;
}
}

// @symbol func_ov084_02129238
void func_ov084_02129238(char* c)
{
    daKrb_c *goomba = (daKrb_c *)c;
    if (goomba->mWithMeshClsn.IsOnGround() != 0)
        return;
    {
        Vector3 pos;
        {
            /* z before y: mwcc keeps that load order. */
            int vx = goomba->mPosX;
            int vz = goomba->mPosZ;
            int vy = goomba->mPosY + 0x190000;
            pos.x = vx;
            pos.y = vy;
            pos.z = vz;
        }
        dBgCh_Gnd rg;
        rg.StartDetectingWater();
        rg.StartDetectingToxic();
        rg.StopDetectingOrdinary();
        rg.SetObjAndPos(pos, goomba);
        if (rg.DetectClsn() != 0) {
            if (func_02037e20((int*)&rg.surface) != 0) {
                if (rg.clsnY != (int)0x80000000) {
                    if (goomba->mPosY < rg.clsnY) {
                        goomba->SpawnCoin();
                        goomba->PoofDust();
                        func_ov084_02129498((char*)c);
                        {
                            Vector3 cap;
                            cap.x = 0;
                            cap.y = 0x6c000;
                            cap.z = 0;
                            goomba->ReleaseCap(cap);
                        }
                        goomba->mPosX = goomba->mHomePos.x;
                        goomba->mPosY = goomba->mHomePos.y;
                        goomba->mPosZ = goomba->mHomePos.z;
                        goomba->RespawnIfHasCap();
                    }
                }
            }
        }
    }
}

// @symbol func_ov084_0212934c
extern "C" {
void func_ov084_0212934c(char* c)
{
    daKrb_c *goomba = (daKrb_c *)c;
    unsigned int kind;
    int frame;
    int type;

    if (goomba->mState != 0)
        return;

    if (!goomba->mWithMeshClsn.IsOnGround())
        return;

    /* Frame before the file pointer: that order is what colors the short
       extract and the file load into the registers the ROM uses. */
    frame = static_cast<Animation &>(goomba->mModelAnim).currFrame;
    kind = (unsigned short)((unsigned)frame >> 12);
    type = (int)goomba->mModelAnim.file;

    if (type == (int)data_ov084_02130ce8[1]) {
        if (kind <= 4 || (kind >= 0xc && kind <= 0x10)) {
            if (((Flags *)(c + 0x468))->flag)
                return;
            func_02012694(0xd0, (const Vector3 *)&goomba->mCamSpacePosX);
            goomba->mSoundLatchFlags |= 2;
            return;
        }
        *(unsigned char *)&goomba->mSoundLatchFlags &= ~2;
        return;
    }

    if (type == (int)data_ov084_02130cf0[1]) {
        if (kind <= 3 || (kind >= 0x10 && kind <= 0x13)) {
            if (((Flags *)(c + 0x468))->flag)
                return;
            func_02012694(0xd0, (const Vector3 *)&goomba->mCamSpacePosX);
            goomba->mSoundLatchFlags |= 2;
            return;
        }
        *(unsigned char *)&goomba->mSoundLatchFlags &= ~2;
    }
}
}

// @symbol func_ov084_02129498
extern "C" {
void func_ov084_02129498(char* r0) {
    daKrb_c *goomba = (daKrb_c *)r0;
    if ((goomba->mCapId & 0xf) < 6)
        goomba->MarkForDestruction();
    else
        goomba->KillAndTrackInDeathTable();
}
}

// @symbol func_ov084_021294d0
extern "C" void func_ov084_021294d0(char* c)
{
    daKrb_c *goomba = (daKrb_c *)c;
    char obj[0x28];
    if (!goomba->mWithMeshClsn.IsOnGround())
        return;

    char* fr = _ZNK10dBgCh_Actr14GetFloorResultEv(&goomba->mWithMeshClsn);
    if (SurfaceInfo_TestFlag0x20((int*)(fr + 4))) {
        func_ov084_021296cc(c);
        goomba->SpawnCoin();
        func_ov084_02129498(c);
        Vector3 v;
        v.x = 0; v.y = 0x6c000; v.z = 0;
        goomba->ReleaseCap(v);
        goomba->mPosX = goomba->mHomePos.x;
        goomba->mPosY = goomba->mHomePos.y;
        goomba->mPosZ = goomba->mHomePos.z;
        goomba->RespawnIfHasCap();
        return;
    }

    /* Copy the floor result into obj's own surface record, then ask what kind of
     * ground it is. The Goomba dies on some terrain types and acts on others. */
    char* floorResult = _ZNK10dBgCh_Actr14GetFloorResultEv(&goomba->mWithMeshClsn);
    int surfaceType;
    {
    char* surface = obj + 4;
    int normalX = *(int*)(floorResult + 4);
    int normalY = *(int*)(floorResult + 8);
    /* `normalY ? normalX : normalX` is not a typo and not dead: both arms are the
     * same value, and the ternary is what makes mwccarm materialize normalX after
     * the load of normalY instead of before it. Collapsing it to a plain store
     * reorders the pair and the function stops reproducing. */
    *(int*)(surface) = normalY ? normalX : normalX;
    *(int*)(surface + 4) = normalY;
    *(int*)(surface + 8) = *(int*)(floorResult + 0xc);
    *(int*)(surface + 0xc) = *(int*)(floorResult + 0x10);
    *(int*)(surface + 0x10) = *(int*)(floorResult + 0x14);
    *(int*)(obj) = (int)data_02099368;
    *(unsigned short*)(obj + 0x18) = *(unsigned short*)(floorResult + 0x18);
    *(unsigned short*)(obj + 0x1a) = *(unsigned short*)(floorResult + 0x1a);
    *(int*)(obj + 0x1c) = *(int*)(floorResult + 0x1c);
    *(int*)(obj + 0x20) = *(int*)(floorResult + 0x20);
    *(int*)(obj + 0x24) = *(int*)(floorResult + 0x24);
    surfaceType = func_02037e38((unsigned int*)surface);
    }
    if (func_02037e84((int*)(obj + 4)) == 8) {
        if (surfaceType == 6 || surfaceType == 7 || surfaceType == 8 || surfaceType == 9)
            goto action;
    }
    if (surfaceType == 0x13 || surfaceType == 1)
        goto action;
    if ((unsigned)(surfaceType - 4) > 1)
        goto dtor;
action:
    goomba->SpawnCoin();
    func_ov084_02129498(c);
    if ((goomba->mCapId & 0xf) < 6 || goomba->mRewardType == 2) {
        goomba->mPosX = goomba->mHomePos.x;
        goomba->mPosY = goomba->mHomePos.y;
        goomba->mPosZ = goomba->mHomePos.z;
        Vector3 v2;
        v2.x = 0; v2.y = 0x6c000; v2.z = 0;
        goomba->ReleaseCap(v2);
        goomba->RespawnIfHasCap();
    }
dtor:
    _ZN5dBgPiD1Ev(obj);
}

// @symbol func_ov084_021296b0
extern "C" {
void func_ov084_021296b0(int *a, int *b)
{
    daKrb_c *goomba = (daKrb_c *)a;
    goomba->mHomePos.x = b[0];
    goomba->mHomePos.y = b[1];
    goomba->mHomePos.z = b[2];
}
}

// @symbol func_ov084_021296cc
extern "C" {
void func_ov084_021296cc(char *c)
{
    daKrb_c *goomba = (daKrb_c *)c;
    if (goomba->mRewardType == 1) {
        char *star;
        char *marker;
        goomba->UntrackStar(goomba->mStarTracked);
        star = (char *)dActor_c::Spawn(
            0xb4, 0x50, goomba->mHomePos, 0, goomba->mAreaId, -1);
        marker = (char *)dActor_c::Spawn(
            0xb3, 0x10, *(Vector3 *)&goomba->mPosX, 0, goomba->mAreaId, -1);
        if (star != 0 && marker != 0) {
            *(int *)(marker + 0x434) = *(int *)(star + 4);
            LinkSilverStarAndStarMarker(star, marker);
            goomba->SpawnSoundObj(1);
        }
        /* Load side stays a raw word: spelling both sides as param1 CSEs +8. */
        goomba->param1 = *(const int *)((const char *)goomba + 8) & 0xff0f;
        return;
    }
    if (goomba->mRewardType != 2)
        return;
    if (goomba->mStarID != data_0209f344[data_0209f208[0]])
        return;
    goomba->UntrackStar(goomba->mStarTracked);
    dActor_c::Spawn(
        0xb4, goomba->mStarID | 0x30, *(Vector3 *)&goomba->mPosX, 0,
        goomba->mAreaId, -1);
    dActor_c::Spawn(
        0xb3, goomba->mStarID | 0x30, *(Vector3 *)&goomba->mPosX, 0,
        goomba->mAreaId, -1);
    goomba->mRewardType = 3;
    goomba->param1 = *(const int *)((const char *)goomba + 8) & 0xff0f;
    goomba->SpawnSoundObj(1);
}
}

// @symbol func_ov084_02129864
extern "C" {
void func_ov084_02129864(char *c){
    daKrb_c *goomba = (daKrb_c *)c;
    if (goomba->mRewardType != 2)
        return;
    if (goomba->mStarTracked >= 0)
        return;
    unsigned int current = data_0209f344[data_0209f208[0]];
    if (goomba->mStarID != current)
        return;
    goomba->mStarTracked = (s8)goomba->TrackStar(goomba->mStarID, 1);
}
}

// @symbol func_ov084_021298d0
extern "C" {
void func_ov084_02129498(char* r0);
void func_02012694(unsigned int id, const Vector3 *pos);
extern int data_ov084_02130218[];

int func_ov084_021298d0(char* c){
    daKrb_c *goomba = (daKrb_c *)c;
    int deathState = goomba->UpdateDeath(goomba->mWithMeshClsn);
    if ((unsigned int)(goomba->mDeathState - 2) > 4) goto L_a4;
    static_cast<Animation &>(goomba->mModelAnim).speed = 0x1000;
    ((Animation *)((char *)goomba + 0x3c0))->Advance();
    if (goomba->mGoombaType != 3) goto L_a4;

    /* SpawnCoin only when the actor we hit is type 0xc6. The flag and the
       cylinder update always run for a giant Goomba in this death phase. */
    unsigned int id = goomba->mdCcAc_c.otherOwner;
    if (id != 0) {
        char* r = (char*)dActor_c::FindWithID(id);
        if (r != 0) {
            int b = (*(unsigned short*)(r + 0xc) == 0xc6);
            if (b != 0) {
                goomba->SpawnCoin();
                func_ov084_02129498(c);
            }
        }
    }
    /* Integer cast forces add rN, rBase, #0x198. A named flags |= CSEs it. */
    *(int*)(((int)c + 0x198)) |= 0x20000;
    goomba->mdCcAc_c.Clear();
    goomba->mdCcAc_c.Update();

L_a4:
    if (deathState == 0) goto L_end;
    func_02012694(data_ov084_02130218[goomba->mGoombaType], (const Vector3 *)&goomba->mCamSpacePosX);
    func_ov084_021296cc(c);
    if ((goomba->mCapId & 0xf) < 6 || goomba->mRewardType == 2) {
        goomba->mPosX = goomba->mHomePos.x;
        goomba->mPosY = goomba->mHomePos.y;
        goomba->mPosZ = goomba->mHomePos.z;
        goomba->RespawnIfHasCap();
    }
    if ((goomba->mCapId & 0xf) < 6)
        goomba->UntrackInDeathTable();
L_end:
    return deathState;
}
}

// @symbol func_ov084_02129a00
extern "C" {
int func_ov084_02129a00(char *self) {
    int eatState = ((dEnemyBase_c *)self)->UpdateYoshiEat(*(dBgCh_Actr *)(self + 0x1b4));
    if (eatState == 0)
        goto ret0;
    if (eatState == 1) {
        Vector3 v;
        char *actor = *(char **)(self + 0xd0);
        v.x = 0;
        v.y = 0x6c000;
        v.z = 0;
        if (((dCapEnemy_c *)self)->GetCapEatenOffIt(v) != 0) {
            func_ov084_02129168(self, actor);
            *(int *)(self + 0x98) = -0xf000;
            *(int *)(self + 0xa8) = 0x14000;
            MaterialChanger::Prepare(*(BMD_File *)(SHARED_FILE(data_ov084_02130cf8)), *(BMA_File *)&data_ov084_0213088c);
            _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(self + 0x3fc, &data_ov084_0213088c, 0x40000000, 0x1000, 0);
            *(int *)(self + 0x404) = 0;
            ((dCc_c *)(self + 0x180))->Clear();
            return 0;
        }
    } else if (eatState == 3) {
        if (((dBgCh_Actr *)(self + 0x1b4))->IsOnGround())
            *(int *)(((int)self + 0x98)) >>= 1;
    }

    if (((dEnemyBase_c *)self)->SpawnParticlesIfHitOtherObj(*(dCc_c *)(self + 0x180)) != 0) {
        void *actor = dActor_c::FindWithID(*(int *)(self + 0x1a4));
        *(int *)(self + 0x10c) = 7;
        func_ov002_020aea30(self, actor, self + 0x1b4);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x370, data_ov084_02130ce0[1], 0x40000000, 0x1000, 0);
        *(int *)(((int)self + 0x198)) |= 1;
        return 1;
    }

    ((daKrb_c *)self)->func_ov084_0212a580();
    ((dCc_c *)(self + 0x180))->Clear();
    if (*(u8 *)(self + 0x107) != 0) {
        func_ov084_021294d0(self);
        {
            u16 s = *(u16 *)(self + 0x104);
            if (s == 0) {
                ((dCc_c *)(self + 0x180))->Update();
            } else if (s == 5) {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x370, data_ov084_02130ce0[1], 0x40000000, 0x1000, 0);
                *(s16 *)(((int)self + 0x94)) += 0x8000;
                *(int *)(self + 0x98) = -*(int *)(self + 0x98);
                MaterialChanger::Prepare(*(BMD_File *)(SHARED_FILE(data_ov084_02130cf8)), *(BMA_File *)&data_ov084_0213088c);
                _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(self + 0x3fc, &data_ov084_0213088c, 0x40000000, 0x1000, 0);
                *(int *)(self + 0x404) = 0;
            }
        }
        ((Animation *)(self + 0x3c0))->Advance();
        if (((dBgCh_Actr *)(self + 0x1b4))->JustHitGround())
            func_ov084_02129168(self, 0);
    }

    if (*(int *)(self + 0x460) != 3)
        goto ret1;
    if (eatState < 3)
        goto ret1;
    if (*(int *)(self + 0x60) >= *(int *)(self + 0x420) - 0x3e8000)
        goto ret1;
    ((fBase_c *)self)->MarkForDestruction();
    return 1;
ret1:
    return 1;
ret0:
    return 0;
}
}

// @symbol func_ov084_02129c9c
extern "C" {
void func_ov084_02129c9c(char *c)
{
    daKrb_c *goomba = (daKrb_c *)c;
    func_02012694(0x118, (const Vector3 *)&goomba->mCamSpacePosX);
    goomba->mState = 2;
    goomba->mHorzSpeed = 0;
    goomba->mVertSpeed = data_ov084_02130248[goomba->mGoombaType];
    goomba->mWithMeshClsn.ClearGroundFlag();
    /* Integer cast keeps the orr on a freshly formed +0x198, not a CSE of flags. */
    *((int *)((char *)(((int)((char *)goomba + 0x198)) + 0))) |= 4;
}
}

// @symbol func_ov084_02129cf4
extern "C" {
void func_ov084_02129cf4(char *c, Fix12i distThresh)
{
    struct Vector3 ppos;

    *(void **)(c + 0x438) = ((dActor_c *)c)->ClosestPlayer();

    if (*(void **)(c + 0x438) == 0
        || (Vec3_Dist((struct Vector3*)(c+0x5c), (struct Vector3*)(c+0x41c)) > distThresh
            && *(unsigned char*)(c+0x113) >= 6)) {
        *(short*)(c+0x400+0x5a) = Vec3_HorzAngle((struct Vector3*)(c+0x5c), (struct Vector3*)(c+0x41c));
        *(int*)(c+0x440) = 0x61a8000;
        return;
    }

    {
        int *ppos_src = (int *)(int)(*(char **)(c + 0x438) + 0x5c);
        ppos.x = ppos_src[0];
        ppos.y = ppos_src[1];
        ppos.z = ppos_src[2];
    }

    if (*(unsigned char*)(c+0x113) < 6) {
        if (Vec3_Dist((struct Vector3*)(c+0x5c), (struct Vector3*)(c+0x41c)) > distThresh
            && !((dBgCh_Actr *)(c+0x1b4))->IsOnWall()) {
            *(int*)(c+0x440) = 0x61a8000;
            *(short*)(c+0x400+0x5a) = Vec3_HorzAngle((struct Vector3*)(c+0x5c), (struct Vector3*)(c+0x41c));
            return;
        }

        if (Vec3_Dist((struct Vector3*)(c+0x5c), &ppos) < *(int*)(c+0x448)) {
            *(int*)(c+0x440) = Vec3_Dist((struct Vector3*)(c+0x5c), &ppos);
            if (*(unsigned short*)(c+0x400+0x58) != 0) {
                *(short*)(c+0x400+0x5a) = Vec3_HorzAngle((struct Vector3*)(c+0x5c), &ppos);
                return;
            }
            *(short*)(c+0x400+0x5a) = Vec3_HorzAngle(&ppos, (struct Vector3*)(c+0x5c));
            return;
        }
        *(int*)(c+0x440) = 0x61a8000;
        return;
    }

    if (Vec3_Dist((struct Vector3*)(c+0x41c), &ppos) > distThresh) {
        *(int*)(c+0x440) = 0x61a8000;
        return;
    }
    *(int*)(c+0x440) = Vec3_Dist((struct Vector3*)(c+0x5c), &ppos);
    *(short*)(c+0x400+0x5a) = Vec3_HorzAngle((struct Vector3*)(c+0x5c), &ppos);
}
}

// @symbol func_ov084_02129ed4
extern "C" {
/* Collision reaction: something touched this Goomba, decide what it did.
 *
 * `flags` is the collision word at 0x1a0 and `other` the actor found through the
 * hit id at 0x1a4. Type ids are the debug-table profile numbers this class is
 * registered under -- 0xc8 KURIBO, 0xc9 KURIBO_S, 0xca KURIBO_L, and 0xbf for a
 * Player -- so `myType` is which of the three Goomba sizes WE are, not what hit
 * us. The outcome is either death (cap released, then KillByInvincibleChar with
 * a knockback direction), a stomp (Player::Bounce), or hurting the player.
 *
 * `variantMatch` and `typeMatch` are scratch booleans, and materializing them
 * instead of testing inline is deliberate: mwccarm emits the comparison into a
 * register and then re-tests it, which is the shape the cartridge has. Folding
 * either back into its `if` collapses the pair. See notes/matching-style.md 3. */
void func_ov084_02129ed4(void* c)
{
    s16 killDirNormal[3];
    s16 killDirVariant[3];
    s16 killDirPlayer[3];
    volatile Vector3 playerPos;
    volatile Vector3 playerPosJump;
    Vector3 capReleaseOnHit;
    Vector3 capReleaseOnKill;
    Vector3 hurtOriginFirstHit;
    Vector3 hurtOriginRepeat;
    Vector3 hurtOriginJumped;
    Vector3 capReleaseOnExit;
    void* other;
    u32 flags;
    s32 turnAround;
    s32 hurtKnockback;
    u16 myType;
    s32 variantMatch;
    s32 typeMatch;
    u32 id;

    id = *(u32*)((char*)c + 0x1a4);
    if (id == 0) return;
    other = dActor_c::FindWithID(id);
    if (other == 0) return;

    flags = I(c, 0x1a0);
    hurtKnockback = 0xc000;
    I(c, 0x46c) = flags;
    myType = U16f(c, 0xc);
    turnAround = 0;
    if (I(c, 0x460) == 3) hurtKnockback = 0x5000;
    variantMatch = (s32)(myType == 0xc9);

    if (variantMatch == 0 && (flags & 0x10)) {
        capReleaseOnHit.x = 0; capReleaseOnHit.y = 0x6c000; capReleaseOnHit.z = 0;
        ((dCapEnemy_c *)c)->ReleaseCap(capReleaseOnHit);
        typeMatch = (s32)(U16f(c, 0xc) == 0xc8);
        if (typeMatch != 0) {
            killDirNormal[0] = -0x2000; killDirNormal[1] = 0; killDirNormal[2] = 0;
            _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, killDirNormal, other, 0x41000);
            return;
        }
        killDirVariant[0] = -0x1800; killDirVariant[1] = 0; killDirVariant[2] = 0;
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, killDirVariant, other, 0x96000);
        return;
    }

    if (flags & 0x20) {
        I(c, 0x10c) = 1;
        if (I(c, 0x460) == 2) U8f(c, 0x108) = 3;
        I(c, 0x80) = 0x1000;
        I(c, 0x84) = 0x1000;
        I(c, 0x88) = 0x1000;
        func_02012694(0xe0, (const ::Vector3 *)((char*)c + 0x74));
        goto block_68;
    }

    if (flags & 0x40000) {
        turnAround = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130cd0[1], 0x40000000, 0x1000, 0);
        I(c, 0x10c) = 4;
        goto block_68;
    }

    variantMatch = (s32)(myType == 0xca);
    if (variantMatch == 0) {
        if (flags & 0x20000) {
            I(c, 0x10c) = 7;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130ce0[1], 0x40000000, 0x1000, 0);
            *(s32*)(((int)c + 0x198) & 0xffffffffffffffffULL) |= 1;
            goto block_68;
        }
        if (flags & 0x2400) {
            I(c, 0x10c) = 5;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130ce0[1], 0x40000000, 0x1000, 0);
            goto block_68;
        }
        if (flags & 0x4000) {
            I(c, 0x10c) = 6;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130cd0[1], 0x40000000, 0x1000, 0);
            goto block_68;
        }
        if (flags & 0x380) {
            turnAround = 1;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130cd0[1], 0x40000000, 0x1000, 0);
            I(c, 0x10c) = 3;
            goto block_68;
        }
        if (flags & 0x40) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130ce0[1], 0x40000000, 0x1000, 0);
            I(c, 0x10c) = 2;
            turnAround = 1;
            goto block_68;
        }
        if (!(flags & 0x8000)) {
            typeMatch = (s32)(U16f(other, 0xc) == 0xbf);
            if (typeMatch != 0) {
                if (U8f(other, 0x6f9) != 0) {
                    capReleaseOnKill.x = 0; capReleaseOnKill.y = 0x6c000; capReleaseOnKill.z = 0;
                    ((dCapEnemy_c *)c)->ReleaseCap(capReleaseOnKill);
                    killDirPlayer[0] = 0x2000; killDirPlayer[1] = 0; killDirPlayer[2] = 0;
                    _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, killDirPlayer, other, 0x41000);
                    return;
                }
                { Vector3* pp = (Vector3*)(((int)other + 0x5c) & 0xffffffffffffffffULL); playerPos.x = pp->x; playerPos.y = pp->y; playerPos.z = pp->z; }
                if (((Player *)other)->IsOnShell() != 0) {
                    I(c, 0x10c) = 5;
                    turnAround = 1;
                    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130ce0[1], 0x40000000, 0x1000, 0);
                    goto block_68;
                }
                if (((dActor_c *)c)->JumpedOnByPlayer(*(dCc_c *)((char*)c + 0x180), *(Player *)other) != 0) {
                    _ZN6Player6BounceE5Fix12IiE(other, 0x28000);
                    func_02012694(0xe0, (const ::Vector3 *)((char*)c + 0x74));
                    I(c, 0x10c) = 1;
                    I(c, 0x80) = 0x1000;
                    I(c, 0x84) = 0x1000;
                    I(c, 0x88) = 0x1000;
                    goto block_68;
                }
                if (U8f(other, 0x6fb) != 0) return;
                if (I(c, 0x434) == 0) {
                    if (I(c, 0x460) == 0) {
                        ((dActor_c *)c)->SmallPoofDust();
                        hurtOriginFirstHit.x = I(c, 0x5c); hurtOriginFirstHit.y = I(c, 0x60); hurtOriginFirstHit.z = I(c, 0x64);
                        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(other, &hurtOriginFirstHit, 0, hurtKnockback, 1, 0, 1);
                        func_ov084_02129498((char*)c);
                        func_02012694(0x110, (const ::Vector3 *)((char*)c + 0x74));
                        return;
                    }
                    if ((I(c, 0x1a0) & 0x400000) == 0) return;
                    hurtOriginRepeat.x = I(c, 0x5c); hurtOriginRepeat.y = I(c, 0x60); hurtOriginRepeat.z = I(c, 0x64);
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(other, &hurtOriginRepeat, data_ov084_02130204[I(c, 0x460)], hurtKnockback, 1, 0, 1);
                    I(c, 0x434) = 1;
                    return;
                }
                goto block_68;
            }
            goto block_68;
        }
        goto block_68;
    }

    typeMatch = (s32)(U16f(other, 0xc) == 0xbf);
    if (typeMatch != 0) {
        { Vector3* pp = (Vector3*)(((int)other + 0x5c) & 0xffffffffffffffffULL); playerPosJump.x = pp->x; playerPosJump.y = pp->y; playerPosJump.z = pp->z; }
        if (((dActor_c *)c)->JumpedOnByPlayer(*(dCc_c *)((char*)c + 0x180), *(Player *)other) != 0) {
            _ZN6Player6BounceE5Fix12IiE(other, 0x28000);
            func_02012694(0xe0, (const ::Vector3 *)((char*)c + 0x74));
            I(c, 0x10c) = 1;
            I(c, 0x80) = 0x1000;
            I(c, 0x84) = 0x1000;
            I(c, 0x88) = 0x1000;
            goto block_68;
        }
        if (U8f(other, 0x6fb) != 0) return;
        if (I(c, 0x434) == 0) {
            I(c, 0x434) = 1;
            if ((I(c, 0x1a0) & 0x400000) == 0) return;
            hurtOriginJumped.x = I(c, 0x5c); hurtOriginJumped.y = I(c, 0x60); hurtOriginJumped.z = I(c, 0x64);
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(other, &hurtOriginJumped, data_ov084_02130204[I(c, 0x460)], hurtKnockback, 1, 0, 1);
            return;
        }
        goto block_68;
    }

block_68:
    if (I(c, 0x10c) != 0) {
        capReleaseOnExit.x = 0; capReleaseOnExit.y = 0x6c000; capReleaseOnExit.z = 0;
        ((dCapEnemy_c *)c)->ReleaseCap(capReleaseOnExit);
    }
    func_ov002_020aea30(c, other, (char*)c + 0x1b4);
    if (turnAround != 0) {
        S16f(c, 0x8e) = (s16)(S16f(c, 0x94) + 0x8000);
    }
    if (I(c, 0x460) != 3) return;
    if ((I(c, 0x46c) & 0x40) || (I(c, 0x46c) & 0x380)) {
        *(s32*)(((int)c + 0x98) & 0xffffffffffffffffULL) += I(other, 0x98);
    }
    if (I(c, 0x46c) & 0x400) {
        *(s32*)(((int)c + 0x98) & 0xffffffffffffffffULL) += 0x20;
    }
}
}

// @symbol _ZN7daKrb_c19func_ov084_0212a580Ev
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
void daKrb_c::func_ov084_0212a580(){
    Vector3_16_local rotation;
    Vector3 pos;
    Vector3 arg;
    Vector3_16_local arg16;

    Matrix4x3_FromRotationY((char *)this + 0x38c, *(short*)((char *)this + 0x8e));
    *(int*)((char *)this + 0x3b0) = *(int*)((char *)this + 0x5c) >> 3;
    *(int*)((char *)this + 0x3b4) = *(int*)((char *)this + 0x60) >> 3;
    *(int*)((char *)this + 0x3b8) = *(int*)((char *)this + 0x64) >> 3;
    rotation.x = *(short*)((char *)this + 0x8c);
    rotation.y = *(short*)((char *)this + 0x8e);
    rotation.z = *(short*)((char *)this + 0x90);
    if ((*(int*)((char *)this + 0xb0) & 0x40000 ? 1 : 0) == 0) {
        if (((dBgCh_Actr *)((char *)this + 0x1b4))->IsOnGround()) {
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j((char *)this, (char *)this + 0x3d4, (char *)this + 0x38c, *(int*)((char *)this + 0x80) * 0x50, 0x1e000, 0xf);
        } else {
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j((char *)this, (char *)this + 0x3d4, (char *)this + 0x38c, *(int*)((char *)this + 0x80) * 0x50, 0x96000, 0xf);
        }
    }
    pos.x = 0;
    pos.z = 0;
    pos.y = 0x6c000;
    pos.x += ((short*)data_02082214)[(*(unsigned short*)((char *)this + 0x8e) >> 4) * 2] * 10;
    pos.z += ((short*)data_02082214)[(*(unsigned short*)((char *)this + 0x8e) >> 4) * 2 + 1] * 10;
    arg.x = ((int*)&pos)[0];
    arg.y = ((int*)&pos)[1];
    arg.z = ((int*)&pos)[2];
    arg16.x = ((unsigned short*)&rotation)[0];
    arg16.y = ((unsigned short*)&rotation)[1];
    arg16.z = ((unsigned short*)&rotation)[2];
    /* equal-arm ternary forces arg16 setup (r2) before arg (r1) — matches ROM call-arg order */
    _ZN11dCapEnemy_c12UpdateCapPosERK7Vector3RK10Vector3_16((dCapEnemy_c*)this, arg, this ? arg16 : arg16);
}

// @symbol func_ov084_0212a6f8
extern "C" {
void func_ov084_0212a6f8(char *c)
{
    daKrb_c *goomba = (daKrb_c *)c;
    goomba->mHorzSpeed = 0;
    /* Header spells this s16; the body loads it unsigned, and the two
       address forms stop mwcc from CSEing the halfword. */
    if (*(unsigned short *)((char *)goomba + 0x400 + 0x54)) {
        *(unsigned short *)(((int)goomba + 0x454)) -= 1;
    }
    if (*(unsigned short *)((char *)goomba + 0x400 + 0x54))
        return;
    goomba->mState = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&goomba->mModelAnim, data_ov084_02130ce8[1], 0, 0x1000, 0);
}
}

// @symbol func_ov084_0212a774
extern "C" {
void func_ov084_0212a774(char *c)
{
    Vector3 v;
    u16 h = *(u16 *)(c + 0x400 + 0x52);

    if (h == 0) {
        ((Animation *)(c + 0x3fc))->Advance();
        if (((Animation *)(c + 0x3c0))->Finished() == 0)
            return;
        *(s32 *)(c + 0xb0) = *(s32 *)(c + 0x44c);
        *(s32 *)(c + 0x434) = 0;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, data_ov084_02130ce8[1], 0, 0x1000, 0);
        *(int *)(c + 0x444) = data_ov084_02130228[*(s32 *)(c + 0x460)];
        ((dBgCh_Actr *)(c + 0x1b4))->ClearLimMovFlag();
        {
            s32 *f198 = (s32 *)(((long long)(int)(c + 0x198)));
            *(s16 *)(c + 0x94) = *(s16 *)(c + 0x8e);
            *f198 = *f198 & ~0x20000;
        }
        MaterialChanger::Prepare(*(BMD_File *)(SHARED_FILE(data_ov084_02130cf8)), *(BMA_File *)&data_ov084_0213089c);
        _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(c + 0x3fc, &data_ov084_0213089c, 0x40000000, 0x1000, 0);
        *(s32 *)(c + 0x404) = 0;
        return;
    }
    if (h <= 0x3c) {
        *(u16 *)(((long long)(int)(c + 0x452))) -= 1;
        if (*(u16 *)(c + 0x400 + 0x52) == 0) {
            func_ov084_021296cc(c);
            ((dEnemyBase_c *)c)->SpawnCoin();
            func_ov084_02129498(c);
            v.x = 0;
            v.y = 0x6c000;
            v.z = 0;
            ((dCapEnemy_c *)c)->ReleaseCap(v);
            *(s32 *)(c + 0x5c) = *(s32 *)(c + 0x41c);
            *(s32 *)(c + 0x60) = *(s32 *)(c + 0x420);
            *(s32 *)(c + 0x64) = *(s32 *)(c + 0x424);
            ((dCapEnemy_c *)c)->RespawnIfHasCap();
        }
    }
    if (((dBgCh_Actr *)(c + 0x1b4))->JustHitGround() != 0) {
        int a8;
        int cnt;
        if (*(u16 *)(c + 0x452) > 0x3c)
            *(u16 *)(c + 0x452) = 0x1e;
        a8 = *(s32 *)(c + 0xa8);
        cnt = *(u16 *)(c + 0x452);
        {
            int aa = a8 < 0 ? -a8 : a8;
            if (aa <= cnt * 0x500) {
                *(s32 *)(c + 0xa8) = (cnt << 0xa) - *(s32 *)(c + 0x9c);
            } else {
                *(s32 *)(c + 0xa8) = cstd::fdiv((a8 * -0x50) / 100, *(s32 *)(c + 0xd8));
            }
        }
        {
            s32 *p98 = (s32 *)(((long long)(int)(c + 0x98)));
            *p98 >>= 1;
        }
        {
            int v98 = *(s32 *)(c + 0x98);
            int av = v98 < 0 ? -v98 : v98;
            if (av < 0x5000) {
                if (v98 < 0)
                    *(s32 *)(c + 0x98) = -0x5000;
                else
                    *(s32 *)(c + 0x98) = 0x5000;
            }
        }

        if (*(u8 *)(c + 0x467) == 0) {
            func_02012694(0x13a, (const ::Vector3 *)(c + 0x74));
        } else if (*(u8 *)(c + 0x467) <= 2) {
            func_02012694(0x13b, (const ::Vector3 *)(c + 0x74));
        }
        *(u8 *)(((long long)(int)(c + 0x467))) += 1;
    } else {
        if (((dBgCh_Actr *)(c + 0x1b4))->IsOnGround() != 0) {
            ((dBgCh_Actr *)(c + 0x1b4))->ClearLimMovFlag();
            *(s32 *)(c + 0x98) = 0;
            *(s32 *)(c + 0xa8) = 0;
            if (*(u16 *)(c + 0x452) > 0x3c)
                *(u16 *)(c + 0x452) = 0x1e;
        }
    }
    *(u8 *)(c + 0x107) = 1;
    if (((dEnemyBase_c *)c)->SpawnParticlesIfHitOtherObj(*(dCc_c *)(c + 0x180)) != 0) {
        void *a = dActor_c::FindWithID(*(u32 *)(c + 0x1a4));
        *(s32 *)(c + 0x10c) = 7;
        func_ov002_020aea30(c, a, c + 0x1b4);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, data_ov084_02130ce0[1], 0x40000000, 0x1000, 0);
        {
            s32 *f198 = (s32 *)(((long long)(int)(c + 0x198)));
            *f198 |= 1;
        }
    }
    *(u8 *)(c + 0x107) = 0;
}
}

bool ApproachLinear(short &value, short target, short step);

// @symbol _ZN7daKrb_c19func_ov084_0212aab0Ev
void daKrb_c::func_ov084_0212aab0()
{
    if (mWithMeshClsn.JustHitGround()) {
        switch (mGoombaType) {
        case 2: HugeLandingDust(true); break;
        case 1: LandingDust(true); break;
        }
    }
    if (mWithMeshClsn.IsOnGround()) {
        mState = 0;
        mdCcAc_c.flags &= ~4;
    } else {
        ApproachLinear(mPrevAngleY, *(short *)((char *)this + 0x45c), 0x800);
    }
    mAngleY = mPrevAngleY;
}

// @symbol func_ov084_0212ab48
extern "C" {
void func_ov084_0212ab48(char *c)
{
    daKrb_c *goomba = (daKrb_c *)c;
    int large;
    func_ov084_02129c9c(c);
    large = (int)(goomba->actorID == (unsigned short)0xca);
    if (large != 0) {
        goomba->mVertSpeed = (int)(((long long)goomba->mVertSpeed * 0x1800 + 0x800) >> 0xc);
    }
    *(short *)((char *)goomba + 0x45c) = goomba->mInitAngleY;
    *(unsigned char *)&goomba->mSoundLatchFlags &= ~1;
    goomba->mAngleY = goomba->mPrevAngleY;
}
}

// @symbol func_ov084_0212abd4
extern "C" {
void func_ov084_0212abd4(char *self)
{
    short step = 0x200;
    func_ov084_02129cf4(self, 0x3e8000);
    _Z14ApproachLinearRiii((int *)(self + 0x98), *(int *)(self + 0x444), 0x500);
    if (((Flag *)(self + 0x468))->b0) {
        if (ApproachLinear(*(short *)(self + 0x94), *(s16 *)(self + 0x45a), step)) {
            *(unsigned char *)(((int)self + 0x468)) &= ~1;
            return;
        }
        {
            unsigned char *p = (unsigned char *)(((int)self + 0x468));
            *p = (*p & ~1) | 1;
        }
        return;
    }
    if (*(u16 *)(self + 0x458) != 0) {
        if (*(unsigned char *)(self + 0x113) < 6) {
            if (*(int *)(self + 0x444) <= data_ov084_02130228[*(int *)(self + 0x460)]) {
                func_ov084_02129c9c(self);
            } else {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x370, data_ov084_02130cf0[1], 0, 0x1000, 0);
            }
            step = 0x800;
            *(int *)(self + 0x444) = data_ov084_02130268[*(int *)(self + 0x460)];
            *(s16 *)(self + 0x45c) = *(s16 *)(self + 0x45a);
        } else {
            *(int *)(self + 0x444) = data_ov084_02130228[*(int *)(self + 0x460)];
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x370, data_ov084_02130ce8[1], 0, 0x1000, 0);
            *(s16 *)(self + 0x45c) = Vec3_HorzAngle((Vector3 *)(self + 0x5c), (Vector3 *)(self + 0x41c));
            step = 0x400;
        }
        ApproachLinear(*(short *)(self + 0x94), *(s16 *)(self + 0x45c), step);
        return;
    }
    if (*(int *)(self + 0x440) >= 0x61a8000) {
        *(s16 *)(self + 0x45c) = *(s16 *)(self + 0x45a);
        *(s16 *)(self + 0x450) = 0x19;
    }
    {
        int bit = ((dEnemyBase_c *)self)->AngleAwayFromWallOrCliff(*(dBgCh_Actr *)(self + 0x1b4), *(s16 *)(self + 0x45c));
        unsigned char *p = (unsigned char *)(((int)self + 0x468));
        bit &= 1;
        *p = (*p & ~1) | bit;
    }
    if (!((Flag *)(self + 0x468))->b0) {
        if (*(int *)(self + 0x440) < *(int *)(self + 0x448) ||
            (*(unsigned char *)(self + 0x113) < 6 &&
             Vec3_Dist((Vector3 *)(self + 0x5c), (Vector3 *)(self + 0x41c)) > 0x3e8000)) {
            if (*(int *)(self + 0x444) <= data_ov084_02130228[*(int *)(self + 0x460)]) {
                func_ov084_02129c9c(self);
            } else {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x370, data_ov084_02130cf0[1], 0, 0x1000, 0);
            }
            if (*(unsigned char *)(self + 0x113) >= 6 || *(u16 *)(self + 0x458) != 0) {
                *(s16 *)(self + 0x45c) = *(s16 *)(self + 0x45a);
            } else {
                step = 0x600;
                *(s16 *)(self + 0x45c) = *(s16 *)(self + 0x45a);
            }
            *(int *)(self + 0x444) = data_ov084_02130268[*(int *)(self + 0x460)];
        } else {
            *(int *)(self + 0x444) = data_ov084_02130228[*(int *)(self + 0x460)];
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x370, data_ov084_02130ce8[1], 0, 0x1000, 0);
            
            if (*(u16 *)(self + 0x450) != 0) {
                *(u16 *)(((int)self + 0x450)) =
                    *(u16 *)(((int)self + 0x450)) - 1;
            } else if (((unsigned)RandomIntInternal(&data_0209e650) >> 16) & 3) {
                *(s16 *)(self + 0x45c) = *(s16 *)(self + 0x94) + (s16)((unsigned)RandomIntInternal(&data_0209e650) >> 16);
                *(u16 *)(self + 0x450) = 0x64;
            } else {
                *(s16 *)(self + 0x45c) = (s16)((unsigned)RandomIntInternal(&data_0209e650) >> 16);
                func_ov084_02129c9c(self);
            }

        }
    }
    if (*(unsigned char *)(self + 0x113) >= 6) {
        if (*(u16 *)(self + 0x456) > 0x1e)
            *(u16 *)(self + 0x458) = *(u16 *)(self + 0x456);
    }
    ApproachLinear(*(short *)(self + 0x94), *(s16 *)(self + 0x45c), step);
}
}

// @symbol func_ov084_0212af74
extern "C" {
void func_ov084_0212af74(char *c)
{
    Vector3 targetPos;
    void *player;
    s32 dist;
    s32 flag;
    s16 ang;
    u32 rnd;
    s32 lvl;
    u32 id;

    id = *(u32 *)(c + 0x43c);
    if (id == 0) {
        *(s32 *)(c + 0x460) = 1;
        return;
    }
    player = dActor_c::FindWithID(id);
    if (player == 0) {
        *(s32 *)(c + 0x460) = 1;
        return;
    }

    if (*(s32 *)(c + 0x444) == 0) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, (void *)data_ov084_02130cc8[1], 0, 0x1000, 0);
    } else {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, (void *)data_ov084_02130ce8[1], 0, 0x1000, 0);
    }

    func_ov074_0212087c(&targetPos, player, *(u8 *)(c + 0x474));

    if (ApproachAngle((s16 *)(c + 0x94), *(s16 *)(c + 0x45a), 4, 0x1000, 0x400) == 0 &&
        ((dBgCh_Actr *)(c + 0x1b4))->IsOnGround() != 0)
    {
        Vec3_HorzDist(c + 0x5c, c + 0x41c);
        dist = Vec3_HorzDist(c + 0x5c, &targetPos);

        if ((*(s32 *)((char *)player + 0x5cc) == 4 && *(s16 *)((char *)player + 0x5f6) == 0) ||
            DecIfAbove0_Byte(c + 0x475) != 0 ||
            dist > 0x3e8000)
        {
            ang = ((dActor_c *)c)->HorzAngleToCPlayer();
            flag = 1;
            if (((dActor_c *)c)->DistToCPlayer() < 0x3e8000 &&
                AngleDiff(ang, *(s16 *)(c + 0x8e)) < 0x3000)
            {
                *(s16 *)(c + 0x45a) = ((dActor_c *)c)->HorzAngleToCPlayer();
                *(s16 *)(c + 0x45c) = *(s16 *)(c + 0x45a);
                lvl = *(s32 *)(c + 0x460);
                if (*(s32 *)(c + 0x444) == data_ov084_02130228[lvl]) {
                    *(s32 *)(c + 0x444) = data_ov084_02130268[lvl];
                }
                lvl = *(s32 *)(c + 0x460);
                if (*(s32 *)(c + 0x444) != data_ov084_02130268[lvl]) {
                    flag = 0;
                    *(s32 *)(c + 0xa8) = data_ov084_02130248[lvl];
                    *(s32 *)(c + 0x98) = 0;
                    *(s32 *)(c + 0x444) = data_ov084_02130268[*(s32 *)(c + 0x460)];
                }
                if (((dBgCh_Actr *)(c + 0x1b4))->JustHitGround() != 0) {
                    ((dActor_c *)c)->LandingDust(1);
                }
            } else {
                if (DecIfAbove0_Short(c + 0x454) == 0) {
                    rnd = RandomIntInternal(&data_0209e650);
                    *(s16 *)(c + 0x454) = (s16)((rnd >> 0x1b) + 0x1e);
                    *(s16 *)(c + 0x45a) = (s16)(rnd >> 0x10);
                }
                *(s32 *)(c + 0x444) = 0;
            }
            if (flag != 0) {
                _Z14ApproachLinearRiii((int *)(c + 0x98), *(s32 *)(c + 0x444), 0x500);
            }
        } else {
            *(s16 *)(c + 0x45a) = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &targetPos);
            if (Vec3_HorzDist(c + 0x5c, &targetPos) < 0x32000) {
                *(s32 *)(c + 0x444) = *(s32 *)((char *)player + 0x5e8) >> 2;
            } else if (Vec3_HorzDist(c + 0x5c, &targetPos) < 0x64000) {
                *(s32 *)(c + 0x444) = *(s32 *)((char *)player + 0x5e8) >> 1;
            } else if (Vec3_HorzDist(c + 0x5c, &targetPos) < 0x96000) {
                *(s32 *)(c + 0x444) = *(s32 *)((char *)player + 0x5e8);
            } else {
                s32 idx = *(s32 *)(c + 0x460);
                s32 v = data_ov084_02130268[idx];
                *(s32 *)(c + 0x444) = v + *(s32 *)((char *)player + 0x5e8);
            }
            Math_Function_0203b14c((int *)(c + 0x98), *(s32 *)(c + 0x444), 0x800, 0x10000, 4);
        }
    }

    if (*(s32 *)(c + 0x60) < *(s32 *)(c + 0x420) - 0x3e8000) {
        ((fBase_c *)c)->MarkForDestruction();
    }

    if (*(s32 *)((char *)player + 0x5cc) != 4)
        return;
    if (*(s16 *)((char *)player + 0x5f6) == 0)
        *(u8 *)(c + 0x475) = 0x1e;
}
}

// @symbol func_ov084_0212b2dc
extern "C" {
void func_ov084_0212b2dc(char *c) {
    daKrb_c *goomba = (daKrb_c *)c;
    if (goomba->mGoombaType == 3)
        func_ov084_0212af74(c);
    else
        func_ov084_0212abd4(c);
    goomba->mAngleY = goomba->mPrevAngleY;
}
}

// @symbol _ZN7daKrb_c16OnAimedAtWithEggEv
int daKrb_c::OnAimedAtWithEgg()
{
    int normal = (actorID == (unsigned short)0xc8) ? 1 : 0;
    if (normal)
        return 0x41000;
    int large = (actorID == (unsigned short)0xca) ? 1 : 0;
    if (large)
        return 0x96000;
    return 0x14000;
}

// @symbol _ZN7daKrb_c13OnTurnIntoEggER6Player
void daKrb_c::OnTurnIntoEgg(Player &playerRef)
{
    char *self = (char *)this;
    char *player = (char *)&playerRef;
    int b5;
    bool b4;

    if ((mCapId & 0xf) < 6 || mRewardType == 2) {
        mPosX = mHomePos.x;
        mPosY = mHomePos.y;
        mPosZ = mHomePos.z;
        RespawnIfHasCap();
    }

    if (((dActor_c *)self)->OnYoshiTryEat() == 6) {
        if (((Player *)player)->IsCollectingCap()) {
            if (unk_108 == 1)
                GivePlayerCoins(*(Player *)player, 1, 0);
            func_ov084_021296cc(self);
        } else {
            b5 = 0;
            b4 = b5;
            if (unk_108 == 1)
                b5 = 1;
            if (mRewardType == 1) {
                UntrackStar(mStarTracked);
                b4 = 1;
                dActor_c::Spawn(0xb4, 0x50, mHomePos, 0, mAreaId, -1);
                /* unsigned on the load side only: spelling both sides as param1
                   lets mwccarm CSE the field address. The ROM wants [rN,#8]. */
                *(int *)(self + 8) = *(unsigned int *)(self + 8) & 0xff0f;
            } else if (mRewardType == 2) {
                if (mStarID == data_0209f344[data_0209f208[0]]) {
                    UntrackStar(mStarTracked);
                    mRewardType = 3;
                    b4 = 1;
                }
            }
            ((Player *)player)->RegisterEggCoinCount(b5, b4, 0);
        }
    } else if (((dActor_c *)self)->OnYoshiTryEat() == 4) {
        if (unk_108 == 1)
            GivePlayerCoins(*(Player *)player, 1, 0);
    }

    func_ov084_02129498(self);
}

// @symbol _ZN7daKrb_c16CleanupResourcesEv
int daKrb_c::CleanupResources()
{
  int i;
  if (mGoombaType == 2)
    UnloadBlueCoinModel(((char*)this));
  data_ov084_02130cf8.Release();
  for (i = 0; i < 7; ++i)
    ((SharedFilePtr *)(data_ov084_02130278[i]))->Release();
  if ((unsigned char)(mRewardType + 0xff) <= 1)
    UnloadSilverStarAndNumber();
  UnloadCapModel();
  if (mGoombaType == 3) {
    unsigned int id = mTargetUniqueID;
    if (id != 0) {
      char* a = (char*)dActor_c::FindWithID(id);
      if (a != 0) {
        unsigned char *p = (unsigned char*)(((int)a + 0x602));
        *p = *p - 1;
      }
    }
  }
  return 1;
}

// @symbol _ZN7daKrb_c16OnPendingDestroyEv
void daKrb_c::OnPendingDestroy()
{
}

// @symbol _ZN7daKrb_c6RenderEv
int daKrb_c::Render()
{
    int locked;
    volatile Vector3 backup;

    locked = (mFlags & 0x40000) != 0;
    if (locked || mIsDormant != 0) return 1;

    backup.x = mScaleX;
    backup.y = mScaleY;
    backup.z = mScaleZ;

    if (mDeathState == 1) {
        mScaleX = (int)(((long long)mScaleX * data_ov084_02130258[mGoombaType] + 0x800) >> 12);
        mScaleY = (int)(((long long)mScaleY * data_ov084_02130258[mGoombaType] + 0x800) >> 12);
        mScaleZ = (int)(((long long)mScaleZ * data_ov084_02130258[mGoombaType] + 0x800) >> 12);
    }

    mModelAnim.Render((const Vector3 *)&mScaleX);

    mScaleX = backup.x;
    mScaleY = backup.y;
    mScaleZ = backup.z;
    mMaterialChanger.Update(mModelAnim.data);
    RenderCapModel(0);
    return 1;
}

// @symbol _ZN7daKrb_c8BehaviorEv
int daKrb_c::Behavior()
{
    Vector3 v1;
    Vector3 v2;
    int r;
    int st;

    func_ov084_02129864(((char*)this));
    func_ov084_021290d4(((char*)this));
    r = _ZN11dCapEnemy_c11GetCapStateEv(((char*)this));
    if (r == 0)
        return 1;
    if (r == 1) {
        *(u32*)((char*)&mFlags) |= 0x10000000;
        PoofDust();
    }
    if (mGoombaType != 3 && mState != 3 &&
        mEatenByYoshi == 0 && mDeathState == 0 &&
        _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(((char*)this), 0x5dc000) != 0)
    {
        Unk_02005d94();
        return 1;
    }

    if (mDeathState != 0) {
        r = UpdateKillByInvincibleChar(*(dBgCh_Actr *)(((char*)this) + 0x1b4), *(ModelAnim *)(((char*)this) + 0x370), 3);
        if (r != 0) {
            if (r == 2) {
                func_ov084_02129498(((char*)this));
                v1.x = 0;
                v1.y = 0x6c000;
                v1.z = 0;
                ReleaseCap(v1);
                mPosX = mHomePos.x;
                mPosY = mHomePos.y;
                mPosZ = mHomePos.z;
                mAngleX = 0;
                mAngleY = 0;
                mAngleZ = 0;
                RespawnIfHasCap();
                func_ov084_021296cc(((char*)this));
            }
            return 1;
        }
        if (func_ov084_021298d0(((char*)this)) == 0)
            func_ov084_0212a580();
        return 1;
    }

    if (func_ov084_02129a00(((char*)this)) != 0)
        return 1;

    if (mState >= 3 ||
        (mGoombaType == 3 && (int)mModelAnim.file == data_ov084_02130cc8[1]))
    {
        mModelAnim.speed = 0x1000;
    } else {
        int v = cstd::fdiv(mHorzSpeed, mScaleX * 2);
        if (v > 0x3000)
            v = 0x3000;
        mModelAnim.speed = v;
    }

    MakeVanishLuigiWork(*(dCc_c *)(((char*)this) + 0x180));

    if (mState != 2) {
        func_ov084_0212934c(((char*)this));
        ((Animation *)&mModelAnim)->Advance();
    }

    st = mState;
    {
        int* q = &data_ov084_02130d74[st * 2];
        int adj = q[1];
        char* thiz = ((char*)this) + (adj >> 1);
        void (*fn)(char*);
        if (adj & 1)
            fn = *(void(**)(char*))(*(char**)thiz + q[0]);
        else
            fn = (void(*)(char*))q[0];
        fn(thiz);
    }

    {
        u16* hp = (u16*)((char*)&mStateTimer);
        *hp += 1;
        if (st != mState)
            *hp = 0;
    }

    func_ov084_02129ed4(((char*)this));

    if (mCapId < 6)
        UpdatePos(0);
    else
        UpdatePos((dCc_c *)(((char*)this) + 0x180));

    if (mDeathState == 0 && mState != 2 && mState != 3) {
        if (IsGoingOffCliff(*(dBgCh_Actr *)(((char*)this) + 0x1b4), 0x32000, 0x1f49, 0, 1, 0x32000) != 0) {
            mPosX = mSafePos.x;
            mPosY = mSafePos.y;
            mPosZ = mSafePos.z;
        } else {
            mSafePos.x = mPosX;
            mSafePos.y = mPosY;
            mSafePos.z = mPosZ;
        }
    }

    {
        int lvl = mGoombaType;
        if (lvl == 0) {
            UpdateWMClsn(*(dBgCh_Actr *)(((char*)this) + 0x1b4), 0);
        } else if (data_0209f2f8 == 6 || data_0209f2f8 == 0x1b) {
            if (unk_444 == data_ov084_02130228[lvl] && mDeathState != 7)
                UpdateWMClsn(*(dBgCh_Actr *)(((char*)this) + 0x1b4), 3);
            else
                UpdateWMClsn(*(dBgCh_Actr *)(((char*)this) + 0x1b4), 2);
        } else {
            UpdateWMClsn(*(dBgCh_Actr *)(((char*)this) + 0x1b4), 2);
        }
    }

    func_ov084_021294d0(((char*)this));
    ((dCc_c *)&mdCcAc_c)->Clear();
    if (mDeathState == 0)
        ((dCc_c *)&mdCcAc_c)->Update();
    func_ov084_0212a580();
    func_ov084_02129238(((char*)this));

    if (mState == 0) {
        int b = (mFlags & 8) ? 1 : 0;
        if (b == 0) {
            if (Vec3_Dist((Vector3*)((char*)&mPosX), &mStuckCheckPos) < 0xa000) {
                *(u16*)((char*)&mStuckTimer) += 1;
                if (mCapId < 6 && mStuckTimer == 0x1e) {
                    func_ov084_02129c9c(((char*)this));
                    mTimer458 = 0x5a;
                }
                if (mStuckTimer >= 0x12c && mTimer458 == 0) {
                    func_ov084_021296cc(((char*)this));
                    SpawnCoin();
                    func_ov084_02129498(((char*)this));
                    v2.x = 0;
                    v2.y = 0x6c000;
                    v2.z = 0;
                    ReleaseCap(v2);
                    mPosX = mHomePos.x;
                    mPosY = mHomePos.y;
                    mPosZ = mHomePos.z;
                    RespawnIfHasCap();
                    return 1;
                }
            } else {
                mStuckTimer = 0;
                mStuckCheckPos.x = mPosX;
                mStuckCheckPos.y = mPosY;
                mStuckCheckPos.z = mPosZ;
            }
            goto done;
        }
    }

    if (mTimer458 == 0)
        mStuckTimer = 0;
done:
    return 1;
}

// @symbol _ZN7daKrb_c13InitResourcesEv
int daKrb_c::InitResources()
{
    char *c = (char *)this;
    int i;

    mRewardType = (*(unsigned int*)(c + 8) >> 4) & 0xf;
    mStarTracked = -1;
    *(unsigned char*)(c + 0x112) = (*(unsigned int*)(c + 8) >> 8) & 0xf;
    mStarID = (*(unsigned int*)(c + 8) >> 0xc) & 0xf;

    if (mRewardType == 1)
    {
        mStarTracked = ((dActor_c *)c)->TrackStar(mStarID, 1);
        LoadSilverStarAndNumber();
    }
    else if (mRewardType == 2)
    {
        LoadSilverStarAndNumber();
    }

    Model::LoadFile(data_ov084_02130cf8);
    for (i = 0; i < 7; i++)
        Animation::LoadFile(*(SharedFilePtr *)(data_ov084_02130278[i]));

    ((dCapEnemy_c *)c)->AddCap((unsigned char)(*(int*)(c + 8) & 0xf));

    if ((*(unsigned char*)(c + 0x113) & 0xf) < 6)
        /* The load and the store must not be spelled the same way: mwcc CSEs the
           field address across an RMW and the ROM re-issues it. See
           notes/mwccarm-codegen.md. */
        ((int*)c)[2] = *(int*)(c + 8) & 0xf0ff;

    if (((dCapEnemy_c *)c)->DestroyIfCapNotNeeded() == 0)
        return 0;

    if (((ModelBase *)(c + 0x370))->SetFile((BMD_File *)(SHARED_FILE(data_ov084_02130cf8)), 1, -1) == 0)
        return 0;

    if (((ShadowModel *)(c + 0x3d4))->InitCylinder() == 0)
        return 0;

    MaterialChanger::Prepare(*(BMD_File*)SHARED_FILE(data_ov084_02130cf8), *(BMA_File*)&data_ov084_0213089c);
    _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(c + 0x3fc, &data_ov084_0213089c, 0x40000000, 0x1000, 0);

    *(unsigned char*)(c + 0x108) = 1;

    {
        int id = *(unsigned short*)(c + 0xc);
        int cond = (id == 0xc9);
        if (cond != false)
        {
            mGoombaType = 0;
        }
        else
        {
            cond = (id == 0xc8);
            if (cond != false)
            {
                if (*(int*)(c + 8) == 0xeeee || *(int*)(c + 8) == 0xeeef)
                {
                    mGoombaType = 3;
                    *(int*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~2;
                    if (*(int*)(c + 8) == 0xeeee)
                        *(unsigned char*)(c + 0x108) = 0;
                }
                else
                {
                    mGoombaType = 1;
                }
            }
            else
            {
                mGoombaType = 2;
                LoadBlueCoinModel(c);
            }
        }
    }

    {
        int scale = data_ov084_02130258[mGoombaType];
        *(int*)(c + 0x80) = scale;
        *(int*)(c + 0x84) = scale;
        *(int*)(c + 0x88) = scale;
    }
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(c + 0x180, c, *(int*)(c + 0x80) * 0x3c, data_ov084_02130208[mGoombaType], 0x200000, 0xa6efe0);

    if (mGoombaType == 2)
        *(int*)(((int)c + 0x19c) & 0xFFFFFFFFFFFFFFFF) &= ~0x8000;

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(c + 0x1b4, c, *(int*)(c + 0x80) * 0x3c, *(int*)(c + 0x80) * 0x3c, 0, 0);
    ((dBgCh_Actr *)(c + 0x1b4))->StartDetectingWater();

    mSoundLatchFlags = 0;
    mState = 0;
    *(int*)(c + 0x10c) = 0;
    unk_438 = 0;
    mDistToPlayer = 0x7fffffff;
    mInitAngleY = *(short*)(c + 0x94);
    unk_444 = data_ov084_02130228[mGoombaType];
    mHeadingHoldTimer = 0;
    mTargetUniqueID = 0;
    mWanderRerollTimer = 0;
    mStuckTimer = 0;
    *(int*)(c + 0x428) = *(int*)(c + 0x5c);
    *(int*)(c + 0x42c) = *(int*)(c + 0x60);
    *(int*)(c + 0x430) = *(int*)(c + 0x64);
    mTimer458 = 0;
    func_ov084_021290d4(c);

    *(int*)(c + 0x41c) = *(int*)(c + 0x5c);
    *(int*)(c + 0x420) = *(int*)(c + 0x60);
    *(int*)(c + 0x424) = *(int*)(c + 0x64);
    *(int*)(c + 0x9c) = data_ov084_02130238[mGoombaType];
    *(int*)(c + 0xa0) = -0x32000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, data_ov084_02130ce8[1], 0, 0x1000, 0);

    unk_467 = 0;
    mSavedParam = *(int*)(c + 8);
    return 1;
}

// @symbol _ZN7daKrb_c13OnYoshiTryEatEv
int daKrb_c::OnYoshiTryEat()
{
    int normal = (actorID == (unsigned short)0xc8) ? 1 : 0;
    if (normal)
        return 0x6;
    int small = (actorID == (unsigned short)0xc9) ? 1 : 0;
    if (small)
        return 0x4;
    return 0;
}

// @symbol daKrb_c_classInit_KURIBO_L
extern "C" daKrb_c *daKrb_c_classInit_KURIBO_L()
{
    return new daKrb_c();
}

// @symbol daKrb_c_classInit_KURIBO_S
extern "C" daKrb_c *daKrb_c_classInit_KURIBO_S()
{
    return new daKrb_c();
}

// @symbol daKrb_c_classInit_KURIBO
extern "C" daKrb_c *daKrb_c_classInit_KURIBO()
{
    return new daKrb_c();
}

