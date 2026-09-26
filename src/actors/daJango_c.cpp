//cpp
/* Klepto the condor (ov062/daJango_c), 23 functions, enrolled.
 * The factory lives in src/d_a_jango.c, the next object, not a hole.
 *
 * Source runs REVERSE of ROM (highest address first). Do not reorder.
 * decl_common.h is deliberately NOT included (it collides with this
 * TU's C-linkage helper); the three needed names are local. One
 * helper is parsed as C (ldm/stm Vector3 copy); see its site.
 *
 * Leftover: the func_ov062 helpers keep linker names; naming belongs
 *   at their definitions.
 * Leftover: the Obj/Data/Found shadows stay file-local (unowned rows).
 * Leftover: NewSimple keeps its scalar-ABI spelling (wall 6az).
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "daJango_c.h"
#include "types.h"
#include "common.h"
/* decl_common.h deliberately NOT included: it declares func_ov062_0211b3ac
   (C++-linkage, 1-arg) which collides with this TU's C-linkage definition.
   The three names this TU needs from it are declared locally instead. */
extern char data_ov062_0211e15c;
extern "C" int func_ov062_0211b51c(char *c);
extern "C" int AngleDiff(int a, int b);
#include "PathPtr.h"
#include "SharedFilePtr.h"
#include "decl_SaveData.h"
#include "SaveData.h"
#include "Player.h"
#include "BlendModelAnim.h"
#include "Animation.h"

/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
/* shadow enum 'Bool' */
enum Bool { FALSE, TRUE };

/* shadow struct 'BCA_File' */
struct BCA_File;

/* Scalar-ABI spelling of Particle::System::NewSimple: the ROM name
   carries by-value class parameters (e.g. Fix12<int>), which mwccarm
   passes differently at the call site, so declaring the true types
   breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);

/* shadow struct 'Data' */
struct Data { BCA_File* a; BCA_File* f; };

/* shadow struct 'Found' */
struct Found {
    char p[0x9c];
    int f9c, fa0;
    char g[0xc8-0xa4];
    int fc8;
};

/* shadow struct 'Obj' */
struct Obj {
    char p0[0x5c];
    Vector3 f5c;
    char g68[0x8c-0x68];
    Vector3_16 f8c;
    char g92[0x9c-0x92];
    int f9c, fa0, fa4, fa8, fac;   /* 9c,a0,a4,a8,ac */
    char gb0[0xcc-0xb0];
    signed char fcc;
    char g[0x334-0xcd];
    /* Small placeholder, NOT the real BlendModelAnim (0x70): every legacy
       file recovered this blob with a fieldless local view (effective 4
       bytes), and the absolute gaps below assume that layout. */
    char anim[4];
    char g2[0x43c-0x338];
    int f43c;
    char g3[0x444-0x440];
    short f444;
    char g3b[0x448-0x446];
    unsigned char f448;
    char g4[0x44c-0x449];
    unsigned int f44c;
    char g5[0x468-0x450];
    int f468;
    unsigned int f46c;
};

/* shadow typedef 'M48' */
typedef struct { int w[12]; } M48;

/* shadow struct 'Base' */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };

/* TUBUILD CONFLICT -- alternate body of typedef 'Vector3', from the legacy file for func_ov062_0211b51c, NOT applied:
typedef struct { int x, y, z; } Vector3;
*/

/* TUBUILD CONFLICT -- alternate body of struct 'BlendModelAnim', from the legacy file for func_ov062_0211b930, NOT applied:
struct BlendModelAnim {
    int SetAnim(BCA_File& f, int a, int b, Fix12 spd, unsigned short t);
};
*/

/* TUBUILD CONFLICT -- alternate body of struct 'BlendModelAnim', from the legacy file for func_ov062_0211bc54, NOT applied:
struct BlendModelAnim {
    void SetAnim(BCA_File &a, int b, int c, int d, unsigned short e);
};
*/

/* TUBUILD CONFLICT -- alternate body of typedef 'Vector3', from the legacy file for func_ov062_0211c594, NOT applied:
typedef struct { int x, y, z; } Vector3;
*/

extern "C" {
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* m, short angX);
extern void MulVec3Mat4x3(const Vector3* v, const void* m, Vector3* res);
extern int data_020a0e68[];
extern "C" void *_ZN7PathPtrC1Ev(void *self);
extern "C" int Vec3_HorzDist(const Vector3* a, const Vector3* b);
extern signed char data_0209f2f8;
extern "C" int func_ov062_0211c658(void *c, void *p);
extern void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void *a, int b, int c, int d, int e);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *pl, Vector3 *v, unsigned int a, int b, unsigned int c, unsigned int d, unsigned int e);
extern char data_ov062_0211e14c[];
extern char data_ov062_0211e17c[];
extern void _Z14ApproachLinearRsss(short *dst, short target, short step);
extern "C" int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *, BCA_File& f, int a, int b, int d, unsigned short e);
extern SharedFilePtr data_ov062_0211e104;
extern "C" void func_02012790(int);
extern "C" void func_02012694(unsigned int, void*);
extern SharedFilePtr data_ov062_0211e10c;
extern SharedFilePtr data_ov062_0211e114;
extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern s16 Vec3_VertAngle(const Vector3 *v0, const Vector3 *v1);
extern "C" int data_0209e650;
extern "C" int RandomIntInternal(int* seed);
extern int Vec3_Dist(const void *a, const void *b);
extern int ApproachAngle(s16 *cur, s16 target, int div, int band, int maxStep);
extern "C" void func_ov062_0211b2fc(char *self);
extern "C" int func_ov062_0211b3ac(char *self);
extern void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
extern int LenVec3(Vector3 *v);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_MulScalar(void *out, Vector3 *in, int s);
extern void SubVec3(void *a, void *b, void *c);
extern void Vec3_Asr(void* d, void* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void MulMat4x3Mat4x3(void* a, void* b, void* c);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* m, int rad, int h, unsigned int u);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *a, Fix12i r, Fix12i h, unsigned int d, unsigned int e);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *a, Fix12i b, Fix12i c, void *d, void *e);
extern SharedFilePtr data_ov062_0211e0fc;
extern SharedFilePtr data_ov002_0210da40;
extern SharedFilePtr data_ov002_0210d9a0;
extern SharedFilePtr data_ov002_0210d9c0;
extern void *data_0209f394;
/* TUBUILD CONFLICT -- alternate declaration of func_ov062_0211c658, from the legacy file for func_ov062_0211b800, NOT applied: extern void func_ov062_0211c658(void *c, void *p); */
/* TUBUILD CONFLICT -- alternate declaration of data_ov062_0211e17c, from the legacy file for func_ov062_0211b8d8, NOT applied: extern void* data_ov062_0211e17c; */
/* TUBUILD CONFLICT -- alternate declaration of func_ov062_0211c658, from the legacy file for func_ov062_0211b8d8, NOT applied: extern "C" int func_ov062_0211c658(unsigned char* c, void* p); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt, from the legacy file for func_ov062_0211b930, NOT applied: extern "C" int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *, BCA_File& f, int a, int b, Fix12 spd, unsigned short t); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c13ClosestPlayerEv, from the legacy file for func_ov062_0211ba84, NOT applied: extern char *_ZN8dActor_c13ClosestPlayerEv(char *self); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov062_0211c658, from the legacy file for func_ov062_0211ba84, NOT applied: extern int func_ov062_0211c658(char *c, void *p); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRsss, from the legacy file for func_ov062_0211ba84, NOT applied: extern void _Z14ApproachLinearRsss(s16 *cur, s16 tgt, s16 step); */
/* TUBUILD CONFLICT -- alternate declaration of Matrix4x3_FromRotationY, from the legacy file for func_ov062_0211ba84, NOT applied: extern void Matrix4x3_FromRotationY(void *m, int angle); */
/* TUBUILD CONFLICT -- alternate declaration of Matrix4x3_ApplyInPlaceToRotationX, from the legacy file for func_ov062_0211ba84, NOT applied: extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX); */
/* TUBUILD CONFLICT -- alternate declaration of MulVec3Mat4x3, from the legacy file for func_ov062_0211ba84, NOT applied: extern void MulVec3Mat4x3(const Vector3 *v, void *m, Vector3 *out); */
/* TUBUILD CONFLICT -- alternate declaration of data_ov062_0211e17c, from the legacy file for func_ov062_0211ba84, NOT applied: extern void *data_ov062_0211e17c; */
/* TUBUILD CONFLICT -- alternate declaration of _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt, from the legacy file for func_ov062_0211bc54, NOT applied: extern "C" void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *, BCA_File &a, int b, int c, int d, unsigned short e); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209f2f8, from the legacy file for func_ov062_0211bc54, NOT applied: extern "C" signed char data_0209f2f8; */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov062_0211bc54, NOT applied: extern "C" void func_02012694(int a0, void *a1); */
/* TUBUILD CONFLICT -- alternate declaration of _ZNK10dBgCh_Actr8IsOnWallEv, from the legacy file for func_ov062_0211bd10, NOT applied: extern int _ZNK10dBgCh_Actr8IsOnWallEv(void *self); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov062_0211bd10, NOT applied: extern s16 Vec3_HorzAngle(const void *a, const void *b); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_VertAngle, from the legacy file for func_ov062_0211bd10, NOT applied: extern s16 Vec3_VertAngle(const void *a, const void *b); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRsss, from the legacy file for func_ov062_0211bd10, NOT applied: extern int _Z14ApproachLinearRsss(s16 *cur, s16 target, s16 step); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c13ClosestPlayerEv, from the legacy file for func_ov062_0211bd10, NOT applied: extern void *_ZN8dActor_c13ClosestPlayerEv(void *self); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt, from the legacy file for func_ov062_0211bd10, NOT applied: extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt( void *anim, void *file, int a, int b, int speed, unsigned short flags); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov062_0211c658, from the legacy file for func_ov062_0211bd10, NOT applied: extern int func_ov062_0211c658(void *self, void *state); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN7PathPtrC1Ev, from the legacy file for func_ov062_0211bd10, NOT applied: extern void _ZN7PathPtrC1Ev(void *self); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN7PathPtr6FromIDEj, from the legacy file for func_ov062_0211bd10, NOT applied: extern void _ZN7PathPtr6FromIDEj(void *self, unsigned int id); */
/* TUBUILD CONFLICT -- alternate declaration of _ZNK7PathPtr7GetNodeER7Vector3j, from the legacy file for func_ov062_0211bd10, NOT applied: extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *self, Vector3 *v, unsigned int i); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209f2f8, from the legacy file for func_ov062_0211bd10, NOT applied: extern s8 data_0209f2f8; */
/* TUBUILD CONFLICT -- alternate declaration of _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt, from the legacy file for func_ov062_0211c218, NOT applied: extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *anim, void *file, int a, int b, int c, unsigned short u); */
/* TUBUILD CONFLICT -- alternate declaration of RandomIntInternal, from the legacy file for func_ov062_0211c218, NOT applied: extern int RandomIntInternal(int *seed); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209e650, from the legacy file for func_ov062_0211c218, NOT applied: extern int data_0209e650; */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov062_0211c2f4, NOT applied: extern s16 Vec3_HorzAngle(const void *a, const void *b); */
/* TUBUILD CONFLICT -- alternate declaration of ApproachAngle, from the legacy file for func_ov062_0211c2f4, NOT applied: extern int ApproachAngle(void *p, int target, int a, int b, int c); */
/* TUBUILD CONFLICT -- alternate declaration of RandomIntInternal, from the legacy file for func_ov062_0211c2f4, NOT applied: extern int RandomIntInternal(int *seed); */
/* TUBUILD CONFLICT -- alternate declaration of Matrix4x3_FromRotationY, from the legacy file for func_ov062_0211c2f4, NOT applied: extern void Matrix4x3_FromRotationY(void *m, int angle); */
/* TUBUILD CONFLICT -- alternate declaration of MulVec3Mat4x3, from the legacy file for func_ov062_0211c2f4, NOT applied: extern void MulVec3Mat4x3(Vector3 *in, void *m, void *out); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov062_0211c658, from the legacy file for func_ov062_0211c2f4, NOT applied: extern void func_ov062_0211c658(void *c, void *p); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209f2f8, from the legacy file for func_ov062_0211c2f4, NOT applied: extern s8 data_0209f2f8; */
/* TUBUILD CONFLICT -- alternate declaration of data_0209e650, from the legacy file for func_ov062_0211c2f4, NOT applied: extern int data_0209e650; */
/* TUBUILD CONFLICT -- alternate declaration of Matrix4x3_FromRotationY, from the legacy file for func_ov062_0211c594, NOT applied: extern void Matrix4x3_FromRotationY(struct Matrix4x3 *m, short angY); */
/* TUBUILD CONFLICT -- alternate declaration of Matrix4x3_ApplyInPlaceToRotationX, from the legacy file for func_ov062_0211c594, NOT applied: extern void Matrix4x3_ApplyInPlaceToRotationX(struct Matrix4x3 *m, short angX); */
/* TUBUILD CONFLICT -- alternate declaration of MulVec3Mat4x3, from the legacy file for func_ov062_0211c594, NOT applied: extern void MulVec3Mat4x3(const Vector3 *v, const struct Matrix4x3 *m, Vector3 *out); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt, from the legacy file for func_ov062_0211c594, NOT applied: extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*, void*, int, int, int, unsigned short); */
/* TUBUILD CONFLICT -- alternate declaration of data_020a0e68, from the legacy file for func_ov062_0211c594, NOT applied: extern struct Matrix4x3 data_020a0e68; */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c10FindWithIDEj, from the legacy file for func_ov062_0211c6a8, NOT applied: extern unsigned int _ZN8dActor_c10FindWithIDEj(unsigned int id); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN7PathPtrC1Ev, from the legacy file for _ZN9daJango_c13InitResourcesEv, NOT applied: extern void _ZN7PathPtrC1Ev(void *self); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN7PathPtr6FromIDEj, from the legacy file for _ZN9daJango_c13InitResourcesEv, NOT applied: extern void _ZN7PathPtr6FromIDEj(void *self, unsigned int id); */
/* TUBUILD CONFLICT -- alternate declaration of _ZNK7PathPtr7GetNodeER7Vector3j, from the legacy file for _ZN9daJango_c13InitResourcesEv, NOT applied: extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *self, void *v, unsigned int idx); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_, from the legacy file for _ZN9daJango_c13InitResourcesEv, NOT applied: extern void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void *self, Fix12i a, Fix12i b, Fix12i c, Fix12i d); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov062_0211c658, from the legacy file for _ZN9daJango_c13InitResourcesEv, NOT applied: extern void func_ov062_0211c658(void *c, void *p); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for _ZN9daJango_c13InitResourcesEv, NOT applied: extern short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b); */
/* TUBUILD CONFLICT -- alternate declaration of data_ov062_0211e114, from the legacy file for _ZN9daJango_c13InitResourcesEv, NOT applied: extern SharedFilePtr data_ov062_0211e114; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov062_0211e104, from the legacy file for _ZN9daJango_c13InitResourcesEv, NOT applied: extern SharedFilePtr data_ov062_0211e104; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov062_0211e17c, from the legacy file for _ZN9daJango_c13InitResourcesEv, NOT applied: extern char data_ov062_0211e17c; */
}

// @symbol _ZN9daJango_c16OnAimedAtWithEggEv
// recovered name: Klepto_OnAimedAtWithEgg
/* daJango_c::OnAimedAtWithEgg - recovered from vtable slot identity */
s32 daJango_c::OnAimedAtWithEgg() {
    return 458752;
}

// @symbol _ZN9daJango_c13InitResourcesEv
/* SharedFilePtr stays incomplete: Model.h forward-declares it and its layout is
   deliberately not recovered (include/SharedFilePtr.h). Used only by address here. */
int daJango_c::InitResources()
{
    void *bmd;
    void *spawned;
    void *pl;
    char path1[8];
    char path2[8];
    unsigned char hat;
    int area;
    int param;
    int zero;
    

    bmd = Model::LoadFile(data_ov062_0211e0fc);
    mBlendModelAnim.SetFile((BMD_File *)bmd, 1, -1);
    mShadowModel.InitCylinder();
    Animation::LoadFile(data_ov062_0211e114);
    Animation::LoadFile(data_ov062_0211e10c);
    Animation::LoadFile(data_ov062_0211e104);
    Model::LoadFile(data_ov002_0210da40);
    Model::LoadFile(data_ov002_0210d9a0);
    Model::LoadFile(data_ov002_0210d9c0);

    mPathId = param1 & 0xff;
    mCarriedItem = (param1 >> 8) & 0xf;
    mHeldItemParam = (param1 >> 0xc) & 0xf;
    if (mPathId < 0)
        mPathId = 0;
    if (mCarriedItem == 0xff)
        mCarriedItem = 0;
    if (mCarriedItem == 2) {
        unk_448 = 2;
        mCarriedItem = 1;
    }

    _ZN7PathPtrC1Ev(path1);
    ((PathPtr *)path1)->FromID(mPathId);
    unk_470 = 4;
    mTerminalVelocity = -0x1e000;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c1, ((char *)this), 0x64000, 0xa0000, 0x200002, 0x3eff0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c2, ((char *)this), 0x3c000, 0xa0000, 0x200000, 0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, ((char *)this), 0x1e000, 0x1e000, 0, 0);

    _ZN7PathPtrC1Ev(path2);
    ((PathPtr *)path2)->FromID(mPathId);
    ((PathPtr *)path2)->GetNode(*(Vector3 *)&mPathNodePosX, mPathNodeIndex);
    mBlendModelAnim.speed = 0x1000;
    mHeldActorID = 0;

    if (mCarriedItem == 1) {
        if (unk_448 != 2) {
            spawned = dActor_c::Spawn(0xb2, mHeldItemParam | 0x50, *(Vector3 *)&mPosX, 0, mAreaId, -1);
        } else {
            spawned = dActor_c::Spawn(0xb3, 0x50, *(Vector3 *)&mPosX, 0, mAreaId, -1);
        }
        if (spawned != 0) {
            mHeldActorID = *(int *)((char *)spawned + 4);
            _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(spawned, 0x64000, 0x258000, 0x1f40000, 0x1f40000);
        }
        mPosX = mPathNodePosX;
        mPosY = mPathNodePosY;
        mPosZ = mPathNodePosZ;
        func_ov062_0211c658(((char *)this), &data_ov062_0211e15c);
    } else {
        pl = data_0209f394;
        if (pl != 0 && *(int *)((char *)pl + 8) != 3 && SaveData::HasPlayerLostCap() != 0) {
            {
                unsigned int hat = *(unsigned char *)((char *)pl + 0x6d9);
                int area = mAreaId;
                unsigned int param = 0;
                param = param | (hat << 8);
                spawned = dActor_c::Spawn(0x10d, param, *(Vector3 *)&mPosX, 0, area, -1);
            }
            if (spawned != 0) {
                _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(spawned, 0x64000, 0x258000, 0x1f40000, 0x1f40000);
                mHeldActorID = *(int *)((char *)spawned + 4);
            }
        }
        unk_44a = Vec3_HorzAngle((Vector3 *)&mPosX, (Vector3 *)&mSpawnPosX);
        func_ov062_0211c658(((char *)this), &data_ov062_0211e17c);
    }
    return 1;
}

// @symbol _ZN9daJango_c8BehaviorEv
struct Klass; typedef void (Klass::*PMF)();
struct M { char pad[8]; PMF pmf; };
struct dCc_c;
struct dBgCh_Actr;
extern "C" {
unsigned short DecIfAbove0_Short(unsigned short *p);
extern "C" void func_ov062_0211c6a8(char *self);
extern char data_ov062_0211e17c[];
}

int daJango_c::Behavior()
{
    M *m;
    int b;

    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    DecIfAbove0_Short((unsigned short *)&mTimer);

    m = (M *)mState;
    if (m->pmf != 0)
        (((Klass *)((char *)this))->*(m->pmf))();

    {
        int accum = mVertSpeed;
        int a0 = mVertAccel;
        int lim = mTerminalVelocity;
        int sum = accum + a0;
        if (sum >= lim)
            lim = sum;
        int t = unk_0ac;
        mVertSpeed = lim;
        unk_0ac = t;
    }
    UpdatePosWithOnlySpeed((dCc_c *)&mdCcAc_c1);
    UpdateWMClsn(mWithMeshClsn, 0);

    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    func_ov062_0211c6a8(((char *)this));

    unsigned int actorId = mHeldActorID;
    if (actorId != 0) {
        void *p = dActor_c::FindWithID(actorId);
        if (p != 0) {
            if (mCarriedItem == 1) {
                *(int *)((char *)p + 0x5c) = mHeldPosX;
                *(int *)((char *)p + 0x60) = mHeldPosY;
                *(int *)((char *)p + 0x64) = mHeldPosZ;
                goto skip_destroy;
            } else if (*(unsigned char *)((char *)p + 0x403) == 0) {
                *(int *)((char *)p + 0x5c) = mHeldPosX;
                *(int *)((char *)p + 0x60) = mHeldPosY;
                *(int *)((char *)p + 0x64) = mHeldPosZ;
                goto skip_destroy;
            } else {
                mHeldActorID = 0;
                ((void (*)(int, int))func_02012790)(0xa, 0);
                mTimer = 0x1e;
                func_ov062_0211c658(((char *)this), (PMF *)data_ov062_0211e17c);
                goto skip_destroy;
            }
        } else {
            mHeldActorID = 0;
            ((void (*)(int, int))func_02012790)(0xa, 0);
            mTimer = 0x1e;
            func_ov062_0211c658(((char *)this), (PMF *)data_ov062_0211e17c);
            goto skip_destroy;
        }
    }

    if (mCarriedItem == 1 && unk_448 != 2) {
        b = (mFlags & 8) != 0;
        if (b != 0) {
            MarkForDestruction();
        }
    }
skip_destroy:
    mBlendModelAnim.Advance();
    if (mState != (void *)data_ov062_0211e14c) {
        func_ov062_0211b51c(((char *)this));
    }

    mdCcAc_c1.Clear();
    mdCcAc_c1.Update();
    mdCcAc_c2.Clear();
    mdCcAc_c2.Update();

    return 1;
}

// @symbol _ZN9daJango_c6RenderEv
int daJango_c::Render()
{
 Base *b = (Base *)&mBlendModelAnim; b->m(0); return 1;
}

// @symbol _ZN9daJango_c16OnPendingDestroyEv
/* recovered: shared header, real C++ method
 *
 * fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`.
 */
void daJango_c::OnPendingDestroy()
{
}

// @symbol _ZN9daJango_c16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the seven shared files InitResources claimed -- three of them from
 * ov002, which is where daJango_c's shared Mario-cap assets live.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
int daJango_c::CleanupResources()
{
    data_ov002_0210da40.Release();
    data_ov002_0210d9a0.Release();
    data_ov002_0210d9c0.Release();
    data_ov062_0211e0fc.Release();
    data_ov062_0211e114.Release();
    data_ov062_0211e10c.Release();
    data_ov062_0211e104.Release();
    return 1;
}

// @symbol func_ov062_0211c6a8
extern "C" {
void func_ov062_0211c6a8(char* c)
{
    int v[3];
    char* actor;

    Vec3_Asr(v, c + 0x5c, 3);
    Matrix4x3_FromTranslation(data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(data_020a0e68, *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
    *(M48*)(c + 0x350) = *(M48*)data_020a0e68;

    if (*(unsigned int*)(c + 0x44c) != 0)
    {
        actor = (char*)dActor_c::FindWithID(*(unsigned int*)(c + 0x44c));
        if (actor != 0)
        {
            *(int*)(c + 0x450) = 0;
            *(int*)(c + 0x454) = 0;
            *(int*)(c + 0x458) = 0;
            MulMat4x3Mat4x3(*(char**)(c + 0x348) + 0x120, c + 0x350, c + 0x3fc);
            *(int*)(c + 0x450) = data_020a0e68[9];
            *(int*)(c + 0x454) = data_020a0e68[10];
            *(int*)(c + 0x458) = data_020a0e68[11];
            *(int*)(((int)c + 0x450)) <<= 3;
            *(int*)(((int)c + 0x454)) <<= 3;
            *(int*)(((int)c + 0x458)) <<= 3;
            if (*(int*)(c + 0x468) == 1)
            {
                Matrix4x3_FromTranslation(data_020a0e68, -0xd000, 0x3000, -0x4000);
                Matrix4x3_ApplyInPlaceToRotationXYZExt(data_020a0e68, 0, -0x8000, -0x4400);
            }
            else
            {
                Matrix4x3_FromTranslation(data_020a0e68, -0x7000, 0x1800, 0);
                Matrix4x3_ApplyInPlaceToRotationXYZExt(data_020a0e68, -0x3000, -0x8000, -0x4000);
            }
            MulMat4x3Mat4x3(data_020a0e68, c + 0x3fc, c + 0x3fc);
            *(void**)(actor + 0xc8) = c + 0x3fc;
        }
    }

    Matrix4x3_FromTranslation(data_020a0e68, *(int*)(c + 0x5c) >> 3, (*(int*)(c + 0x60) - 0x18000) >> 3, *(int*)(c + 0x64) >> 3);
    *(M48*)(c + 0x3cc) = *(M48*)data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x3a4, c + 0x3cc, 0x40000, 0x258000, 0xf);
}
}

// @symbol func_ov062_0211c658
struct C658; typedef int (C658::*PMF658)();
struct C658 { char pad[0x42c]; PMF658 *pp; };
extern "C" int func_ov062_0211c658(void *c, void *p) { C658 *cc = (C658 *)c; PMF658 *qq = (PMF658 *)p; cc->pp = qq; PMF658 *q = cc->pp; if (*q == 0) return 1; return (cc->**q)(); }

// @symbol func_ov062_0211c594
extern "C" {
int func_ov062_0211c594(char *c) {
    Vector3 in;
    Vector3 out;

    *(int*)(c + 0xa4) = 0;
    *(int*)(c + 0xa8) = 0;
    *(int*)(c + 0xac) = 0;

    if (*(unsigned char*)(c + 0x446) == 1) {
        in.x = 0; in.y = 0; in.z = 0;
        out.x = 0; out.y = 0; out.z = 0;
        in.z = 0x14000;
        Matrix4x3_FromRotationY(&data_020a0e68, *(short*)(c + 0x8e));
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, -0x4000);
        MulVec3Mat4x3(&in, &data_020a0e68, &out);
        *(int*)(c + 0xa8) = out.y;
        *(unsigned short*)(c + 0x100) = 0x3c;
    }

    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x334, *(((BCA_File **)&data_ov062_0211e104)[1]), 4, 0, 0x1000, 0);
    return 1;
}
}

// @symbol func_ov062_0211c2f4
extern "C" int func_ov062_0211c2f4(char *self) {
    /* Real PathPtr object (as legacy had it via PathPtr.h): its implicit
       ctor call is genuine ROM bytes. Do NOT byte-buffer this one. */
    PathPtr path;
    int zero[6];
    Vector3 node;
    Vector3 node2;
    Vector3 diff;
    Vector3 scaled;
    int len;
    u32 idx;

    path.FromID(*(u32 *)(self + 0x464));
    zero[0] = 0; zero[1] = 0; zero[2] = 0; zero[3] = 0; zero[4] = 0; zero[5] = 0;
    path.GetNode(node, *(u32 *)(self + 0x474));
    ApproachAngle((short *)(self + 0x94), Vec3_HorzAngle((const Vector3 *)(self + 0x5c), &node), 0xa, 0x200, 0x100);
    ApproachAngle((short *)(self + 0x96), 0, 0xa, 0x100, 0x50);

    idx = *(int *)(self + 0x474) - 1;
    if ((int)idx < 0)
        idx = *(int *)(self + 0x470) - 1;
    path.GetNode(node2, idx);
    Vec3_Sub(&diff, (Vector3 *)(self + 0x5c), &node);
    len = LenVec3(&diff);
    if (len == 0)
        goto arrived;
    if (len > 0x14000)
        goto faraway;

arrived:
    *(int *)(self + 0x460) = 7;
    if (*(u8 *)(self + 0x448) == 2 || (u8)(s8)(data_0209f2f8 - 0x18) <= 1) {
        (*(int *)(((int)self + 0x474)))++;
        if (*(int *)(self + 0x474) >= 4)
            *(int *)(self + 0x474) = 0;
    } else if (func_ov062_0211b3ac(self) == 0) {
        u32 r = (u32)RandomIntInternal(&data_0209e650) >> 8 & 3;
        if (*(int *)(self + 0x474) != r) {
            *(int *)(self + 0x474) = r;
        } else {
            int *p = (int *)(((int)self + 0x474));
            (*p)++;
            *p &= 3;
        }
    }
    *(int *)(self + 0x5c) = node.x; *(int *)(self + 0x60) = node.y; *(int *)(self + 0x64) = node.z;
    *(int *)(self + 0x430) = *(int *)(self + 0x5c);
    *(int *)(self + 0x434) = *(int *)(self + 0x60);
    *(int *)(self + 0x438) = *(int *)(self + 0x64);
    zero[2] = 0; zero[0] = 0; zero[1] = 0; zero[2] = 0x14000;
    Matrix4x3_FromRotationY(&data_020a0e68, *(s16 *)(self + 0x8e));
    MulVec3Mat4x3((Vector3 *)zero, data_020a0e68, (Vector3 *)(self + 0xa4));
    func_ov062_0211c658(self, &data_ov062_0211e17c);
    return 1;

faraway:
    if (AngleDiff(Vec3_HorzAngle((const Vector3 *)(self + 0x5c), &node), *(s16 *)(self + 0x8e)) >= 0x2000)
        goto ret1;
    {
        int s;
        zero[1] = *(int *)(self + 0x60);
        s = _ZN4cstd4fdivEii(0x14000, len);
        Vec3_MulScalar(&scaled, &diff, s);
        SubVec3(self + 0x5c, &scaled, self + 0x5c);
        if (*(u8 *)(self + 0x446) != 1)
            goto ret1;
        *(int *)(self + 0x60) = zero[1];
        if (*(u16 *)(self + 0x100) == 0) {
            *(int *)(self + 0xa8) = 0;
            *(u8 *)(self + 0x446) = 0;
        }
    }
ret1:
    return 1;
}

// @symbol func_ov062_0211c218
extern "C" {
int func_ov062_0211c218(char *c)
{
    *(short *)(c + 0x44a) = *(short *)(c + 0x8e);
    *(int *)(c + 0x43c) = 0;
    *(short *)(c + 0x100) = (((unsigned int)RandomIntInternal(&data_0209e650) >> 8) & 3) * 0x64 + 0x12c;
    if (*(unsigned char *)(c + 0x447) == 1) {
        *(short *)(c + 0x100) = 0;
        *(unsigned char *)(c + 0x447) = 0;
    }
    *(int *)(c + 0x440) = 0;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x334, *(((BCA_File **)&data_ov062_0211e104)[1]), 4, 0, 0x1000, 0);
    if (*(unsigned char *)(c + 0x448) == 2 ||
        (unsigned char)(signed char)((signed char)data_0209f2f8 - 0x18) <= 1) {
        *(int *)(c + 0x440) = 2;
        *(short *)(c + 0x100) = 0;
    }
    return 1;
}
}

// @symbol func_ov062_0211bd10
extern "C" {
/* data_ov062_0211e104/114 are TU-wide SharedFilePtr scalars (legacy InitResources view); words beside them go through casts (same addresses, no second type). */
extern char data_ov062_0211e18c[];

int func_ov062_0211bd10(char *c)
{
    s16 angV;
    s16 angD;
    void *player;
    int ppos[3];
    Vector3 node;
    char path[8];
    int thr;
    s8 stage;
    int *p43c;
    s16 *p94;
    int *pp;
    int tmp;
    int flag;

    if (*(u16 *)(c + 0x444) != 0) {
        if (((dBgCh_Actr *)(c + 0x178))->IsOnWall() != 0) {
            *(s16 *)(c + 0x44a) = (s16)(*(s16 *)(c + 0x94) + 0x4000);
        } else {
            *(s16 *)(c + 0x44a) = Vec3_HorzAngle((const Vector3 *)(c + 0x5c), (const Vector3 *)(c + 0x484));
        }
        _Z14ApproachLinearRsss((s16 *)(c + 0x94), *(s16 *)(c + 0x44a), 0x1000);
        _Z14ApproachLinearRsss((s16 *)(c + 0x92), Vec3_VertAngle((const Vector3 *)(c + 0x5c), (const Vector3 *)(c + 0x484)), 0x500);
        _Z14ApproachLinearRsss((s16 *)(c + 0x96), 0, 0x500);
        if (Vec3_Dist(c + 0x5c, c + 0x484) < 0x1f4000) {
            *(u16 *)(c + 0x444) = 0;
        } else {
            *(u16 *)(c + 0x444) = 0x1e;
        }
        func_ov062_0211b2fc(c);
        return 1;
    }

    if (*(int *)(c + 0x468) == 0 && *(int *)(c + 0x44c) == 0) {
        angV = Vec3_VertAngle((const Vector3 *)(c + 0x5c), (const Vector3 *)(c + 0x484));
        angD = (s16)(AngleDiff(*(s16 *)(c + 0x44a), *(s16 *)(c + 0x94)) / 3);
    } else {
        angV = Vec3_VertAngle((const Vector3 *)(c + 0x5c), (const Vector3 *)(c + 0x430));
        angD = (s16)(AngleDiff(*(s16 *)(c + 0x44a), *(s16 *)(c + 0x94)) / 3);
    }

    if (data_0209f2f8 == 0x10) {
        ApproachAngle((s16 *)(c + 0x92), angV, 0xa, 0x400, 0x200);
    } else {
        _Z14ApproachLinearRsss((s16 *)(c + 0x92), angV, 0x300);
    }
    ApproachAngle((s16 *)(c + 0x96), angD, 0xa, 0x200, 0x100);

    if (data_0209f2f8 == 0x10 || *(u8 *)(c + 0x448) == 2) {
        p94 = (s16 *)(c + 0x94);
        *p94 = (s16)(*p94 - 0x200);
    } else {
        p94 = (s16 *)(c + 0x94);
        *p94 = (s16)(*p94 - 0x400);
    }

    if (*(int *)(c + 0x44c) == 0 && *(int *)(c + 0x468) == 0) {
        if (SaveData::HasPlayerLostCap() == 0) {
            player = ((dActor_c *)c)->ClosestPlayer();
            if (player != 0) {
                pp = (int *)((char *)player + 0x5c);
                tmp = pp[0];
                ppos[0] = tmp;
                tmp = pp[1];
                stage = data_0209f2f8;
                ppos[1] = tmp;
                tmp = pp[2];
                ppos[2] = tmp;
                if (stage != 0x10 &&
                    (*(int *)(c + 0x60) + 0x28000 < ppos[1] ||
                     *(u8 *)((char *)player + 0x706) != 0)) {
                    func_ov062_0211b2fc(c);
                    return 1;
                }
                thr = 0x3e8000;
                ppos[1] = *(int *)(c + 0x60);
                if (stage != 0x10) {
                    thr = 0x2bc000;
                }
                if (Vec3_Dist(c + 0x5c, ppos) < thr) {
                    flag = ((u8 *)player)[0x709] ? 1 : 0;
                    if (flag == 0) {
                        if (((u8 *)player)[0x6fb] == 0) {
                            func_ov062_0211c658(c, &data_ov062_0211e18c);
                        }
                    }
                }
                /* fall through to shared b2fc + return */
            }
        }
        func_ov062_0211b2fc(c);
        return 1;
    }

    if (angD < 0x300) {
        if (*(int *)(c + 0x43c) == 1) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
                c + 0x334, *(((BCA_File **)&data_ov062_0211e104)[1]), 4, 0, 0x1000, 0);
            if (*(u8 *)(c + 0x448) == 2 ||
                (unsigned)(u8)(s8)((s8)data_0209f2f8 - 0x18) <= 1u) {
                p43c = (int *)(c + 0x440);
                *p43c = *p43c - 1;
                if (*(int *)(c + 0x440) <= 0) {
                    *(int *)(c + 0x440) = 0;
                }
            }
            *(int *)(c + 0x43c) = 2;
        }
    } else if (*(int *)(c + 0x43c) == 0) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            c + 0x334, *(((BCA_File **)&data_ov062_0211e114)[1]), 4, 0, 0x1000, 0);
        p43c = (int *)(c + 0x43c);
        *p43c = *p43c + 1;
    }

    if (*(int *)(c + 0x43c) >= 2) {
        p43c = (int *)(c + 0x43c);
        *p43c = *p43c + 1;
        if (*(int *)(c + 0x43c) > 0x18) {
            *(int *)(c + 0x43c) = 0;
        }
    }

    if (*(u8 *)(c + 0x448) != 2 && data_0209f2f8 != 0x18 && data_0209f2f8 != 0x19 &&
        (*(int *)(c + 0x468) != 0 || *(int *)(c + 0x44c) != 0) &&
        func_ov062_0211b3ac(c) != 0 && *(u16 *)(c + 0x100) != 0) {
        *(u16 *)(c + 0x100) = 0;
    }

    func_ov062_0211b2fc(c);

    if (*(u16 *)(c + 0x100) == 0 && *(int *)(c + 0x440) == 0) {
        _ZN7PathPtrC1Ev(path);
        ((PathPtr *)path)->FromID(*(unsigned int *)(c + 0x464));
        ((PathPtr *)path)->GetNode(node, *(unsigned int *)(c + 0x474));
        if (AngleDiff(Vec3_HorzAngle((const Vector3 *)(c + 0x5c), &node), *(s16 *)(c + 0x8e)) < 0x2000) {
            func_ov062_0211c658(c, &data_ov062_0211e15c);
        }
    }

    return 1;
}
}

// @symbol func_ov062_0211bc54
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" int func_ov062_0211bc54(char *thiz)
{
    if (data_0209f2f8 == 0x10) {
        *(unsigned short*)(thiz + 0x100) = 0x32;
    } else {
        *(unsigned short*)(thiz + 0x100) =
            (((unsigned)RandomIntInternal(&data_0209e650) >> 8) & 0xf) + 0x14;
    }
    *(int*)(thiz + 0xa4) = 0;
    *(int*)(thiz + 0xa8) = 0;
    *(int*)(thiz + 0xac) = 0;
    func_02012694(0xee, thiz + 0x74);
    *(int*)(thiz + 0x43c) =
        ((((unsigned)RandomIntInternal(&data_0209e650) >> 8) & 3) << 8) + 0x300;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim*)(thiz + 0x334), *(BCA_File*)((void**)&data_ov062_0211e114)[1], 4, 0, 0x1000, 0);
    return 1;
}

// @symbol func_ov062_0211ba84
/* C function in this class TU. The ROM copy is the C front end's block
   move (movs ip, r0; ldm/stm of 12 bytes). C++ scalarizes that assignment
   on every installed mwccarm. func_ov062_0211b930 ends at this address, so
   it was emitted in this object, not linked in from outside. */
#pragma cplusplus off
/* local extern: this function is compiled as C, which cannot call a member */
extern char *_ZN8dActor_c13ClosestPlayerEv(char *self);
/* local extern: this function is compiled as C, which cannot call a member */
extern int _ZNK10dBgCh_Actr8IsOnWallEv(char *self);
int func_ov062_0211ba84(char *c)
{
    Vector3 v;
    Vector3 t;
    Vector3 hv;
    Vector3 vv;
    s16 pitch;
    char *ip;
    int tx, ty, tz;

    pitch = 0;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    ip = _ZN8dActor_c13ClosestPlayerEv(c);
    if (ip != 0) {
        if (*(u8 *)(ip + 0x6fb) == 1) {
            *(s16 *)(c + 0x444) = 0x1e;
            func_ov062_0211c658(c, &data_ov062_0211e17c);
            return 1;
        }

        t = *(Vector3 *)(ip + 0x5c);
        tx = t.x;
        {
            int y = *(int *)(ip + 0x644);
            t.y = y;
            t.y = y + 0x28000;
        }
        tz = t.z;
        ty = t.y;
        hv.x = tx; hv.y = ty; hv.z = tz;
        *(s16 *)(c + 0x44a) = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &hv);
        vv.x = tx; vv.y = ty; vv.z = tz;
        pitch = Vec3_VertAngle((Vector3 *)(c + 0x5c), &vv);

    }
    if (data_0209f2f8 == 0x10) {
        _Z14ApproachLinearRsss((s16 *)(c + 0x94), *(s16 *)(c + 0x44a), (s16)*(int *)(c + 0x43c));
        _Z14ApproachLinearRsss((s16 *)(c + 0x92), pitch, (s16)*(int *)(c + 0x43c));
    } else {
        _Z14ApproachLinearRsss((s16 *)(c + 0x94), *(s16 *)(c + 0x44a), (s16)(*(int *)(c + 0x43c) + 0x500));
        _Z14ApproachLinearRsss((s16 *)(c + 0x92), pitch, (s16)(*(int *)(c + 0x43c) + 0x500));
    }
    v.z = 0x1e000;
    Matrix4x3_FromRotationY(data_020a0e68, *(s16 *)(c + 0x8e));
    Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, *(s16 *)(c + 0x8c));
    MulVec3Mat4x3(&v, data_020a0e68, (Vector3 *)(c + 0xa4));
    func_ov062_0211b51c(c);
    if (*(u16 *)(c + 0x100) == 0 || _ZNK10dBgCh_Actr8IsOnWallEv(c + 0x178) != 0) {
        *(s16 *)(c + 0x444) = 0x1e;
        func_ov062_0211c658(c, &data_ov062_0211e17c);
    }
    return 1;
}
#pragma cplusplus on

// @symbol func_ov062_0211b930
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" int func_ov062_0211b930(Obj* o)
{
    dActor_c* found;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&(o->anim), *(((Data *)&data_ov062_0211e10c)->f), 4, 0x40000000, 0x1000, 0);
    o->f43c = 1;
    if (o->f44c != 0 && (found = dActor_c::FindWithID(o->f44c)) != 0) {
        Found* f = (Found*)found;
        f->f9c = -0x2000;
        f->fa0 = -0x32000;
        f->fc8 = 0;
        if (o->f468 == 1) {
            ((fBase_c*)found)->MarkForDestruction();
            if (o->f448 != 2) {
                dActor_c::Spawn(0xb2, o->f46c | 0x40, o->f5c, &o->f8c, o->fcc, -1);
            } else {
                dActor_c::Spawn(0xb3, 0x10, o->f5c, &o->f8c, o->fcc, -1);
                o->f468 = 0;
            }
        } else {
            func_02012790(0xa);
        }
        o->f43c = 0;
        o->f44c = 0;
        o->f444 = 0x1e;
    }
    func_02012694(0xef, (char*)o + 0x74);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x7e, o->f5c.x, o->f5c.y, o->f5c.z);
    o->fa4 = 0;
    o->fa8 = 0;
    o->fac = 0;
    o->f9c = 0;
    return 1;
}

// @symbol func_ov062_0211b8d8
extern "C" int func_ov062_0211b8d8(unsigned char* c) {
    int f = *(int*)(c + 0x38c);
    int fc = ((Animation*)(c + 0x384))->GetFrameCount();
    if ((int)((unsigned int)(f << 4) >> 16) >= fc - 1) {
        *(short*)(c + 0x444) = 0x1e;
        func_ov062_0211c658(c, &data_ov062_0211e17c);
    }
    return 1;
}

// @symbol func_ov062_0211b880
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" int func_ov062_0211b880(unsigned char* c) {
    *(int*)(c + 0x390) = 0x2000;
    *(int*)(c + 0xa4) = 0;
    *(int*)(c + 0xa8) = 0;
    *(int*)(c + 0xac) = 0;
    *(int*)(c + 0x9c) = 0;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim*)(c + 0x334), *(((BCA_File **)&data_ov062_0211e104)[1]), 4, 0, 0x1000, 0);
    return 1;
}

// @symbol func_ov062_0211b800
extern "C" {
int func_ov062_0211b800(char *c) {
    *(int*)(c + 0xa8) = 0x14000;
    _Z14ApproachLinearRsss((short*)(c + 0x92), 0, 0x300);
    _Z14ApproachLinearRsss((short*)(c + 0x96), 0, 0x300);
    if (*(int*)(c + 0x60) >= *(int*)(c + 0x434) - 0x2000) {
        *(int*)(c + 0xa4) = 0;
        *(int*)(c + 0xa8) = 0;
        *(int*)(c + 0xac) = 0;
        *(int*)(c + 0x60) = *(int*)(c + 0x434);
        *(int*)(c + 0x390) = 0x1000;
        func_ov062_0211c658(c, &data_ov062_0211e17c);
    }
    return 1;
}
}

// @symbol func_ov062_0211b51c
extern "C" {
int func_ov062_0211b51c(char *c)
{
    void *pl;
    void *sp;
    Vector3 pos;
    unsigned int flag;
    int fl;
    int id;
    enum Bool isPlayer;
    void *state;
    int r;
    u8 hc;
    u8 st;
    int newchar;
    int zero;

    id = *(int *)(c + 0x134);
    if (id != 0) {
        pl = dActor_c::FindWithID((unsigned int)id);
        if (pl == 0)
            return (int)pl;
        fl = *(int *)(c + 0x130);
        if ((fl & 0x4000) != 0)
            return func_ov062_0211c658(c, data_ov062_0211e14c);
        if ((fl & 0x27f0) != 0)
            return func_ov062_0211c658(c, data_ov062_0211e14c);
        isPlayer = (enum Bool)(((dActor_c *)pl)->actorID == 0xbf);
        if (isPlayer) {
            if (((dActor_c *)c)->BumpedUnderneathByPlayer(*(Player *)pl) == 1 ||
                *(u8 *)((char *)pl + 0x6f9) == 1 ||
                ((Player *)pl)->IsOnShell() == 1)
                return func_ov062_0211c658(c, data_ov062_0211e14c);
        }
    }

    id = *(int *)(c + 0x168);
    if (id == 0)
        return id;
    pl = dActor_c::FindWithID((unsigned int)id);
    if (pl == 0)
        return (int)pl;
    isPlayer = (enum Bool)(((dActor_c *)pl)->actorID == 0xbf);
    if (!isPlayer)
        return (int)isPlayer;
    state = data_ov062_0211e18c;
    if (*(void **)(c + 0x42c) != state)
        return (int)state;
    r = ((Player *)pl)->IsCollectingCap();
    if (r != 0)
        return r;
    r = *(int *)(c + 0x44c);
    if (r != 0)
        return r;

    st = *(u8 *)(c + 0x448);
    hc = *(u8 *)((char *)pl + 0x6d9);
    if (st != 2 &&
        *(u8 *)((char *)pl + 0x6ff) == 0 &&
        *(u8 *)((char *)pl + 0x6fd) == 0 &&
        *(u8 *)((char *)pl + 0x6fb) == 0 &&
        *(u8 *)((char *)pl + 0x6f9) == 0 &&
        *(int *)((char *)pl + 8) != 3) {
        newchar = *(int *)((char *)pl + 8);
        if (hc != newchar) {
            ((Player *)pl)->SetNewHatCharacter(hc, 0, 0);
        } else {
            r = SaveData::HasPlayerLostCap();
            if (r != 0)
                return r;
            SaveData::PlayerLoseCap();
        }
        {
            int area = *(signed char *)(c + 0xcc);
            unsigned int ch = *(unsigned int *)((char *)pl + 8);
            unsigned int param = 0;
            param = param | (ch << 8);
            sp = dActor_c::Spawn(0x10d, param, *(Vector3 *)(c + 0x5c), 0, area, -1);
        }
        if (sp != 0) {
            _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(
                sp, 0x64000, 0x258000, 0x1f40000, 0x1f40000);
            *(int *)(c + 0x44c) = *(int *)((char *)sp + 4);
            func_ov062_0211c658(c, data_ov062_0211e17c);
        }
    }

    *(u8 *)(c + 0x447) = 1;
    flag = 0;
    if (*(u8 *)(c + 0x448) == 2 || *(int *)((char *)pl + 8) == 3)
        flag = 1;

    pos.x = *(int *)(c + 0x5c);
    pos.y = *(int *)(c + 0x60);
    pos.z = *(int *)(c + 0x64);
    r = _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(pl, &pos, flag, 0xc000, 1, 0, 1);
    if (r == 0)
        return r;
    *(u16 *)(c + 0x444) = 0x1e;
    return func_ov062_0211c658(c, data_ov062_0211e17c);
}
}

// @symbol func_ov062_0211b3ac
extern "C" int func_ov062_0211b3ac(char* sl)
{
    char* player;
    /* Byte buffer (see c2f4 section above for why: no implicit ctor). */
    char path[8];
    Vector3 node;
    Vector3 best;
    Vector3 ppos;
    int i;
    int bestIdx;
    int bestDist;

    {
        int *ctr = (int *)(sl + 0x460);
        *ctr = *ctr + 1;
        *ctr = *ctr & 7;
    }
    if (*(int *)(sl + 0x460) != 0)
        return 0;

    player = (char *)((dActor_c *)sl)->ClosestPlayer();
    _ZN7PathPtrC1Ev(&path);
    ((PathPtr *)&path)->FromID(*(u32*)(sl + 0x464));

    bestIdx = 0;
    node.x = bestIdx; node.y = bestIdx; node.z = bestIdx;
    best.x = bestIdx; best.y = bestIdx; best.z = bestIdx;

    if (player != 0) {
        int *pp = (int *)(player + 0x5c);
        ppos.x = *pp;
        i = bestIdx;
        ppos.y = pp[1];
        bestDist = 0x10000000;
        ppos.z = pp[2];
        while (i < 4) {
            ((PathPtr *)path)->GetNode(node, (u32)i);
            if (bestDist > Vec3_HorzDist(&node, &ppos)) {
                bestIdx = i;
                bestDist = Vec3_HorzDist(&node, &ppos);
                best = node;
            }
            i = i + 1;
        }
    }

    if (Vec3_HorzDist(&best, (Vector3*)(sl + 0x430)) == 0)
        return 0;

    if (data_0209f2f8 == 0x10) {
        *(unsigned char*)(sl + 0x446) = 0;
        if (*(int*)(sl + 0x474) == (bestIdx ^ 2))
            *(unsigned char*)(sl + 0x446) = 1;
    }
    *(int*)(sl + 0x474) = bestIdx;
    return 1;
}

// @symbol func_ov062_0211b2fc
extern "C" {
void func_ov062_0211b2fc(char* c){
  Vector3 v[2];
  v[0].z = 0;
  v[0].x = 0;
  v[0].y = 0;
  v[0].z = 0;
  v[1].x = 0;
  v[1].y = 0;
  v[1].z = 0;
  v[0].x = 0;
  v[0].y = 0;
  v[0].z = 0x14000;
  if (*(int*)(c+0x468) == 0 && *(int*)(c+0x44c) == 0) {
    if (*(unsigned short*)(c+0x444) == 0) {
      short* a = (short*)(((int)c + 0x94));
      *a = *a - 0x100;
    }
    v[0].z = 0x28000;
  }
  Matrix4x3_FromRotationY(data_020a0e68, *(short*)(c+0x8e));
  Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, *(short*)(c+0x8c));
  MulVec3Mat4x3(&v[0], data_020a0e68, (Vector3*)(c+0xa4));
}
}

// @symbol _ZN9daJango_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases, then
 * return the object to its heap. Nobody writes that; declaring `~daJango_c()` is
 * enough, because mwcc emits D2, D0 and D1 together and objisolate keeps the
 * one this file is bound to.
 *
 * The deallocation is an inline operator delete -- dEnemyBase_c's, reachable because
 * dEnemyBase_c is this class's IMMEDIATE base.
 *
 * (No out-of-line definition here: the header carries the single inline
 * definition for the merged TU; the compiler emits D0 from the visible body.)
 */

// @symbol _ZN9daJango_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and six destructor calls, every one a consequence of
 * `struct daJango_c : dEnemyBase_c` and the members that declaration types, destroyed in
 * reverse declaration order, then dEnemyBase_c::~dEnemyBase_c.
 *
 * This body is the evidence for the header: each member's size closes exactly
 * on the next one's offset.
 *
 * (No out-of-line definition here either, for the same single-definition reason.)
 */
