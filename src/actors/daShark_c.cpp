//cpp
/* daShark_c -- the lake shark. ov090 0x0213367c..0x02133ca0, twelve functions.
 *
 * The cartridge spells the class 9daShark_c at 0x021343b0. _ZTI9daShark_c at
 * 0x021343bc is the __si_class_type_info record (base _ZTI12dEnemyBase_c),
 * and _ZTV9daShark_c at 0x021343ec is the vtable; the word before it, at
 * 0x021343e8, relocates to _ZTI9daShark_c. The header used to coin the
 * class Shark (factory alias Shark_Spawn); that rename landed earlier and
 * nothing here renames it.
 *
 * The run is gap-free and complete: D1, D0, five file-local helpers, then
 * CleanupResources, OnPendingDestroy, Render, Behavior and InitResources.
 * daShark_c_classInit at 0x02133ca0 abuts the end and stays in src/d_a_shark.c.
 * daMenbo_c, daManta_c and daPukupuku_c are the neighbours and are not part
 * of this compiler input. tu_map also hangs the label daPukupuku_c on this
 * run because its vtable walk reads the two state-table words at 0x021343a0
 * and 0x021343a8 (func_ov090_0213387c and func_ov090_02133830) as extra
 * slots. The pukupuku table ends at slot 30, 0x0213439c; those words are
 * copied by __sinit_ov090_02134020 into data_ov090_021345cc, which
 * InitResources installs as this class's state. They are not pukupuku methods.
 *
 * #pragma defer_codegen off emits .text in source order, so this file is
 * ROM-ascending. One out-of-line destructor is the key function: it emits
 * D1 (0x0213367c) then D0 (0x021336bc) and anchors _ZTV9daShark_c and
 * _ZTI9daShark_c in this object. ov090 has no D2 for this class.
 * The factory is not instantiated here.
 */

#pragma defer_codegen off

/* decl_common.h includes common.h, so the flat Matrix4x3 { s32 m[12]; } is
 * seen before daShark_c.h pulls math/Matrix.h through ModelAnim.h. The
 * guard is first-include-wins; func_ov090_02133904 copies that matrix as
 * twelve words. */
#include "decl_common.h"
#include "daShark_c.h"
#include "PathPtr.h"
#include "SharedFilePtr.h"
#include "decl_PathPtr.h"

/* Render calls ModelAnim's slot 5 through a view parked at 0x30c. The
 * parameter is an int in the matched body, not Vector3 const *. */
struct SharkRenderBase {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int);
};
struct SharkRenderView {
    char pad[0x30c];
    SharkRenderBase base;
};
typedef char SharkRenderView_size_must_be_0x310[
    sizeof(SharkRenderView) == 0x310 ? 1 : -1];

/* Behavior's state node: eight bytes, then the pointer-to-member it calls. */
struct SharkCallbackOwner;
typedef void (SharkCallbackOwner::*SharkCallback)();
struct SharkBehaviorState {
    char pad_00[8];
    SharkCallback callback;
};

/* func_ov090_021338b4 writes mState (0x370) and calls the member it finds
 * there. The pointer-to-member spelling is what the body matched under. */
struct SharkStateHolder;
typedef int (SharkStateHolder::*SharkStatePMF)();
struct SharkStateHolder {
    char pad[0x370];
    SharkStatePMF *pp;
};
typedef char SharkStateHolder_size_must_be_0x374[
    sizeof(SharkStateHolder) == 0x374 ? 1 : -1];

/* Still mangled externs: their receivers are raw offsets this file has not
 * typed (the ModelAnim at 0x30c, the dCcAcPos_c at 0x110 in the char *
 * helpers) or the raw PathPtr storage InitResources needs, so a member call
 * has no typed object to go through. Typing those receivers is remaining
 * work. */
extern "C" {
extern int func_02012694(int, void *);
extern void _ZN9Animation7AdvanceEv(void *);
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    char *anim, void *file, int a, int b, unsigned int u);
extern char data_ov090_021345ac[];
extern void Vec3_Asr(void *dst, void *src, int n);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(
    void *m, short rx, short ry, short rz);
extern char data_020a0e68[];
extern u16 DecIfAbove0_Short(u16 *value);
extern void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
extern s32 LenVec3(Vector3 *value);
extern s16 Vec3_HorzAngle(Vector3 *a, Vector3 *b);
extern s16 Vec3_VertAngle(Vector3 *a, Vector3 *b);
extern void Matrix4x3_FromRotationY(void *matrix, s32 angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *matrix, s16 angle);
extern void MulVec3Mat4x3(void *a, void *matrix, void *b);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(
    void *thiz, void *file, int a, int b);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void *f);
extern void _ZN7PathPtrC1Ev(void *thiz);
extern void _ZN7PathPtr6FromIDEj(void *thiz, unsigned int id);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void *thiz, void *actor, void *pos, int f, int g,
    unsigned int h, unsigned int i);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(
    void *thiz, void *out, unsigned int j);
extern void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void *, void *);
extern void *_ZN8dActor_c10FindWithIDEj(unsigned int);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    void *, void *, unsigned int, int, unsigned int, unsigned int, unsigned int);

void func_ov090_02133904(char *c);
void func_ov090_02133710(char *c);
}

void ApproachLinear(s16 &value, s16 target, s16 step);

/* One written destructor. The compiler emits D1 (0x0213367c) and D0
 * (0x021336bc). Writing it out of line anchors _ZTV9daShark_c. */
// @symbol _ZN9daShark_cD1Ev
// @symbol _ZN9daShark_cD0Ev
daShark_c::~daShark_c()
{
}

// @symbol func_ov090_02133710
extern "C" void func_ov090_02133710(char *c)
{
    struct Vector3 v;
    struct Vector3 v2;
    void *a;
    *(int *)(((int)c + 0x384)) += 1;
    if (*(int *)(c + 0x384) > 2)
        *(int *)(c + 0x384) = 0;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    if (*(int *)(c + 0x384) == 1) {
        v.z = 0x8c000;
    } else if (*(int *)(c + 0x384) == 2) {
        v.z = -0x8c000;
    }
    Matrix4x3_FromRotationY(data_020a0e68, 0);
    MulVec3Mat4x3(&v, data_020a0e68, c + 0x374);
    _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(c + 0x110, c + 0x374);
    if (*(unsigned int *)(c + 0x134) == 0)
        return;
    a = _ZN8dActor_c10FindWithIDEj(*(unsigned int *)(c + 0x134));
    {
        int b = (*(unsigned short *)((char *)a + 0xc) == 0xbf);
        if (b == 0)
            return;
    }
    if (*(unsigned char *)((char *)a + 0x6fb))
        return;
    v2.x = *(int *)(c + 0x5c);
    v2.y = *(int *)(c + 0x60);
    v2.z = *(int *)(c + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &v2, 3, 0xc000, 1, 0, 1);
}

// @symbol func_ov090_02133830
extern "C" int func_ov090_02133830(char *c)
{
    int val = *(int *)(c + 0x364);
    val = (val >> 12) << 16;
    val = (unsigned int)val >> 16;
    if (val == 0) {
        func_02012694(9, c + 0x74);
    }
    *(int *)(c + 0x368) = 0x1000;
    _ZN9Animation7AdvanceEv(c + 0x35c);
    func_ov090_02133710(c);
    return 1;
}

// @symbol func_ov090_0213387c
extern "C" int func_ov090_0213387c(char *c)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        c + 0x30c, *(void **)((char *)data_ov090_021345ac + 4), 0, 0x1000, 0);
    return 1;
}

// @symbol func_ov090_021338b4
extern "C" int func_ov090_021338b4(void *self, void *state)
{
    SharkStateHolder *c = (SharkStateHolder *)self;
    SharkStatePMF *p = (SharkStatePMF *)state;
    c->pp = p;
    SharkStatePMF *q = c->pp;
    if (*q == 0)
        return 1;
    return (c->**q)();
}

// @symbol func_ov090_02133904
extern "C" void func_ov090_02133904(char *c)
{
    int v[3];
    Vec3_Asr(v, c + 0x5c, 3);
    Matrix4x3_FromTranslation(data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(
        data_020a0e68, *(short *)(c + 0x8c), *(short *)(c + 0x8e), *(short *)(c + 0x90));
    *(struct Matrix4x3 *)(c + 0x328) = *(struct Matrix4x3 *)data_020a0e68;
}

// @symbol _ZN9daShark_c16CleanupResourcesEv
int daShark_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov090_021345a4)->Release();
    ((SharedFilePtr *)data_ov090_021345ac)->Release();
    return 1;
}

// @symbol _ZN9daShark_c16OnPendingDestroyEv
void daShark_c::OnPendingDestroy()
{
}

// @symbol _ZN9daShark_c6RenderEv
int daShark_c::Render()
{
    SharkRenderBase *b = &((SharkRenderView *)this)->base;
    b->m(0);
    return 1;
}

// @symbol _ZN9daShark_c8BehaviorEv
int daShark_c::Behavior()
{
    DecIfAbove0_Short((u16 *)&mStateTimer);
    {
        SharkBehaviorState *state = *(SharkBehaviorState **)&mState;
        if (state->callback != 0) {
            (((SharkCallbackOwner *)this)->*(state->callback))();
        }
    }
    {
        PathPtr path;
        Vector3 node;
        Vector3 difference;
        Vector3 velocity;
        int distance;

        path.FromID(mPathID);
        path.GetNode(node, mPathNodeIdx);
        Vec3_Sub(&difference, (Vector3 *)&mPosX, &node);
        distance = LenVec3(&difference);
        if (distance == 0 || distance <= 0x258000) {
            mPathNodeIdx++;
            if (mPathNodeIdx >= mPathNodeCount)
                mPathNodeIdx = 0;
        }
        ApproachLinear(mPrevAngleY,
            Vec3_HorzAngle((Vector3 *)&mPosX, &node), 0x180);
        ApproachLinear(mPrevAngleX,
            Vec3_VertAngle((Vector3 *)&mPosX, &node), 0x40);
        mAngleX = mPrevAngleX;
        mAngleY = mPrevAngleY;
        mAngleZ = mPrevAngleZ;
        velocity.y = velocity.x = velocity.z = 0;
        velocity.z = 0x14000;
        Matrix4x3_FromRotationY(data_020a0e68, mPrevAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, mPrevAngleX);
        MulVec3Mat4x3(&velocity, data_020a0e68, (Vector3 *)&unk_0a4);
    }
    {
        int speed = mVertSpeed + mVertAccel;
        int terminalVelocity = mTerminalVelocity;
        int unk = unk_0ac;
        if (speed >= terminalVelocity)
            terminalVelocity = speed;
        mVertSpeed = terminalVelocity;
        unk_0ac = unk;
    }
    UpdatePosWithOnlySpeed(&mdCcAcPos_c);
    func_ov090_02133904((char *)this);
    mdCcAcPos_c.Clear();
    {
        Player *player = ClosestPlayer();
        if (player != 0 && *((u8 *)player + 0x6fb) == 0)
            mdCcAcPos_c.Update();
    }
    return 1;
}

// @symbol _ZN9daShark_c13InitResourcesEv
int daShark_c::InitResources()
{
    /* Raw storage, not PathPtr locals. PathPtr's constructor is declared,
     * so a typed local would construct at the declaration and again at the
     * explicit call. The ROM constructs p1 before the collision init and
     * p2 after it. */
    u32 p1_storage[sizeof(PathPtr) / sizeof(u32)];
    u32 p2_storage[sizeof(PathPtr) / sizeof(u32)];
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x30c,
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov090_021345a4), 1, -1);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov090_021345ac);
    mPathID = (*(s32 *)&param1) & 0xff;
    if (mPathID < 0)
        mPathID = 0;
    _ZN7PathPtrC1Ev(p1_storage);
    _ZN7PathPtr6FromIDEj(p1_storage, mPathID);
    mPathNodeCount = _ZNK7PathPtr8NumNodesEv(p1_storage);
    mTerminalVelocity = -0x3c000;
    mClsnOffset.x = 0;
    mClsnOffset.y = 0;
    mClsnOffset.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        ((char *)this) + 0x110, ((char *)this), &mClsnOffset,
        0x42000, 0x6e000, 0x200004, 0);
    _ZN7PathPtrC1Ev(p2_storage);
    _ZN7PathPtr6FromIDEj(p2_storage, mPathID);
    mPathNodeIdx = 1;
    _ZNK7PathPtr7GetNodeER7Vector3j(p2_storage, ((char *)this) + 0x5c, mPathNodeIdx);
    func_ov090_021338b4(((char *)this), data_ov090_021345cc);
    return 1;
}
