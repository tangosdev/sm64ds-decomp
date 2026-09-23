//cpp
/* Champignon foe (ov077/daPopoi_c), 22 functions: free helpers plus
 * class members. Hand-assembled in REVERSE ROM order (highest address
 * first); the per-function include and declaration blocks are the
 * assembly scaffolding. Do not reorder.
 *
 * Leftover: the func_ov077 helpers keep linker names; naming belongs
 *   at their definitions.
 * Leftover: the file homes keep decl_common.h's char spelling (shared
 *   header owns them); the Init loads stay mangled accordingly.
 * Leftover: unk_400 and friends are unrecovered header fields.
 */

extern "C" {
// @symbol daPopoi_c_classInit
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_dCcAcPos_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
#include "daPopoi_c.h"
#include "Model.h"
#include "Player.h"

/* resolved: VT0 = _ZTV9daPopoi_c */
int *daPopoi_c_classInit(void)
{
    return (int *)new daPopoi_c;
}
}

// @symbol _ZN9daPopoi_c13InitResourcesEv
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void*);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void*, void*, int, int, unsigned int, unsigned int);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void*, void*, void*, int, int, unsigned int, unsigned int);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void*, void*, int, int, void*, int);
extern int func_ov077_02126d5c(void*, void*);
extern Vector3 data_ov077_02127a5c;
}

int daPopoi_c::InitResources()
{
  Vector3 v;
  mModelAnim.SetFile((BMD_File *)_ZN5Model8LoadFileER13SharedFilePtr(&data_ov077_02127c88), 1, -1);
  _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov077_02127ca0);
  _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov077_02127c90);
  _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov077_02127c98);
  mVertAccel = -0x1000;
  mTerminalVelocity = -0x1e000;
  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x52000, 0x52000, 0x800004, 0);
  v.x = data_ov077_02127a5c.x;
  v.y = data_ov077_02127a5c.y;
  v.z = data_ov077_02127a5c.z;
  _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &v, 0x54000, 0x32000, 0x200004, 0);
  mAngleY = mPrevAngleY;
  _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x64000, 0x64000, 0, 0);
  unk_400 = 0;
  unk_404 = mPosX;
  unk_408 = mPosY;
  unk_40c = mPosZ;
  mModelAnim.speed = 0x1000;
  unk_410 = mPosX;
  unk_414 = mPosY;
  unk_418 = mPosZ;
  func_ov077_02126d5c(((char*)this), &data_ov077_02127ce8);
  return 1;
}

#include "types.h"
// @symbol _ZN9daPopoi_c8BehaviorEv
/* The state machine, as this member sees it. unk_3fc points at a record whose
 * third word is the handler. StateOwner is deliberately incomplete: mwccarm
 * 2004/b56 picks the pointer-to-member representation from class completeness.
 * Do not substitute daPopoi_c without a byte check. */
struct StateOwner;
typedef void (StateOwner::*StateFn)();
struct StateRecord { char pad[8]; StateFn handler; };
struct dCc_c;
struct dBgCh_Actr;
extern "C" {
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, dCc_c *cc);
int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *self, Vector3 *v);
int func_02010844(void *unused, Vector3 *v, s16 angle);
int _ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi(void *self, dBgCh_Actr *wm, Fix12i a, s16 b, int c, int d, void *e);
void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void *self, dBgCh_Actr *wm, unsigned int j);
void func_ov077_02126dac(char *t);
void func_ov077_02126528(char *c);
void _ZN5dCc_c5ClearEv(dCc_c *self);
void _ZN5dCc_c6UpdateEv(dCc_c *self);
void _ZN9Animation7AdvanceEv(void *self);
extern int data_0209f32c;
}

int daPopoi_c::Behavior()
{
    int goingOffCliff;
    Vector3 floorNormal;
    int slope;
    StateRecord *state;

    /* Below the water surface (data_0209f32c): snap back to the spawn position
     * and skip the frame. */
    if (mPosY < data_0209f32c) {
        mPosX = unk_404;
        mPosY = unk_408;
        mPosZ = unk_40c;
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)((char *)&mStateTimer));
    DecIfAbove0_Short((unsigned short *)((char *)&unk_426));

    state = *(StateRecord **)((char *)&unk_3fc);
    if (state->handler != 0)
        (((StateOwner *)((char *)this))->*(state->handler))();

    _ZN8dActor_c9UpdatePosEP5dCc_c(((char *)this), (dCc_c *)((char *)&mdCcAcPos_c));

    /* Slope under the feet, as an angle relative to the way we are facing. */
    slope = 0;
    if (_ZNK10dBgCh_Actr10IsOnGroundEv((char *)&mWithMeshClsn)) {
        void *floorResult = _ZNK10dBgCh_Actr14GetFloorResultEv((char *)&mWithMeshClsn);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)floorResult + 4, &floorNormal);
        slope = func_02010844(((char *)this), &floorNormal, mAngleY);
    }

    /* Roll back to last frame's position if this step would walk off a ledge,
     * or onto ground tilted more than 0x100 either way. */
    goingOffCliff = _ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi(((char *)this), (dBgCh_Actr *)((char *)&mWithMeshClsn), 0x3c000, (s16)0x2888, 0, 1, (void *)0x32000);
    if (goingOffCliff == 0) {
        if (slope < 0)
            slope = (s16)-slope;
        if (slope <= 0x100)
            goto writeback;
    }
    mPosX = unk_410;
    mPosY = unk_414;
    mPosZ = unk_418;
writeback:
    unk_410 = mPosX;
    unk_414 = mPosY;
    unk_418 = mPosZ;
    _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(((char *)this), (dBgCh_Actr *)((char *)&mWithMeshClsn), 2);

    mAngleY = mPrevAngleY;
    func_ov077_02126dac(((char *)this));

    if (_ZNK10dBgCh_Actr10IsOnGroundEv((char *)&mWithMeshClsn) && *(void **)((char *)&unk_3fc) != (void *)data_ov077_02127cd8) {
        func_ov077_02126528(((char *)this));
    }
    _ZN5dCc_c5ClearEv((dCc_c *)((char *)&mdCcAc_c));
    _ZN5dCc_c6UpdateEv((dCc_c *)((char *)&mdCcAc_c));
    _ZN5dCc_c5ClearEv((dCc_c *)((char *)&mdCcAcPos_c));
    _ZN5dCc_c6UpdateEv((dCc_c *)((char *)&mdCcAcPos_c));

    mModelAnim.Advance();
    return 1;
}

// @symbol _ZN9daPopoi_c6RenderEv
extern int data_0209f32c;

/* Just enough of ModelAnim's vtable to reach ModelAnim::Render at slot 5
 * (offset 0x14). The five leading virtuals exist only to place that slot;
 * declaring the real class here would drag in its bases, and ModelAnim is
 * multiply derived. */
struct ModelAnimDraw {
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void Render(int);  /* vtable offset 0x14 */
};

int daPopoi_c::Render()
{
    /* Nothing below the water surface is drawn. */
    if (mPosY < data_0209f32c) return 1;
    ModelAnimDraw *model = (ModelAnimDraw *)((char *)&mModelAnim);
    model->Render(0);
    return 1;
}

// @symbol _ZN9daPopoi_c16OnPendingDestroyEv
/* recovered: shared header, real C++ method
 *
 * fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`.
 */

void daPopoi_c::OnPendingDestroy()
{
}

// @symbol _ZN9daPopoi_c16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the 4 shared file(s) InitResources claimed.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "SharedFilePtr.h"


int daPopoi_c::CleanupResources()
{
    ((SharedFilePtr *)&data_ov077_02127c88)->Release();
    ((SharedFilePtr *)&data_ov077_02127ca0)->Release();
    ((SharedFilePtr *)&data_ov077_02127c90)->Release();
    ((SharedFilePtr *)&data_ov077_02127c98)->Release();
    return 1;
}

// @symbol func_ov077_02126dac
extern "C" {
extern void Matrix4x3_FromRotationY(void *, int);
void func_ov077_02126dac(char *t)
{
    Matrix4x3_FromRotationY(t + 0x35c, *(short *)(t + 0x8e));
    *(int *)(t + 0x380) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x384) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x388) = *(int *)(t + 0x64) >> 3;
}
}

// @symbol func_ov077_02126d5c
/* Install a state and run its entry action on the same frame.
 *
 * The handler is read back out of the field after the store rather than reused
 * from the argument -- mwccarm emits the str and then an ldr of the same slot,
 * and sourcing it from `p` instead collapses that pair.
 *
 * This member's shadow pointer-to-member returns int, where the one Behavior
 * declares returns void; they are separate typedefs on purpose. */
struct StateHost;
typedef int (StateHost::*StateEntryFn)();
struct StateHost { char pad[0x3fc]; StateEntryFn *state; };

extern "C" int func_ov077_02126d5c(void *vc, void *vp)
{
    StateHost *self = (StateHost *)vc;
    StateEntryFn *next = (StateEntryFn *)vp;

    self->state = next;

    StateEntryFn *installed = self->state;
    if (*installed == 0) return 1;
    return (self->**installed)();
}

// @symbol func_ov077_02126cd4
extern "C" {
extern unsigned int RandomIntInternal(void* s);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
extern int data_0209e650[];
int func_ov077_02126cd4(char* c){
  *(short*)(c+0x400+0x20) = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0xf) << 0xc);
  *(short*)(c+0x100) = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0x3f) + 0xaa);
  *(int*)(c+0x39c) = 0x1000;
  *(int*)(c+0x98) = 0x8000;
  *(int*)(c+0x41c) = 0;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x340, (void*)((int*)&data_ov077_02127c90)[1], 0, 0x1000, 0);
  return 1;
}
}

extern "C" {
// @symbol func_ov077_02126ad0
#include "decl_Player.h"
#include "common.h"
extern int Vec3_Dist(void* a, void* b);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int cc, void* v, unsigned int d);
extern int func_ov077_02126300(void* c);
extern int func_ov077_02126d5c(void* c, void* p);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void* p);
extern short Vec3_HorzAngle(void* a, void* b);
extern void _Z14ApproachLinearRsss(short* a, short b, short cc);
extern void* _ZN8dActor_c13ClosestPlayerEv(void* c);

extern char data_ov077_02127cf8[];

int func_ov077_02126ad0(char* c)
{
    int dist;
    char* player;
    struct Vector3 pp;

    dist = Vec3_Dist(c + 0x5c, c + 0x404);
    *(unsigned int*)(c + 0x428) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int*)(c + 0x428), 3, 0x186, c + 0x74, 0);

    if (func_ov077_02126300(c) != 0) {
        func_ov077_02126d5c(c, data_ov077_02127d18);
        return 1;
    }

    if (_ZNK10dBgCh_Actr8IsOnWallEv(c + 0x184) != 0) {
        *(int*)(c + 0x5c) = *(int*)(c + 0x410);
        *(int*)(c + 0x60) = *(int*)(c + 0x414);
        *(int*)(c + 0x64) = *(int*)(c + 0x418);
    }

    if (dist > 0x1f4000) {
        *(short*)(c + 0x420) = Vec3_HorzAngle(c + 0x5c, c + 0x404);
        if (*(unsigned short*)(c + 0x100) < 0x14)
            *(unsigned short*)(c + 0x100) = 0x14;
        _Z14ApproachLinearRsss((short*)(c + 0x94), *(short*)(c + 0x420), 0x400);
    }
    _Z14ApproachLinearRsss((short*)(c + 0x94), *(short*)(c + 0x420), 0x100);

    if (*(unsigned short*)(c + 0x100) < 0x64)
        *(int*)(c + 0x39c) = 0x1000 / (0x64 - *(unsigned short*)(c + 0x100));

    if (*(unsigned short*)(c + 0x100) == 0) {
        func_ov077_02126d5c(c, data_ov077_02127cf8);
        return 1;
    }

    if (*(unsigned short*)(c + 0x426) != 0)
        return 1;

    player = (char*)_ZN8dActor_c13ClosestPlayerEv(c);
    if (player != 0) {
        struct Vector3* src = (struct Vector3*)(((long)(player + 0x5c)));
        pp.x = src->x;
        pp.y = src->y;
        pp.z = src->z;
        if (Vec3_Dist(c + 0x404, &pp) < 0x3e8000
            && *(unsigned char*)(player + 0x6f9) == 0
            && *(unsigned char*)(player + 0x703) == 0
            && _ZN6Player12GetHurtStateEv(player) < 0) {
            func_ov077_02126d5c(c, data_ov077_02127d08);
            return 1;
        }
    }
    return 1;
}
}

extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int frame, int speed, unsigned int flags);
int func_ov077_02126a84(char *c) {
    *(int*)(c + 0x98) = 0;
    *(short*)(c + 0x100) = 0x46;
    *(int*)(c + 0x39c) = 0x1000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x340, (void*)((void**)&data_ov077_02127c98)[1], 0, 0x1000, 0);
    return 1;
}
}

extern "C" {
extern int func_ov077_02126d5c(void*, void*);

int func_ov077_02126a50(char *c) {
    unsigned short h = *(unsigned short*)(c + 0x100);
    if (h == 0) {
        func_ov077_02126d5c(c, &data_ov077_02127ce8);
    }
    return 1;
}
}

extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);

int func_ov077_02126a04(char *c) {
    *(short*)(c + 0x420) = *(short*)(c + 0x8e) + 0x4000;
    *(int*)(c + 0x39c) = 0x1000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x340, ((void**)&data_ov077_02127c90)[1], 0, 0x1000, 0);
    return 1;
}
}

extern "C" {
extern void _Z14ApproachLinearRsss(short*, short, short);
extern int func_ov077_02126d5c(void*, void*);
int func_ov077_021269a8(char* c) {
    extern int AngleDiff(short, short); /* this file's own view (decl_common says int,int); short is byte-load-bearing here */
    short tgt = *(short*)(c + 0x420);
    _Z14ApproachLinearRsss((short*)(c + 0x94), tgt, 0x500);
    int diff = AngleDiff(*(short*)(c + 0x94), *(short*)(c + 0x420));
    if (diff < 0x100) {
        *(short*)(c + 0x426) = 0x1e;
        func_ov077_02126d5c(c, &data_ov077_02127ce8);
    }
    return 1;
}
}

extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
extern unsigned int RandomIntInternal(void* s);
extern int data_0209e650[];
int func_ov077_02126930(char* c){
  *(short*)(c+0x100) = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0x3f) + 0xaa);
  *(int*)(c+0x98) = 0x6000;
  *(short*)(c+0x400+0x22) = 0;
  *(int*)(c+0x41c) = 0;
  *(int*)(c+0x39c) = 0x2000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x340, (void*)((int*)&data_ov077_02127c90)[1], 0, 0x1000, 0);
  return 1;
}
}

extern "C" {
// @symbol func_ov077_0212679c
extern int Vec3_Dist(void *a, void *b);
extern int func_ov077_02126300(void *c);
extern int func_ov077_02126d5c(void *c, void *p);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void *thiz);
extern void* _ZN8dActor_c13ClosestPlayerEv(void* thiz);
extern short Vec3_HorzAngle(void *a, void *b);
extern void _Z14ApproachLinearRsss(short *r, short a, short b);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, void *v, unsigned int d);
extern char data_ov077_02127cf8[];

int func_ov077_0212679c(char *c)
{
    struct Vector3 pv;
    struct Vector3 *pp;
    int dist;
    char *p;
    unsigned short spd;

    dist = Vec3_Dist((void *)(c + 0x5c), (void *)(c + 0x404));

    if (func_ov077_02126300(c) != 0) {
        func_ov077_02126d5c(c, data_ov077_02127d18);
        return 1;
    }

    if (_ZNK10dBgCh_Actr8IsOnWallEv(c + 0x184) != 0) {
        *(int *)(c + 0x5c) = *(int *)(c + 0x410);
        *(int *)(c + 0x60) = *(int *)(c + 0x414);
        *(int *)(c + 0x64) = *(int *)(c + 0x418);
    }

    p = (char *)_ZN8dActor_c13ClosestPlayerEv(c);
    if (p != 0) {
        /* u64 launder forces base materialization after the null cmp */
        pp = (struct Vector3 *)(void *)(unsigned long long)(unsigned long)(p + 0x5c);
        pv.x = pp->x;
        pv.y = pp->y;
        pv.z = pp->z;
        *(short *)(c + 0x420) = Vec3_HorzAngle((void *)(c + 0x5c), &pv);
    }

    _Z14ApproachLinearRsss((short *)(c + 0x94), *(short *)(c + 0x420), *(short *)(c + 0x422));
    _Z14ApproachLinearRsss((short *)(c + 0x422), 0x600, 0x100);

    *(unsigned int *)(c + 0x428) = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        *(unsigned int *)(c + 0x428), 3, 0x186, c + 0x74, 0);

    spd = *(unsigned short *)(c + 0x100);
    if (spd < 0x64) {
        *(int *)(c + 0x39c) = __aeabi_idiv(0x1000, 5 - spd / 20);
    }

    if (dist > 0x5dc000 || *(unsigned short *)(c + 0x100) == 0 || func_ov077_02126300(c) != 0)
        func_ov077_02126d5c(c, data_ov077_02127cf8);

    return 1;
}
}

extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* c, void* f, int a, int b, unsigned int u);
int func_ov077_02126758(char* c){
  *(int*)(c+0x98)=0;
  *(int*)(c+0x39c)=0x1000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x340, ((void**)&data_ov077_02127ca0)[1], 0x40000000, 0x1000, 0);
  return 1;
}
}

// @symbol func_ov077_02126640
/* daTgz_c::Kill - recovered from vtable slot identity */
/* (Vector3: real header type in scope) */
extern "C" {
void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void *thiz, const Vector3 &v);
void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
int _ZN6Player12GetHurtStateEv(void *p);
int func_ov002_020db674(void *c, int a1, int a2, int a3);
void func_02012694(int a, void *b, int c);
int _ZN9Animation8FinishedEv(void *p);
int func_ov077_02126d5c(void *c, void *p);
extern char data_ov077_02127cf8[];
}

extern "C" int func_ov077_02126640(char *c)
{
    Vector3 v;
    char *a;
    int t;
    if (((daPopoi_c *)c)->unk_400 != 0) {
        v.x = data_ov077_02127a5c.x;
        v.y = data_ov077_02127a5c.y;
        v.z = data_ov077_02127a5c.z;
        ((daPopoi_c *)c)->mdCcAcPos_c.SetPosRelativeToActor(v);
        if (*(int *)(c + 0x168) != 0) {
            a = (char *)_ZN8dActor_c10FindWithIDEj(*(int *)(c + 0x168));
            if (a != 0) {
                if (a == (char *)((daPopoi_c *)c)->unk_400) {
                    if (((Player *)a)->mIsMetal == 0) {
                        if (((Player *)a)->mIsMega == 0) {
                            t = (((Player *)a)->mIsNoControl != 0);
                            if (t == 0) {
                                if (((Player *)a)->GetHurtState() < 0) {
                                    if (func_ov002_020db674(a, 0x28000, 0x70000,
                                            (int)(short)(*(short *)(c + 0x8e) + 0x8000)) != 0) {
                                        ((daPopoi_c *)c)->unk_400 = 0;
                                        func_02012694(0x10b, c + 0x74, 0);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (_ZN9Animation8FinishedEv(c + 0x390) != 0) {
        func_ov077_02126d5c(c, &data_ov077_02127cf8);
    }
    return 1;
}

/* (Vector3: real header type in scope) */
extern "C" {
void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void *thiz, const Vector3 &v);
void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
int _ZN6Player12GetHurtStateEv(void *p);
int _ZN6Player15IsCollectingCapEv(void *p);
int func_ov077_02126d5c(void *c, void *p);
extern char data_ov077_02127cd8[]; /* decl_common's view; only its address is taken here */
}

extern "C" void func_ov077_02126528(char *c)
{
    Vector3 v;
    char *a;
    int t;
    v.x = data_ov077_02127a5c.x;
    v.y = data_ov077_02127a5c.y;
    v.z = data_ov077_02127a5c.z;
    ((daPopoi_c *)c)->mdCcAcPos_c.SetPosRelativeToActor(v);
    if (*(int *)(c + 0x168) == 0) return;
    a = (char *)_ZN8dActor_c10FindWithIDEj(*(int *)(c + 0x168));
    if (a == 0) return;
    t = (*(unsigned short *)(a + 0xc) == 0xbf);
    if (t == 0) return;
    if (*(unsigned char *)(a + 0x6f9) == 1) return;
    if (*(unsigned char *)(a + 0x703) == 1) return;
    t = (((Player *)a)->mIsNoControl != 0);
    if (t == 1) return;
    if (_ZN6Player12GetHurtStateEv(a) >= 0) return;
    if (_ZN6Player15IsCollectingCapEv(a) != 0) return;
    *(int *)(c + 0x400) = (int)a;
    func_ov077_02126d5c(c, &data_ov077_02127cd8);
}

extern "C" {
/* (Vector3: real header type in scope) */

typedef struct dBgCh_LinPad {
    char pad[0x78];
} dBgCh_LinPad;

extern signed char data_0209f2f8;
extern char data_020a0e68[];

extern void _ZN9dBgCh_LinC1Ev(void *self);
extern void _ZN9dBgCh_LinD1Ev(void *self);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(
    void *self, Vector3 *a, Vector3 *b, void *actor);
extern int _ZN9dBgCh_Lin10DetectClsnEv(void *self);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, int angle);
extern void MulVec3Mat4x3(void *in, void *m, void *out);

/* Probe ahead for a wall or a missing floor. Returns 1 if the way is blocked,
 * and in that case also rolls the actor back to last frame's position and zeroes
 * its horizontal speed (+0x98). Returns 0 when the path is clear.
 *
 * Two rays leave the actor's head height (+0x28000): a long level one 0xc8000
 * ahead, and a short one 0x2c000 ahead pitched down 0x3000. Blocked means
 * "the level ray hit something, OR the pitched ray found no ground".
 *
 * Guarded by data_0209f2f8 == 0x2a, so the probe only runs in one level.
 *
 * NOTE on the two `end.x = sx; end.x = sx + ox;` pairs below: the dead first
 * store is deliberate. mwccarm writes the base and then the sum, and folding
 * them into one assignment changes the store sequence this function's ROM bytes
 * record. Leave them. */
int func_ov077_02126300(void *vc)
{
    char *c = (char *)vc;
    dBgCh_LinPad ray1;
    dBgCh_LinPad ray2;
    Vector3 start;
    Vector3 end;
    Vector3 dir;
    Vector3 out;
    int y;

    if (data_0209f2f8 == 0x2a) {
        _ZN9dBgCh_LinC1Ev(&ray1);
        _ZN9dBgCh_LinC1Ev(&ray2);

        start.x = 0;
        start.y = 0;
        start.z = 0;
        end.x = 0;
        end.y = 0;
        end.z = 0;
        dir.x = 0;
        dir.y = 0;
        dir.z = 0;
        out.x = 0;
        out.y = 0;
        out.z = 0;

        start.x = *(int *)(c + 0x5c);
        y = *(int *)(c + 0x60);
        start.y = y;
        start.z = *(int *)(c + 0x64);
        start.y = y + 0x28000;
        dir.z = 0xc8000;
        Matrix4x3_FromRotationY(data_020a0e68, *(short *)(c + 0x8e));
        MulVec3Mat4x3(&dir, data_020a0e68, &out);
        {
            int sx = start.x;
            int ox = out.x;
            int sy = start.y;
            int sz = start.z;
            int oy;
            int oz;
            end.x = sx;
            end.x = sx + ox;
            oy = out.y;
            oz = out.z;
            end.y = sy;
            end.y = sy + oy;
            end.z = sz;
            end.z = sz + oz;
        }
        _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(&ray1, &start, &end, c);

        start.x = *(int *)(c + 0x5c);
        y = *(int *)(c + 0x60);
        start.y = y;
        start.z = *(int *)(c + 0x64);
        start.y = y + 0x28000;
        dir.x = 0;
        dir.y = 0;
        dir.z = 0x2c000;
        Matrix4x3_FromRotationY(data_020a0e68, *(short *)(c + 0x8e));
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, 0x3000);
        MulVec3Mat4x3(&dir, data_020a0e68, &out);
        {
            int sx = start.x;
            int ox = out.x;
            int sy = start.y;
            int sz = start.z;
            int oy;
            int oz;
            end.x = sx;
            end.x = sx + ox;
            oy = out.y;
            oz = out.z;
            end.y = sy;
            end.y = sy + oy;
            end.z = sz;
            end.z = sz + oz;
        }
        _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(&ray2, &start, &end, c);

        if (_ZN9dBgCh_Lin10DetectClsnEv(&ray1) != 0 ||
            _ZN9dBgCh_Lin10DetectClsnEv(&ray2) == 0) {
            *(int *)(c + 0x5c) = *(int *)(c + 0x410);
            *(int *)(c + 0x60) = *(int *)(c + 0x414);
            *(int *)(c + 0x64) = *(int *)(c + 0x418);
            *(int *)(c + 0x98) = 0;
            _ZN9dBgCh_LinD1Ev(&ray2);
            _ZN9dBgCh_LinD1Ev(&ray1);
            return 1;
        }
        _ZN9dBgCh_LinD1Ev(&ray2);
        _ZN9dBgCh_LinD1Ev(&ray1);
    }
    return 0;
}
}

// @symbol _ZN9daPopoi_cD0Ev
/* D0 is the DELETING destructor: destroy through this class (dEnemyBase_c
 * chain) then return the object to its heap via an inline operator delete.
 * Both variants are emitted from the single inline destructor in
 * daPopoi_c.h (class-form skill): D1 then D0 in ROM order, no leaf D2. */

/* (no separate definition: the single ~daPopoi_c() below emits the D0 and
 * D1 variants together; mwccarm orders the variant group itself.) */

// @symbol _ZN9daPopoi_cD1Ev
/* D1 is emitted from the inline destructor in daPopoi_c.h alongside D0
 * (class-form skill); this marker at D1's ROM ordinal keeps the
 * accounting naming it. Members are destroyed in reverse declaration
 * order, then dEnemyBase_c::~dEnemyBase_c. */

