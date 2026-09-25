//cpp
/* daManta_c -- ov090 0x0213269c..0x02132fe8, twelve functions.
 *
 * The cartridge spells the class 9daManta_c at 0x021341f4. _ZTI9daManta_c
 * at 0x0213420c is the __si_class_type_info record [__si_class_type_info,
 * that string, _ZTI12dEnemyBase_c], and _ZTV9daManta_c at 0x0213423c is the
 * vtable; the word before it, at 0x02134238, relocates to _ZTI9daManta_c.
 * The name is the cartridge's own; the tree's historical factory alias was
 * MantaRay_Spawn.
 *
 * One out-of-line destructor is the key function, so this object emits
 * _ZTV9daManta_c, _ZTI9daManta_c and _ZTS9daManta_c. It comes out D1
 * (0x0213269c) then D0 (0x021326dc); the D2 the compiler also emits has no
 * home on the cartridge (manifest: compiler_only_output). daManta_c_classInit
 * at 0x02132fe8 abuts this run and stays in src/d_a_manta.c.
 *
 * tu_map also hangs daMenbo_c on this run. daMenbo_c's table is 31 slots
 * and ends at 0x021341e4; the two words there and at 0x021341ec relocate
 * to func_ov090_021327e4 and func_ov090_02132a58. Those are this file's
 * helpers, not menbo methods. No daMenbo_c function is in the span.
 *
 * #pragma defer_codegen off emits .text in source order, so this file is
 * ROM-ascending. decl_common.h is included first so Matrix4x3 stays the
 * flat m[12] spelling: func_ov090_02132b14 indexes .m, not .t.
 */

#pragma defer_codegen off

#include "decl_common.h"
#include "daManta_c.h"
#include "PathPtr.h"
#include "SharedFilePtr.h"

/* Render calls ModelAnim slot 5 through a view parked at 0x30c. The
 * parameter is an int in the matched body, not Vector3 const *. */
struct MantaRenderBase {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int);
};
struct MantaRenderView {
    char pad[0x30c];
    MantaRenderBase base;
};

/* Behavior's state node: eight bytes, then the pointer-to-member it calls. */
struct MantaCallbackOwner;
typedef void (MantaCallbackOwner::*MantaCallback)();
struct MantaBehaviorState {
    char pad_00[8];
    MantaCallback callback;
};

/* func_ov090_02132ac4 writes the state at 0x370 and calls it. */
struct MantaStateHolder;
typedef int (MantaStateHolder::*MantaStatePMF)();
struct MantaStateHolder {
    char pad[0x370];
    MantaStatePMF *pp;
};

extern "C" {
extern SharedFilePtr data_ov090_02134524;
extern SharedFilePtr data_ov002_0210da10;
extern SharedFilePtr data_ov002_0210d9a8;
extern SharedFilePtr data_ov090_0213452c;
extern unsigned char data_0209f2d8;
extern Matrix4x3 data_020a0e68;

void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(
    dCcAcPos_c *, const Vector3 &);
Player *_ZN8dActor_c10FindWithIDEj(unsigned int id);
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    Player *, const Vector3 &, unsigned int, Fix12i, unsigned int,
    unsigned int, unsigned int);
void _ZN9Animation7AdvanceEv(void *a);
void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
    u32 id, u32 a, const Vector3 &v, const Vector3_16 *r, int b, int c);
void func_02012790(u32 a);
void *_ZN8dActor_c11SpawnNumberERK7Vector3jbtPS_(
    void *self, const Vector3 &v, u32 n, int b, u16 t, void *p);
void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
void Matrix4x3_FromTranslation(Matrix4x3 *m, Fix12i x, Fix12i y, Fix12i z);
void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
void MulMat4x3Mat4x3(const int *a, const int *b, int *dst);
unsigned short DecIfAbove0_Short(unsigned short *p);
void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
int LenVec3(Vector3 *v);
short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
short Vec3_VertAngle(const Vector3 *v0, const Vector3 *v1);
void Matrix4x3_FromRotationY(void *m, int angle);
void Matrix4x3_ApplyInPlaceToRotationX(void *m, short angle);
void MulVec3Mat4x3(Vector3 *v, void *m, Vector3 *out);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void *self, dActor_c *a, const Vector3 &v, int b, int c,
    unsigned int d, unsigned int e);
int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    void *anim, void *file, int a, int b, unsigned int u);

void func_ov090_02132730(char *thiz);
int func_ov090_021327e4(char *c);
int func_ov090_02132a58(char *c);
int func_ov090_02132ac4(MantaStateHolder *c, MantaStatePMF *p);
void func_ov090_02132b14(char *self);
}

int ApproachLinear(short &v, short target, short step);

/* One written destructor. The compiler emits D1 then D0.
 *
 * D1 is one vtable store and four destructor calls, every one a consequence
 * of `struct daManta_c : dEnemyBase_c` and the members it types: the
 * vptr, then ModelAnim (0x30c), dBgCh_Actr (0x150) and dCcAcPos_c (0x110) in
 * reverse declaration order, then dEnemyBase_c::~dEnemyBase_c. That body is
 * the evidence for the header's member layout; daManta_c_classInit constructs
 * the same types at the same offsets. D0 adds dEnemyBase_c's inline operator
 * delete, which is why nothing here names a heap. */
// @symbol _ZN9daManta_cD1Ev
// @symbol _ZN9daManta_cD0Ev
daManta_c::~daManta_c()
{
}

// @symbol func_ov090_02132730
extern "C" void func_ov090_02132730(char *thiz)
{
    char *c = thiz;
    Vector3 v;
    v.x = data_ov090_02134200.x;
    v.y = data_ov090_02134200.y;
    v.z = data_ov090_02134200.z;
    _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(
        (dCcAcPos_c *)(c + 0x110), v);
    {
        unsigned int id = *(unsigned int *)(c + 0x134);
        if (id == 0) return;
        {
        Player *a = _ZN8dActor_c10FindWithIDEj(id);
        int b = (int)(*(unsigned short *)((char *)a + 0xc) == 0xbf);
        if (b == 0) return;
        {
            Vector3 hv;
            hv.x = *(int *)(c + 0x5c);
            hv.y = *(int *)(c + 0x60);
            hv.z = *(int *)(c + 0x64);
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, hv, 1, 0xc000, 1, 0, 1);
        }
        }
    }
}

// @symbol func_ov090_021327e4
extern "C" int func_ov090_021327e4(char *c)
{
    void *o;
    Vector3 num1;
    Vector3 num2;

    _ZN9Animation7AdvanceEv(c + 0x35c);
    func_ov090_02132730(c);

    if (*(int *)(c + 0x378) >= 5)
        goto Ldecay;

    if (*(u16 *)(c + 0x100) == 0) {
        o = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                0xf4, 1, *(Vector3 *)(c + 0x39c), (Vector3_16 *)(c + 0x8c),
                *(signed char *)(c + 0xcc), -1);
        if (o != 0) {
            int idx = *(int *)(c + 0x3fc);
            int *pf = (int *)(c + 0x3fc);
            *(int *)(c + idx * 4 + 0x3ac) = *(int *)((char *)o + 4);
            *pf += 1;
            if (*(int *)(c + 0x3fc) >= 0x14)
                *(int *)(c + 0x3fc) = 0;
            *(int *)((char *)o + 0x38c) = (int)c;
        }
        *(u16 *)(c + 0x100) = 0x27;
    }

    if (*(int **)(c + 0x3a8) == 0)
        goto Ldecay;

    if (*(int *)(c + 0x378) == 0) {
        int i;
        int key = (*(int **)(c + 0x3a8))[1];
        for (i = 0; i < 0x14; i++) {
            int slot = ((int *)(c + 0x3ac))[i];
            if ((0, slot) == key) {
                *(int *)(c + 0x400) = i;
                *(int *)(c + 0x378) += 1;
                func_02012790(0x25);
                num1 = *(Vector3 *)((char *)*(void **)(c + 0x3a8) + 0x5c);
                _ZN8dActor_c11SpawnNumberERK7Vector3jbtPS_(c, num1, *(int *)(c + 0x378), 0, 0, 0);
                *(int *)(c + 0x3a8) = 0;
                return 1;
            }
        }
        goto Lreset;
    } else {
        *(int *)(c + 0x400) += 1;
        if (*(int *)(c + 0x400) >= 0x14)
            *(int *)(c + 0x400) = 0;
        {
            int r0 = *(int *)(c + 0x400);
            int r1 = (int)*(int **)(c + 0x3a8);
            r0 = (int)(c + (r0 << 2));
            r1 = *(int *)(r1 + 4);
            r0 = *(int *)(r0 + 0x3ac);
            if (r0 != r1)
                goto Lreset;
        }
        *(int *)(c + 0x378) += 1;
        func_02012790(0x25);
        num2 = *(Vector3 *)((char *)*(void **)(c + 0x3a8) + 0x5c);
        _ZN8dActor_c11SpawnNumberERK7Vector3jbtPS_(c, num2, *(int *)(c + 0x378), 0, 0, 0);
        *(int *)(c + 0x3a8) = 0;
        return 1;
    }

Lreset:
    *(int *)(c + 0x378) = 0;
    *(int *)(c + 0x400) = 0;
    *(int *)(c + 0x3a8) = 0;
Ldecay:
    if (*(int *)(c + 0x378) == 5) {
        *(int *)(c + 0x38c) += 1;
        if (*(int *)(c + 0x38c) > 0x1e) {
            _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                0xb2, *(int *)(c + 0x388) | 0x40, *(Vector3 *)(c + 0x5c),
                (Vector3_16 *)(c + 0x8c), *(signed char *)(c + 0xcc), -1);
            *(int *)(c + 0x378) = 0xa;
        }
    }
    return 1;
}

// @symbol func_ov090_02132a58
extern "C" int func_ov090_02132a58(char *c)
{
    void **bundle = (void **)&data_ov090_0213452c;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        (void *)(c + 0x30c), bundle[1], 0, 0x1000, 0);
    *(int *)(c + 0x3a8) = 0;
    *(int *)(c + 0x3fc) = 0;
    *(int *)(c + 0x400) = 0;
    *(int *)(c + 0x368) = 0x1000;
    int i;
    for (i = 0; i < 0x14; i++) ((int *)(c + 0x3ac))[i] = 0;
    return 1;
}

// @symbol func_ov090_02132ac4
extern "C" int func_ov090_02132ac4(MantaStateHolder *c, MantaStatePMF *p)
{
    c->pp = p;
    MantaStatePMF *q = c->pp;
    if (*q == 0)
        return 1;
    return (c->**q)();
}

// @symbol func_ov090_02132b14
extern "C" void func_ov090_02132b14(char *self)
{
    struct Vector3 v;
    Vec3_Asr(&v, (struct Vector3 *)(self + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(s16 *)(self + 0x8c), *(s16 *)(self + 0x8e), *(s16 *)(self + 0x90));
    *(struct Matrix4x3 *)(self + 0x328) = data_020a0e68;
    *(int *)(self + 0x39c) = 0;
    *(int *)(self + 0x3a0) = 0;
    *(int *)(self + 0x3a4) = 0;
    data_020a0e68 = *(struct Matrix4x3 *)(self + 0x328);
    MulMat4x3Mat4x3((const int *)(*(char **)(self + 0x320) + 0x90),
        data_020a0e68.m, data_020a0e68.m);
    {
        int *p9 = (int *)((unsigned long long)((int)(self) + 0x39c));
        int *p10 = (int *)((unsigned long long)((int)(self) + 0x3a0));
        int *p11 = (int *)((unsigned long long)((int)(self) + 0x3a4));
        *(int *)(self + 0x39c) = data_020a0e68.m[9];
        *(int *)(self + 0x3a0) = data_020a0e68.m[10];
        *(int *)(self + 0x3a4) = data_020a0e68.m[11];
        *p9 <<= 3;
        *p10 <<= 3;
        *p11 <<= 3;
    }
}

// @symbol _ZN9daManta_c16CleanupResourcesEv
int daManta_c::CleanupResources()
{
    data_ov090_02134524.Release();
    data_ov002_0210da10.Release();
    data_ov002_0210d9a8.Release();
    data_ov090_0213452c.Release();
    return 1;
}

// @symbol _ZN9daManta_c16OnPendingDestroyEv
void daManta_c::OnPendingDestroy()
{
}

// @symbol _ZN9daManta_c6RenderEv
int daManta_c::Render()
{
    MantaRenderBase *b = &((MantaRenderView *)this)->base;
    b->m(0);
    return 1;
}

// @symbol _ZN9daManta_c8BehaviorEv
int daManta_c::Behavior()
{
    char *c = (char *)this;
    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    {
        MantaBehaviorState *o = *(MantaBehaviorState **)&unk_370;
        if (*(int *)((char *)o + 8) != 0) {
            (((MantaCallbackOwner *)c)->*(o->callback))();
        }
    }
    {
        PathPtr p;
        Vector3 node;
        Vector3 diff;
        Vector3 v;
        int len;

        p.FromID(*(unsigned int *)&unk_37c);
        p.GetNode(node, *(unsigned int *)&mPathNode);
        Vec3_Sub(&diff, (Vector3 *)&mPosX, &node);
        len = LenVec3(&diff);
        if (len == 0 || len <= 0x258000) {
            mPathNode++;
            if (mPathNode >= unk_380)
                mPathNode = 0;
        }
        ApproachLinear(mPrevAngleY, Vec3_HorzAngle((Vector3 *)&mPosX, &node), 0x60);
        ApproachLinear(mPrevAngleX, Vec3_VertAngle((Vector3 *)&mPosX, &node), 0x40);
        mAngleX = mPrevAngleX;
        mAngleY = mPrevAngleY;
        mAngleZ = mPrevAngleZ;
        v.y = v.x = v.z = 0;
        v.z = 0xa000;
        Matrix4x3_FromRotationY(&data_020a0e68, mPrevAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mPrevAngleX);
        MulVec3Mat4x3(&v, &data_020a0e68, (Vector3 *)&unk_0a4);
    }
    {
        int s = mVertSpeed + mVertAccel;
        int m2 = mTerminalVelocity;
        int ac = unk_0ac;
        if (s >= m2) m2 = s;
        mVertSpeed = m2;
        unk_0ac = ac;
    }
    UpdatePosWithOnlySpeed(&mdCcAcPos_c);
    func_ov090_02132b14(c);
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    return 1;
}

// @symbol _ZN9daManta_c13InitResourcesEv
int daManta_c::InitResources()
{
    unsigned char *thiz = (unsigned char *)this;
    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov090_02134524), 1, -1);
    Model::LoadFile(data_ov002_0210da10);
    Model::LoadFile(data_ov002_0210d9a8);
    Animation::LoadFile(data_ov090_0213452c);

    unk_37c = param1 & 0xff;
    unk_388 = (*(unsigned int *)&param1 >> 0xc) & 0xf;
    if (unk_37c < 0) unk_37c = 0;

    {
        PathPtr pp;
        pp.FromID(*(unsigned int *)&unk_37c);
        unk_380 = pp.NumNodes();
    }

    mTerminalVelocity = -0x3c000;

    {
        Vector3 v;
        v.x = data_ov090_02134200.x;
        v.y = data_ov090_02134200.y;
        v.z = data_ov090_02134200.z;
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, (dActor_c *)thiz, v,
            0x150000, 0xc8000, 0x200004, 0);
    }

    {
        PathPtr pp;
        pp.FromID(*(unsigned int *)&unk_37c);
        mPathNode = 1;
        pp.GetNode(*(Vector3 *)&mPosX, *(unsigned int *)&mPathNode);
    }

    {
        int b = (int)(data_0209f2d8 == 2);
        if (b != 0) {
            mPathNode = 3;
            mPrevAngleX = (short)0xf303;
            mPrevAngleY = 0xb50;
            mPrevAngleZ = 0;
            mPosX = (int)0xfdfb8000;
            mPosY = (int)0xff8f8000;
            mPosZ = 0x29a000;
            mFlags = 0;
        }
    }

    func_ov090_02132ac4((MantaStateHolder *)thiz, (MantaStatePMF *)&data_ov090_0213454c);
    return 1;
}
