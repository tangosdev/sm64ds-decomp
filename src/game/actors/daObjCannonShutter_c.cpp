//cpp
/* Canonical production translation unit -- reconstructed with tools/tubuild.py.
 * ov002/daObjCannonShutter_c  (8 function(s))
 *
 * The default stock build compiles this file once, derives its 8
 * exact text contributions, and substitutes them for retained one-function
 * comparison objects. The manifest partial-link proof and ordinary ROM build
 * both require the linked module and final ROM to remain byte-identical.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x020bc8f4  src/game/actors/daObjCannonShutter_c/_ZN20daObjCannonShutter_cD1Ev.cpp
 *   [1] 0x020bc938  src/game/actors/daObjCannonShutter_c/_ZN20daObjCannonShutter_cD0Ev.cpp
 *   [2] 0x020bc990  src/unnamed/ov002/020b/func_ov002_020bc990.c
 *   [3] 0x020bc9b0  src/game/actors/daObjCannonShutter_c/_ZN20daObjCannonShutter_c16CleanupResourcesEv.cpp
 *   [4] 0x020bc9f4  src/game/actors/daObjCannonShutter_c/_ZN20daObjCannonShutter_c6RenderEv.cpp
 *   [5] 0x020bca78  src/game/actors/daObjCannonShutter_c/_ZN20daObjCannonShutter_c8BehaviorEv.cpp
 *   [6] 0x020bcc20  src/game/actors/daObjCannonShutter_c/_ZN20daObjCannonShutter_c13InitResourcesEv.cpp
 *   [7] 0x020bcccc  src/game/actors/daObjCannonShutter_c/daObjCannonShutter_c_Spawn.c
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "daObjCannonShutter_c.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"

/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
/* shadow struct 'Sub' */
struct Sub {
  virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
  virtual void v10(); virtual void m14(int arg);
};

/* shadow struct 'Obj' */

/* shadow typedef 's16' */
typedef short s16;

/* shadow struct 'SharedFilePtr' */
struct SharedFilePtr; struct BMD_File; struct KCL_File; struct Matrix4x3; struct CLPS_Block;

extern "C" {
extern void func_0201277c(int);
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;
char *_ZN8dActor_c13ClosestPlayerEv(char *self);
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, int a, int b);
void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
void Matrix4x3_FromRotationY(void *m, int angle);
void MulVec3Mat4x3(Vector3 *v, void *m, Vector3 *dst);
void AddVec3(Vector3 *a, Vector3 *b, Vector3 *d);
int Vec3_Dist(void *a, void *b);
int Vec3_HorzDist(void *a, void *b);
daObjCannonShutter_c *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, daObjCannonShutter_c *after);
extern Matrix4x3 data_020a0e68;
extern struct SharedFilePtr data_ov002_0210e12c;
extern struct SharedFilePtr data_ov002_0210e124;
extern struct CLPS_Block data_ov002_0210d7f4;
struct BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, struct BMD_File *f, int a, int b);
struct KCL_File *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
void *self, struct KCL_File *k, struct Matrix4x3 &m, int fx, short s, struct CLPS_Block &c);
int IsCannonOpenInCurLevel(void);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daObjCannonShutter_c_Spawn, 0x020bcccc, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol daObjCannonShutter_c_Spawn
/* recovered: globals resolved, declarations from a shared header */
/* recovered: globals resolved */
/* resolved: VT = _ZTV20daObjCannonShutter_c */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int *daObjCannonShutter_c_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(816);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)(_ZTV20daObjCannonShutter_c + 2); }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN20daObjCannonShutter_c13InitResourcesEv, 0x020bcc20, size 0xac */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjCannonShutter_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
int daObjCannonShutter_c::InitResources()
{
    struct BMD_File *bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210e12c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii((unsigned char *)&mModel, bmd, 1, -1);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((unsigned char *)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((unsigned char *)this));
    {
        struct KCL_File *kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov002_0210e124);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            (unsigned char *)&mMeshCollider, kcl, *(struct Matrix4x3 *)&mClsnMat, 0x199,
            mAngleY, data_ov002_0210d7f4);
    }
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    if (IsCannonOpenInCurLevel() != 0) {
        mCannonOpen = 1;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN20daObjCannonShutter_c8BehaviorEv, 0x020bca78, size 0x1a8 */
/* -------------------------------------------------------------------------- */
int daObjCannonShutter_c::Behavior()
{
    if (mCannonOpen != 0) {
        if (*(u8 *)(_ZN8dActor_c13ClosestPlayerEv((char *)this) + 0x703) != 0) {
            mPosX = mHomePosX;
            mPosY = mHomePosY;
            mPosZ = mHomePosZ;
            if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0)) {
                _ZN10dBgActor_c21UpdateModelPosAndRotYEv(this);
                _ZN10dBgActor_c19UpdateClsnPosAndRotEv(this);
            }
        } else {
            if (mMeshCollider.IsEnabled()) {
                mMeshCollider.Disable();
            }
        }
        return 1;
    }
    if (mOpening != 0) {
        Vector3 in;
        Vector3 out;
        in.x = 0;
        in.y = 0;
        in.z = 0;
        out.x = 0;
        out.y = 0;
        out.z = 0;
        if (mOpenPhase != 0) {
            if (mOpenPhase == 1) in.z = 0x2000;
        } else {
            in.y = -0x1000;
        }
        Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
        MulVec3Mat4x3(&in, &data_020a0e68, &out);
        AddVec3((Vector3 *)&mPosX, &out, (Vector3 *)&mPosX);
        if (Vec3_Dist(&mHomePosX, &mPosX) > 0xa000) {
            mOpenPhase = 1;
        }
        if (Vec3_HorzDist(&mHomePosX, &mPosX) > 0xc8000) {
            daObjCannonShutter_c *a = _ZN8dActor_c15FindWithActorIDEjPS_(0xe, 0);
            while (a != 0) {
                a->mCannonOpen = 1;
                a = _ZN8dActor_c15FindWithActorIDEjPS_(0xe, a);
            }
        }
        _ZN10dBgActor_c21UpdateModelPosAndRotYEv(this);
    }
    if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0)) {
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(this);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN20daObjCannonShutter_c6RenderEv, 0x020bc9f4, size 0x84 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjCannonShutter_c6RenderEv
/* recovered: named members + shared header, real C++ method */
int daObjCannonShutter_c::Render()
{
  if (data_0209f2f8 == 6 && data_0209f220 == 1 && (int)(param1 & 0xff) == 1)
    return 1;
  if (mCannonOpen != 0)
    return 1;
  Sub* b = (Sub*)&mModel;
  b->m14(0);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN20daObjCannonShutter_c16CleanupResourcesEv, 0x020bc9b0, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjCannonShutter_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daObjCannonShutter_c::CleanupResources()
{
    if (((dBgW *)((char *)&mMeshCollider))->IsEnabled()) {
        ((dBgW *)((char *)&mMeshCollider))->Disable();
    }
    data_ov002_0210e12c.Release();
    data_ov002_0210e124.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov002_020bc990, 0x020bc990, size 0x20 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020bc990(void *c){
    func_0201277c(0x47);
    *(char*)((char*)c+0x32c)=1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN20daObjCannonShutter_cD0Ev, 0x020bc938, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjCannonShutter_cD0Ev
/* Not defined here: identical to _ZN20daObjCannonShutter_cD1Ev below (both legacy
 * files independently reconstructed the same real out-of-line
 * `daObjCannonShutter_c::~daObjCannonShutter_c(){}`) -- a plain D0/D1 collapse redefinition,
 * same shape as VirtualDoor/#1725, not the InvisiblePole/CameraTag/Exit/
 * WingFeather/Seaweed/HealingHeart mwcc ICE (which needs a hand-written
 * free-function D0 next to a real D1, not two real definitions). The
 * compiler emits D0 itself from the one definition kept below. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN20daObjCannonShutter_cD1Ev, 0x020bc8f4, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjCannonShutter_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Two vtable stores and three destructor calls, every one a consequence of
 * `struct daObjCannonShutter_c : dBgActor_c`: its own vptr, then dBgActor_c's -- inlined,
 * because dBgActor_c's destructor is defined in its class body -- then
 * dBgActor_c's Model and dBgW_KcMbg, then dActor_c. This class adds no
 * member with a destructor of its own.
 */
daObjCannonShutter_c::~daObjCannonShutter_c()
{
}
