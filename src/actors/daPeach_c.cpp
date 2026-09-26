//cpp
/* Princess Peach in the castle courtyard (ov085/daPeach_c), 25
 * functions: states, talk, model, factory. Source ROM-ascending
 * under defer_codegen off (D1 below D0, no D2). Do not reorder.
 *
 * deslop
 * Leftover: the func_ov085 helper keeps its linker name; the
 *   ModelAnim SetAnim/Advance calls keep computed spellings
 *   (by-value Fix12<int> parameters, wall 6az).
 * Leftover: raw offsets on c (state bytes, second anim) are
 *   unrecovered header fields.
 */
#include "common.h"
#include "daPeach_c.h"
#include "dActor_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Animation.h"
#include "dCc_c.h"
#include "types.h"

struct BMD_File;
struct Vector3_16;

/* ModelAnim's fourth virtual slot, reached through a four-slot stand-in.
 * Behavior calls it on the ModelAnim at +0xd4; the real declaration lives
 * with that class, and this file only needs the dispatch. */
struct PeachAnimSlots {
    virtual int g0();
    virtual int g1();
    virtual int g2();
    virtual int g3();
};

/* The twelve-word body of a Matrix4x3, copied whole in and out of the
 * scratch matrix. */
typedef struct { int w[12]; } PeachM48;

extern "C" {

/* math / vector helpers */
extern int AngleDiff(int, int);
extern int Vec3_HorzDist(const void *a, const void *b);
extern short Vec3_HorzAngle(const void *a, const void *b);
extern short Vec3_VertAngle(const void *a, const void *b);
extern int _Z14ApproachLinearRsss(short *dst, short target, short rate);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, short angY);
extern void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short angZ);
extern int data_020a0e68[];

/* mesh collision */
extern int dBgCh_Actr_UpdateContinuous_Veneer(void *c);
extern void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *c);
extern void *_ZNK10dBgCh_Actr13GetWallResultEv(void *c);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *self, dActor_c *a, int b, int c, Vector3_16 *d, Vector3_16 *e);

/* actor plumbing */
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *thiz, void *sm, void *mtx, int f, int g, unsigned int h);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *self, dActor_c *a, int b, int c, unsigned int d, unsigned int e);

/* model / animation */
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *self, void *bca, int frame, int speed, unsigned int flags);

/* ov085 statics. data_ov085_0212f280 and data_ov085_021304f4 keep
 * include/decl_common.h's spelling, so this file adds no new reading of
 * either symbol. */
extern int data_ov085_0212f280[];      /* the seven animation files */
extern char data_ov085_021304f4;       /* the model file */
extern void *data_ov085_021304bc[];
extern int data_ov085_021304c4[];
extern void *data_ov085_021304d4[];
extern int data_ov085_021304e4[];
extern void *data_ov085_021304ec[];
extern daPeach_c::StateFunc data_ov085_0213055c[];

}

#pragma defer_codegen off

/* ROM ordinals 0 and 1 -- _ZN9daPeach_cD1Ev 0x02129d18 size 0x48,
                           _ZN9daPeach_cD0Ev 0x02129d60 size 0x5c */
// @symbol _ZN9daPeach_cD1Ev
// @symbol _ZN9daPeach_cD0Ev
/* ONE declaration, TWO ROM functions. mwccarm emits the complete variant D1
 * and the deleting variant D0 from the single destructor this class declares,
 * so D0 has no source of its own; with deferred code generation off they land
 * in the cartridge's order, D1 first at 0x02129d18 and D0 at 0x02129d60.
 *
 * There is nothing to write. The vptr store and the four member destructor
 * calls -- dBgCh_Actr at 0x194, dCcAc_c at 0x160, ShadowModel at 0x138,
 * ModelAnim at 0xd4 -- and then dActor_c's own step are every one a
 * consequence of `struct daPeach_c : dActor_c` and the members that
 * declaration types. D0's deallocation is the inline operator delete reached
 * through fBase_c, which is why nothing here mentions a heap.
 *
 * daPeach_c declares its destructor in the class body. The two functions
 * below demand D1 and D0, in that order: a destructor call, then a
 * delete-expression. Nothing calls either one, so the link drops them.
 * include/daPeach_c.h stays as it is. */
void PeachDemandCompleteDtor(daPeach_c *peach)
{
    peach->~daPeach_c();
}

void PeachDemandDeletingDtor(daPeach_c *peach)
{
    delete peach;
}

// @symbol _ZN9daPeach_c12UpdateLookAtEv
/* Peach turns her head toward the nearest player, but only when he is close
 * enough (0x15e000) and roughly in front of her (within 0x3000 of her
 * facing). Otherwise both target angles fall back to zero and she looks
 * straight ahead. The two ApproachLinear calls make the turn gradual. */
void daPeach_c::UpdateLookAt()
{
    char *s = (char *)this;
    Player *p = ClosestPlayer();
    if (p == 0)
        return;
    Vector3 v;
    Vector3 *psrc = (Vector3 *)(((long long)(int)((char *)p + 0x5c)));
    v = *psrc;
    int hd = Vec3_HorzDist(s + 0x5c, &v);
    v.y = v.y - 0x1e000;
    short ha = Vec3_HorzAngle(s + 0x5c, &v);
    short va = Vec3_VertAngle(s + 0x5c, &v);
    if (hd < 0x15e000 && AngleDiff(ha, *(short *)(s + 0x8e)) < 0x3000) {
        *(short *)(s + 0x364) = va;
        *(short *)(s + 0x366) = *(short *)(s + 0x8e) - ha;
    } else {
        *(short *)(s + 0x364) = 0;
        *(short *)(s + 0x366) = 0;
    }
    _Z14ApproachLinearRsss((short *)(s + 0x362), *(short *)(s + 0x366), 0x250);
    _Z14ApproachLinearRsss((short *)(s + 0x360), *(short *)(s + 0x364), 0x100);
}

/* ROM ordinal 3 -- _ZN9daPeach_c21UpdateGroundCollisionEP10dBgCh_Actr,
                    0x02129ebc, size 0xd0 */
// @symbol _ZN9daPeach_c21UpdateGroundCollisionEP10dBgCh_Actr
/* The floor normal becomes a pitch at +0xa8 so Peach stands square on a
 * slope. The wall branch reads its normal and drops it -- the ROM computes
 * the copy and uses nothing of it. */
void daPeach_c::UpdateGroundCollision(dBgCh_Actr *clsn)
{
    int *self = (int *)this;
    int n0[3];
    int n1[3];
    dBgCh_Actr_UpdateContinuous_Veneer(clsn);
    if (clsn->IsOnGround()) {
        ((SurfaceInfo *)((char *)_ZNK10dBgCh_Actr14GetFloorResultEv(clsn) + 4))->CopyNormalTo(*(Vector3 *)n0);
        if (n0[1] != 0) {
            long long a = (long long)n0[0] * (long long)self[0xa4 / 4];
            long long b = (long long)n0[2] * (long long)self[0xac / 4];
            int x = (int)((a + 0x800) >> 12);
            int y = (int)((b + 0x800) >> 12);
            self[0xa8 / 4] = -(_ZN4cstd4fdivEii(x + y, n0[1]) + 0x8000);
        }
    }
    if (clsn->IsOnWall()) {
        ((SurfaceInfo *)((char *)_ZNK10dBgCh_Actr13GetWallResultEv(clsn) + 4))->CopyNormalTo(*(Vector3 *)n1);
    }
}

// @symbol func_ov085_02129f8c
/* The one member of this TU the cartridge does not name: a free function with
 * C linkage, called by InitState0 and InitState4. It asks whether the actor
 * whose id sits at +0x184 is still alive and is still kind 0xbf. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_02129f8c(char *c) {
    unsigned int id = *(unsigned int *)(c + 0x184);
    void *actor;
    unsigned short kind;
    int r;
    if (id == 0) return id;
    actor = dActor_c::FindWithID(id);
    if (actor == 0) return (int)actor;
    kind = *(unsigned short *)((char *)actor + 0xc);
    if (kind == 0xbf) r = 1; else r = 0;
    /* Both paths return r. The cartridge still compares r with zero first;
     * a single return lets the compiler delete that compare. */
    if (r == 0)
        goto done;
    return r;
done:
    return r;
}
}

// @symbol _ZN9daPeach_c11UpdateModelEv
/* The model matrix at +0xf0 is rebuilt from the body angle and the position,
 * then the head joint's own matrix (+0x360 inside the animated model) is
 * rotated by the two look angles through the scratch matrix at
 * data_020a0e68. */
void daPeach_c::UpdateModel()
{
    char *c = (char *)this;
    Matrix4x3_FromRotationY(c + 0xf0, *(short *)(c + 0x8e));
    *(int *)(c + 0x114) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x118) = *(int *)(c + 0x60) >> 3;
    *(int *)(c + 0x11c) = *(int *)(c + 0x64) >> 3;
    *(PeachM48 *)data_020a0e68 = *(PeachM48 *)(*(char **)(c + 0xe8) + 0x360);
    Matrix4x3_ApplyInPlaceToRotationY(data_020a0e68, *(short *)(c + 0x362));
    Matrix4x3_ApplyInPlaceToRotationZ(data_020a0e68, *(short *)(c + 0x360));
    *(PeachM48 *)(*(char **)(c + 0xe8) + 0x360) = *(PeachM48 *)data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x138, c + 0xf0, 0x8c000, 0x32000, 0xf);
}

// @symbol _ZN9daPeach_c10InitState0Ev
int daPeach_c::InitState0()
{
    char *c = (char *)this;
    ((dActor_c *)c)->UpdatePos((dCc_c *)(c + 0x160));
    UpdateGroundCollision(&mWithMeshClsn);
    func_ov085_02129f8c(c);
    return 1;
}

// @symbol _ZN9daPeach_c6State0Ev
int daPeach_c::State0()
{
    char *c = (char *)this;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov085_021304ec[1], 0, 0x1000, 0);
    ((Animation *)(c + 0x124))->SetFlags(0x40000000);
    *(int *)(c + 0x98) = 0x4000;
    *(int *)(c + 0xa8) = 0xa000;
    *(int *)(c + 0x354) = 4;
    return 1;
}

// @symbol _ZN9daPeach_c6State2Ev
int daPeach_c::State2()
{
    return 1;
}

// @symbol _ZN9daPeach_c10InitState2Ev
int daPeach_c::InitState2()
{
    char *c = (char *)this;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov085_021304e4[1], 0, 0x1000, 0);
    *(int *)(c + 0x98) = 0x4000;
    *(int *)(c + 0x354) = 3;
    return 1;
}

// @symbol _ZN9daPeach_c10InitState4Ev
int daPeach_c::InitState4()
{
    char *c = (char *)this;
    short v = *(short *)(c + 0x8e);
    *(short *)(c + 0x94) = v;
    ((dActor_c *)c)->UpdatePos((dCc_c *)(c + 0x160));
    UpdateGroundCollision(&mWithMeshClsn);
    func_ov085_02129f8c(c);
    return 1;
}

// @symbol _ZN9daPeach_c6State1Ev
int daPeach_c::State1()
{
    char *c = (char *)this;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov085_021304c4[1], 0, 0x1000, 0);
    *(int *)(c + 0x98) = 0x1800;
    *(int *)(c + 0x354) = 2;
    return 1;
}

// @symbol _ZN9daPeach_c6State3Ev
/* The conversation, in three steps held in the u8 at +0x368: start the talk,
 * turn to face the player and put the message up, then wait for the player's
 * talk state to go back to -1. */
int daPeach_c::State3()
{
    char *c = (char *)this;
    Vector3 v;
    switch (*(u8 *)(c + 0x368)) {
    case 0:
        if (((daPeach_c *)c)->mTalkPlayer->StartTalk(*(fBase_c *)c, 1) != 0)
            *(u8 *)(((int)c + 0x368)) += 1;
        break;
    case 1:
        if (_Z14ApproachLinearRsss((s16 *)(c + 0x8e),
                Vec3_HorzAngle((Vector3 *)(c + 0x5c),
                               (Vector3 *)&((daPeach_c *)c)->mTalkPlayer->mPosX),
                0x514) != 0) {
            v.x = *(int *)(c + 0x5c);
            v.y = *(int *)(c + 0x60);
            v.z = *(int *)(c + 0x64);
            v.y = v.y + 0xa0000;
            if (((daPeach_c *)c)->mTalkPlayer->ShowMessage(*(fBase_c *)c, 0xd0, &v, 0, 0) != 0)
                *(u8 *)(((int)c + 0x368)) += 1;
        }
        break;
    case 2:
        if (((daPeach_c *)c)->mTalkPlayer->GetTalkState() == -1)
            SetState(0);
        break;
    }
    return 1;
}

// @symbol _ZN9daPeach_c10InitState1Ev
int daPeach_c::InitState1()
{
    char *c = (char *)this;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)&((daPeach_c *)c)->mModelAnim, data_ov085_021304d4[1], 0, 0x1000, 0);
    *(int *)(c + 0x12c) = 0;
    ((Animation *)(c + 0x124))->Advance();
    *(char *)(c + 0x368) = 1;
    *(int *)(c + 0x354) = 1;
    return 1;
}

// @symbol _ZN9daPeach_c10InitState3Ev
/* The same liveness question func_ov085_02129f8c asks, written out here
 * instead of called -- the ROM inlines it at this one site. */
int daPeach_c::InitState3()
{
    char *c = (char *)this;
    if (*(int *)(c + 0x180) & 0x8000000) {
        char *a = (char *)dActor_c::FindWithID(*(unsigned int *)(c + 0x184));
        if (a) {
            int match = (((dActor_c *)a)->actorID == 0xbf) ? 1 : 0;
            if (match != 0) {
                ((daPeach_c *)c)->mTalkPlayer = (Player *)a;
                if (((daPeach_c *)c)->mTalkPlayer->StartTalk(*(fBase_c *)c, false)) {
                    SetState(1);
                }
            }
        }
    }
    return 1;
}

// @symbol _ZN9daPeach_c6State4Ev
int daPeach_c::State4()
{
    char *c = (char *)this;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov085_021304bc[1], 0, 0x1000, 0);
    *(int *)(c + 0x354) = 0;
    return 1;
}

// @symbol _ZN9daPeach_c17CallStateBehaviorEv
/* The second member of the selected pair. mStateFuncs points at a pair of
 * pointers-to-member copied out of the ten ROM constants at 0x0212ff34. */
void daPeach_c::CallStateBehavior()
{
    StateFunc *func = mStateFuncs + 1;
    (this->**func)();
}

// @symbol _ZN9daPeach_c13CallStateInitEv
void daPeach_c::CallStateInit()
{
    StateFunc *func = mStateFuncs;
    (this->**func)();
}

// @symbol _ZN9daPeach_c8SetStateEi
void daPeach_c::SetState(int state)
{
    mStateFuncs = data_ov085_0213055c + state * 2;
    CallStateInit();
}

// @symbol _ZN9daPeach_c16CleanupResourcesEv
/* One direct release, then a LOOP over a seven-entry table of pointers.
 * Unlike every sibling in this overlay, which writes its releases out one per
 * line, Peach's animation files are reached through data_ov085_0212f280 and
 * freed by index. Seven is the count the ROM's `blt` tests against. */
int daPeach_c::CleanupResources()
{
    ((SharedFilePtr *)(&data_ov085_021304f4))->Release();
    s32 i = 0;
    do {
        ((SharedFilePtr *)data_ov085_0212f280[i])->Release();
        i++;
    } while (i < 7);
    return 1;
}

// @symbol _ZN9daPeach_c16OnPendingDestroyEv
/* Empty -- the ROM body is a single `bx lr`. The override exists to suppress
 * whatever the base does on pending destroy, not to do anything itself. */
void daPeach_c::OnPendingDestroy()
{
}

// @symbol _ZN9daPeach_c6RenderEv
/* THE CALL IS QUALIFIED, AND THAT IS LOAD-BEARING. Model::Render is virtual
 * (slot 5 of _ZTV5Model) and ModelAnim overrides it, so a plain
 * `mModelAnim.Render(0)` emits the vtable dispatch -- three words where the
 * ROM has one `bl`. Naming the base explicitly suppresses the dispatch. */
int daPeach_c::Render()
{
    mModelAnim.Model::Render(0);
    return 1;
}

// @symbol _ZN9daPeach_c8BehaviorEv
int daPeach_c::Behavior()
{
    CallStateBehavior();
    UpdateLookAt();
    if (mStateValue != 1)
        ((Animation *)((Animation *)&mModelAnim))->Advance();
    ((PeachAnimSlots *)((char *)&mModelAnim))->g3();
    ((dCc_c *)&mCylinder)->Clear();
    ((dCc_c *)&mCylinder)->Update();
    UpdateModel();
    return 1;
}

// @symbol _ZN9daPeach_c13InitResourcesEv
/* Declared by final name, not as members: both Init calls take Fix12<int>
 * where these calls pass int literals, and Fix12<int> is an aggregate with no
 * converting constructor from int. dBgCh_Actr::Init's last parameter is a
 * Vector3_16* as well -- the S5_ in ...P10Vector3_16S5_ back-references the
 * pointer type, it is not an int. */
int daPeach_c::InitResources()
{
    char *s = (char *)((dActor_c *)this);
    void *f = Model::LoadFile(*(SharedFilePtr *)&data_ov085_021304f4);
    ((ModelBase *)(s + 0xd4))->SetFile((BMD_File *)f, 1, -1);
    for (int i = 0; i < 7; i++)
        Animation::LoadFile(*(SharedFilePtr *)data_ov085_0212f280[i]);
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        s + 0x160, ((dActor_c *)this), 0x90000, 0xc0000, 0x4800004, 0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        s + 0x194, ((dActor_c *)this), 0x40000, 0x40000, (Vector3_16 *)0, (Vector3_16 *)0);
    *(int *)(s + 0x9c) = -0x2000;
    *(int *)(s + 0xa0) = -0x3c000;
    *(int *)(s + 0x80) = 0x1000;
    *(int *)(s + 0x84) = 0x1000;
    *(int *)(s + 0x88) = 0x1000;
    SetState(0);
    UpdateModel();
    return 1;
}

// @symbol daPeach_c_classInit
/* The registry factory behind the PEACH_PRINCESS profile. It allocates 0x36c
 * -- this class's own sizeof -- and installs this class's vtable, the second
 * of the two independent witnesses the header's layout is read from.
 *
 * Reconstructed source-style name: SM64DS proves daPeach_c through RTTI,
 * allocation size, vtable identity and the PEACH_PRINCESS registry profile;
 * later EAD lineage supplies classInit. The exact original spelling is not
 * preserved. Historical alias: PrincessPeach_Spawn.
 *
 * `return new daPeach_c()` is the whole body: the synthesized constructor is
 * what runs dActor_c's base step, stores the vptr and then constructs the
 * ModelAnim at 0xd4, the ShadowModel at 0x138, the dCcAc_c at 0x160 and the
 * dBgCh_Actr at 0x194, in that order, and `operator new` is fBase_c's. */
extern "C" daPeach_c *daPeach_c_classInit(void)
{
    return new daPeach_c();
}
