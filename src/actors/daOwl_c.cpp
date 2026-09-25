//cpp
/* Production translation unit for ov094/daOwl_c.
 * 21 function(s), .text 0x02135700..0x02136798. The OWL actor.
 *
 * NAME: _ZTS7daOwl_c is "7daOwl_c" at ov094 0x02136a10; _ZTI at 0x02136a28
 * reads [__si_class_type_info, that string, _ZTI12dEnemyBase_c]. The vtable's
 * address point is 0x02136a58, and slots 16/17 hold D1/D0 below. The tree
 * previously called the class HootTheOwl (coined; vtable address only).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x02135700), D0
 * (0x02135748), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is ROM-ascending.
 * The next function, daOwl_c_classInit at 0x02136798, is a separate TU
 * (src/d_a_owl.c) and stays out.
 */

#pragma defer_codegen off

#include "decl_common.h"
#include "daOwl_c.h"
#include "SharedFilePtr.h"

/* decl_common.h includes common.h, so Matrix4x3 is the flat s32 m[12]
 * spelling. math/Matrix.h (via daOwl_c.h -> ModelAnim.h) stands down. */
typedef char Matrix4x3_size_must_be_0x30[sizeof(Matrix4x3) == 0x30 ? 1 : -1];

struct OwlVec {
    int x, y, z;
};
typedef char OwlVec_size_must_be_0x0c[sizeof(OwlVec) == 0x0c ? 1 : -1];

struct V6 {
    int v[6];
};
typedef char V6_size_must_be_0x18[sizeof(V6) == 0x18 ? 1 : -1];

struct C;
typedef int (C::*PMF)();
struct C {
    char pad[0x3c8];
    PMF *pp;
};

/* Local Player, not include/Player.h. GetTalkState is a direct call; the
 * real class is only forward-declared by the actor headers. */
struct Player {
    int GetTalkState();
};

extern "C" {
extern char data_ov094_02136b40[];
extern char data_ov094_02136b60[];
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;
extern short data_02082214[];
extern int data_0209e650[];
extern Matrix4x3 data_020a0e68;

int func_ov094_02136188(C *c, PMF *p);
void func_ov094_021357a4(char *c);

/* Leftover: ModelAnim::SetAnim, dCcAcPos_c::Init, dBgCh_Actr::Init and
 * dActor_c::DropShadowRadHeight take Fix12<int> by value, so they stay mangled;
 * the other mangled externs have no member declaration in a shared header yet.
 * RandomIntInternal stays unsigned so func_ov094_02135e64 keeps its logical shift. */
void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void *self, void *v);
int _ZN8dActor_c10FindWithIDEj(unsigned id);
int func_ov002_020df840(void *a, void *b, void *d);
void _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(Vector3 *a, const Vector3 *b, Fix12i f);
int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a, int fix, unsigned int b);
int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int cc, void *pos, unsigned int d);
int func_ov002_020df7f4(void *c);
int _ZNK10dBgCh_Actr8IsOnWallEv(void *self);
int func_ov002_020df7ac(void *thiz);
void _ZN10dBgCh_Actr12Unk_0203589cEv(void *self);
int func_02012694(int a, void *pos);
int ApproachAngle(short *p, int target, int a, int b, int c);
void *_ZN8dActor_c13ClosestPlayerEv(void *thiz);
int _ZN6Player9StartTalkER7fBase_cb(void *thiz, void *ab, int b);
int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void *thiz, void *ab, unsigned int a, const void *v, unsigned int c, unsigned int d);
int Vec3_Dist(const void *a, const void *b);
short Vec3_HorzAngle(const void *a, const void *b);
short Vec3_VertAngle(const void *a, const void *b);
unsigned int RandomIntInternal(void *seed);
void _Z14ApproachLinearRiii(int *x, int target, int step);
void Matrix4x3_FromRotationY(void *m, int angle);
void MulVec3Mat4x3(void *dst, void *mat, void *src);
void Vec3_Sub(OwlVec *out, OwlVec *a, OwlVec *b);
int LenVec3(OwlVec *v);
void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
void Matrix4x3_FromTranslation(Matrix4x3 *m, Fix12i x, Fix12i y, Fix12i z);
void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *m, Fix12i x, Fix12i y, Fix12i z);
void _ZN9ModelBase12ApplyOpacityEjj(void *self, unsigned int opacity, unsigned int unused);
void MulMat4x3Mat4x3(const int *a, const int *b, int *dst);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *sm, Matrix4x3 *m, Fix12i fx, int t, unsigned int u);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void *, void *, int *, int, int, unsigned int, unsigned int);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *, void *, int, int, void *, int);
int IsStarCollectedInCurLevel(int);
void DecIfAbove0_Short(void *);
}

// @symbol _ZN7daOwl_cD1Ev
// @symbol _ZN7daOwl_cD0Ev
/* The whole body is compiler-emitted: one vptr store, then ShadowModel
 * (0x370), ModelAnim (0x30c), dBgCh_Actr (0x150) and dCcAcPos_c (0x110) in
 * reverse declaration order, then dEnemyBase_c::~dEnemyBase_c. D1 is the
 * evidence for the header's four member types; D0 adds dEnemyBase_c's inline
 * operator delete. */
daOwl_c::~daOwl_c()
{
}

// @symbol func_ov094_021357a4
extern "C" void func_ov094_021357a4(char *c)
{
    int st[3];
    OwlVec tmp;
    tmp.x = data_ov094_02136a1c[0];
    tmp.y = data_ov094_02136a1c[1];
    tmp.z = data_ov094_02136a1c[2];
    _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(c + 0x110, &tmp);
    if (*(int *)(c + 0x134) == 0)
        return;
    int a = _ZN8dActor_c10FindWithIDEj(*(int *)(c + 0x134));
    if (a == 0)
        return;
    int t = *(unsigned short *)(a + 0xc);
    unsigned eq = (t == 0xbf);
    if (!eq)
        return;
    *(int *)(c + 0x3cc) = a;
    st[0] = 0;
    st[1] = 0;
    st[2] = 0;
    st[1] = 0x1838000;
    if (data_0209f2f8 == 0x16)
        st[1] = 0x1194000;
    *(int *)(c + 0xa4) = 0;
    *(int *)(c + 0xa8) = 0;
    *(int *)(c + 0xac) = 0;
    if (func_ov002_020df840((void *)*(int *)(c + 0x3cc), c, st) != 1) {
        *(int *)(c + 0x3cc) = 0;
        return;
    }
    func_ov094_02136188((C *)c, (PMF *)&data_ov094_02136b70);
}

// @symbol func_ov094_021358b4
extern "C" int func_ov094_021358b4(void *t)
{
    char *c = (char *)t;
    if (*(int *)(c + 0x3e8) == 0) {
        if (*(u8 *)(c + 0x3e4) != 0) {
            u8 *cd = (u8 *)(((int)c + 0x3e4));
            *cd = *cd - 1;
            *(short *)(c + 0x92) = 0;
            *(int *)(c + 0xa8) = 0x14000;
        } else {
            *(int *)(c + 0x3e8) = 1;
            *(int *)(c + 0xa4) = 0;
            *(int *)(c + 0xa8) = 0;
            *(int *)(c + 0xac) = 0;
            int *p = (int *)(((int)c + 0x60));
            *(int *)(c + 0x5c) = *(int *)(c + 0x3d8);
            *(int *)(c + 0x60) = *(int *)(c + 0x3dc);
            *(int *)(c + 0x64) = *(int *)(c + 0x3e0);
            *p += 0x64000;
        }
    } else {
        _Z14ApproachLinearR7Vector3RKS_5Fix12IiE((Vector3 *)(c + 0x5c), (Vector3 *)(c + 0x3d8), 0x2000);
        if (*(u8 *)(c + 0x3e4) < 0x1f) {
            u8 *cd = (u8 *)(((int)c + 0x3e4));
            *cd = *cd + 1;
        } else {
            *(u8 *)(c + 0x3e4) = 0x1f;
            func_ov094_02136188((C *)c, (PMF *)data_ov094_02136b60);
        }
    }
    return 1;
}

// @symbol func_ov094_0213598c
extern "C" int func_ov094_0213598c(char *c)
{
    *(int *)(c + 0x3e8) = 0;
    *(short *)(c + 0x100) = 0;
    *(int *)(c + 0x3f0) = 0x2000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x30c, (void *)data_ov094_02136af8[1], 0, 0x1000, 0);
    return 1;
}

// @symbol func_ov094_021359d8
extern "C" int func_ov094_021359d8(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    void *rider;

    if (*(unsigned short *)(c + 0x100) == 1) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x30c, (void *)data_ov094_02136ae8[1], 0, 0x1000, 0);
        *(int *)(c + 0x3f0) = 0x1000;
    }

    if (*(unsigned short *)(c + 0x100) == 0) {
        *(int *)(c + 0x3f4) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int *)(c + 0x3f4), 3, 0x18c, c + 0x74, 0);
    }

    if (*(unsigned short *)(c + 0x100) != 0) {
        if (*(int *)(c + 0x3e8) == 0
            && ((unsigned int)(*(int *)(c + 0x364) << 4) >> 16) <= 2) {
            func_02012694(0x139, c + 0x74);
            *(int *)(c + 0x3e8) = 1;
        } else {
            if (((unsigned int)(*(int *)(c + 0x364) << 4) >> 16) > 2)
                *(int *)(c + 0x3e8) = 0;
        }
    }

    rider = *(void **)(c + 0x3cc);
    if (rider != 0 && func_ov002_020df7f4(rider) == 1) {
        *(unsigned short *)(c + 0x100) = 0xa;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x30c, (void *)data_ov094_02136af8[1], 0, 0x1000, 0);
    }

    ApproachAngle((short *)(c + 0x92), 0, 0xa, 0x200, 0x100);

    if (_ZNK10dBgCh_Actr8IsOnWallEv(c + 0x150) != 0 || func_02035638(c + 0x150) != 0) {
        rider = *(void **)(c + 0x3cc);
        if (rider != 0 && func_ov002_020df7ac(rider) != 0) {
            _ZN10dBgCh_Actr12Unk_0203589cEv(c + 0x150);
            *(int *)(c + 0x3cc) = 0;
            *(int *)(c + 0x3e8) = 0;
            *(unsigned short *)(c + 0x100) = 0;
            func_ov094_02136188((C *)c, (PMF *)&data_ov094_02136b30);
            return 1;
        }
    }

    rider = *(void **)(c + 0x3cc);
    if (rider == 0) {
        goto cleanup;
    }
    if (rider == 0) {
        goto end;
    }
    if (func_ov002_020df7f4(rider) < 0) {
cleanup:
        *(int *)(c + 0x3cc) = 0;
        *(int *)(c + 0x3e8) = 0;
        *(unsigned short *)(c + 0x100) = 0;
        func_ov094_02136188((C *)c, (PMF *)&data_ov094_02136b30);
    }
end:
    return 1;
}

// @symbol func_ov094_02135bd4
extern "C" int func_ov094_02135bd4(void *c)
{
    *(short *)((char *)c + 0x100) = 0x1e;
    *(int *)((char *)c + 0x3e8) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)c + 0x30c, (void *)data_ov094_02136af8[1], 0, 0x1000, 0);
    *(int *)((char *)c + 0x3f0) = 0x2000;
    return 1;
}

// @symbol func_ov094_02135c28
extern "C" int func_ov094_02135c28(void *thiz)
{
    char *c = (char *)thiz;
    int result;
    int ang;
    int *p3e8;
    short tbl;
    int idx;
    V6 buf;

    if (*(u8 *)(c + 0x3d4) == 0) {
        if (_ZN8dActor_c13ClosestPlayerEv(c) != 0) {
            void *player = *(void **)(c + 0x3d0);
            if (player != 0) {
                if ((u16)(*(u16 *)((char *)player + 0x6ce) & 0x800) == 0) {
                    if (_ZN6Player9StartTalkER7fBase_cb(player, c, 1) != 0) {
                        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(void **)(c + 0x3d0), c, 0xa2, (const void *)(c + 0x5c), 0, 0) == 1) {
                            func_02012694(0x176, (void *)(c + 0x74));
                            *(u8 *)(c + 0x3d4) = 1;
                            func_ov094_02136188((C *)c, (PMF *)data_ov094_02136b50);
                            return 1;
                        }
                    }
                }
            }
        }
    }

    if (Vec3_Dist((const void *)(c + 0x5c), (const void *)(c + 0x3d8)) > 0x190000) {
        *(s16 *)(c + 0x100) = 0x32;
        *(s16 *)(c + 0x3ec) = Vec3_HorzAngle((const void *)(c + 0x5c), (const void *)(c + 0x3d8));
    } else if (*(u16 *)(c + 0x100) == 0) {
        *(s16 *)(c + 0x3ec) = (s16)(((u32)RandomIntInternal(data_0209e650) >> 8) << 12);
        *(s16 *)(c + 0x100) = (s16)((((u32)RandomIntInternal(data_0209e650) >> 8) & 0x3f) + 0x32);
    }

    ApproachAngle((short *)(c + 0x94), *(s16 *)(c + 0x3ec), 0xa, 0x200, 0x100);
    ApproachAngle((short *)(c + 0x92), 0, 0xa, 0x200, 0x100);

    buf.v[0] = 0;
    buf.v[1] = 0;
    buf.v[2] = 0x5000;
    buf.v[3] = 0;
    buf.v[4] = 0;
    buf.v[5] = 0;

    p3e8 = (int *)(((int)c + 0x3e8));
    *p3e8 += 0x200;
    ang = *(int *)(c + 0x3e8);
    idx = ((u16)(short)ang >> 4) * 2;
    tbl = data_02082214[idx];
    result = (int)(((long long)tbl * 0x64000 + 0x800) >> 12);
    _Z14ApproachLinearRiii((int *)(c + 0x60), *(int *)(c + 0x3dc) + result, 0x3000);

    Matrix4x3_FromRotationY(&data_020a0e68, *(s16 *)(c + 0x94));
    MulVec3Mat4x3(&buf, &data_020a0e68, (void *)(c + 0xa4));
    return 1;
}

// @symbol func_ov094_02135e64
extern "C" int func_ov094_02135e64(char *c)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x30c, (void *)data_ov094_02136af0[1], 0, 0x1000, 0);
    *(short *)(c + 0x3ec) = (short)((RandomIntInternal(data_0209e650) >> 8) << 0xc);
    *(short *)(c + 0x100) = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0x3f) + 0x32);
    *(int *)(c + 0x3f0) = 0x1000;
    return 1;
}

// @symbol func_ov094_02135ee0
extern "C" int func_ov094_02135ee0(void *self)
{
    char *s = (char *)self;
    if (*(Player **)(s + 0x3d0) == 0) {
        func_ov094_02136188((C *)self, (PMF *)&data_ov094_02136b60);
        return 1;
    }
    Vector3 v = *(Vector3 *)((char *)(*(Player **)(s + 0x3d0)) + 0x5c);
    Vector3 w;
    w.x = v.x;
    w.y = v.y;
    w.z = v.z;
    ApproachAngle((short *)(s + 0x94), Vec3_HorzAngle(s + 0x5c, &w), 0xa, 0x200, 0x100);
    Vector3 u;
    v.y += 0xc8000;
    u.x = v.x - 0;
    u.y = v.y;
    u.z = v.z;
    ApproachAngle((short *)(s + 0x92), Vec3_VertAngle(s + 0x5c, &u), 0xa, 0x200, 0x100);
    if ((*(Player **)(s + 0x3d0))->GetTalkState() < 0) {
        *(char *)(s + 0x3d4) = 2;
        func_ov094_02136188((C *)self, (PMF *)&data_ov094_02136b60);
    }
    return 1;
}

// @symbol func_ov094_02135fe0
extern "C" int func_ov094_02135fe0(char *c)
{
    *(int *)(c + 0xa4) = 0;
    *(int *)(c + 0xa8) = 0;
    *(int *)(c + 0xac) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x30c, (void *)data_ov094_02136af0[1], 0, 0x1000, 0);
    return 1;
}

// @symbol func_ov094_02136024
extern "C" int func_ov094_02136024(char *c)
{
#define LA(p) ((char *)(unsigned)((unsigned)(p)))
    char *p = (char *)_ZN8dActor_c13ClosestPlayerEv(c);
    if (p != 0 && *(int *)(p + 0x37c) != 0) {
        char *ip = LA(p + 0x5c);
        OwlVec pp;
        OwlVec d;
        OwlVec *selfpos = (OwlVec *)(c + 0x5c);
        pp.x = *(int *)(ip + 0);
        pp.y = *(int *)(ip + 4);
        pp.z = *(int *)(ip + 8);
        *(int *)(c + 0x60) = *(int *)(p + 0x644) + 0x190000;
        *(int *)(c + 0x3d0) = (int)p;
        Vec3_Sub(&d, selfpos, &pp);
        if (LenVec3(&d) < 0x28000) {
            d.x = *(int *)(c + 0x5c);
            d.y = *(int *)(c + 0x60);
            d.z = *(int *)(c + 0x64);
            d.x = 0;
            d.y = 0;
            d.z = 0x320000;
            Matrix4x3_FromRotationY(&data_020a0e68, 0x4000);
            MulVec3Mat4x3(&d, &data_020a0e68, (OwlVec *)(c + 0x3d8));
            {
                int *px = (int *)LA(c + 0x3d8);
                int *py = (int *)LA(c + 0x3dc);
                int *pz = (int *)LA(c + 0x3e0);
                *px = *px + *(int *)(c + 0x5c);
                *py = *py + *(int *)(c + 0x60);
                *pz = *pz + *(int *)(c + 0x64);
            }
            *(short *)(c + 0x94) = Vec3_HorzAngle((OwlVec *)(c + 0x5c), (OwlVec *)(c + 0x3d8));
            *(short *)(c + 0x8e) = *(short *)(c + 0x94);
            func_ov094_02136188((C *)c, (PMF *)&data_ov094_02136b60);
        }
    }
    return 1;
#undef LA
}

// @symbol func_ov094_02136150
extern "C" int func_ov094_02136150(char *c)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x30c, *(void **)((char *)data_ov094_02136af0 + 4), 0, 0x1000, 0);
    return 1;
}

// @symbol func_ov094_02136188
extern "C" int func_ov094_02136188(C *c, PMF *p)
{
    c->pp = p;
    PMF *q = c->pp;
    if (*q == 0)
        return 1;
    return (c->**q)();
}

// @symbol func_ov094_021361d8
extern "C" void func_ov094_021361d8(void *raw)
{
    char *self = (char *)raw;
    Matrix4x3 out;
    OwlVec v;
    Vec3_Asr((Vector3 *)&v, (Vector3 *)(self + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(s16 *)(self + 0x8c), *(s16 *)(self + 0x8e), *(s16 *)(self + 0x90));
    _ZN9ModelBase12ApplyOpacityEjj(self + 0x30c, *(unsigned char *)(self + 0x3e4), 1);
    *(Matrix4x3 *)(self + 0x328) = data_020a0e68;
    MulMat4x3Mat4x3((const int *)(*(char **)(self + 0x320) + 0x30),
        (const int *)(self + 0x328), out.m);
    Matrix4x3_FromTranslation(&data_020a0e68,
        *(Fix12i *)(self + 0x5c) >> 3,
        (*(Fix12i *)(self + 0x60) - 0x38000) >> 3,
        *(Fix12i *)(self + 0x64) >> 3);
    *(Matrix4x3 *)(self + 0x398) = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        self, self + 0x370, (Matrix4x3 *)(self + 0x398), 0x64000, 0x320000, 0xf);
}

// @symbol func_ov094_021362e0
extern "C" void func_ov094_021362e0(void *raw)
{
    char *c = (char *)raw;
    OwlVec v;
    char *m;
    int z = 0;
    if (*(int *)(c + 0x3cc) == 0)
        return;
    *(volatile Fix12i *)&v.x = z;
    *(volatile Fix12i *)&v.y = z;
    *(volatile Fix12i *)&v.z = z;
    m = *(char **)(c + 0x3cc);
    data_020a0e68 = *(Matrix4x3 *)(*(char **)(m + 0xc8));
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0x3000, z, z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, -0x6000, 0x1000, 0x4000);
    v.x = data_020a0e68.m[9];
    v.y = data_020a0e68.m[10];
    v.z = data_020a0e68.m[11];
    *(int *)(c + 0x5c) = v.x << 3;
    *(int *)(c + 0x60) = v.y << 3;
    *(int *)(c + 0x64) = v.z << 3;
    *(s16 *)(c + 0x94) = *(s16 *)(*(char **)(c + 0x3cc) + 0x8e);
    *(s16 *)(c + 0x92) = (s16)z;
    *(Matrix4x3 *)(c + 0x328) = data_020a0e68;
}

// @symbol _ZN7daOwl_c16CleanupResourcesEv
int daOwl_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov094_02136ae0)->Release();
    ((SharedFilePtr *)data_ov094_02136af8)->Release();
    ((SharedFilePtr *)data_ov094_02136ae8)->Release();
    ((SharedFilePtr *)data_ov094_02136af0)->Release();
    return 1;
}

// @symbol _ZN7daOwl_c16OnPendingDestroyEv
void daOwl_c::OnPendingDestroy()
{
}

// @symbol _ZN7daOwl_c6RenderEv
int daOwl_c::Render()
{
    if (mCurrentState == (State *)data_ov094_02136b40)
        return 1;
    mModelAnim.Render(0);
    return 1;
}

// @symbol _ZN7daOwl_c8BehaviorEv
int daOwl_c::Behavior()
{
    DecIfAbove0_Short(&mStateTimer);
    {
        State *o = mCurrentState;
        /* Read the handler word. `&o->mMain` materialises the whole pmf. */
        if (*(int *)((char *)o + 8) != 0) {
            (this->*(o->mMain))();
        }
    }
    if (mCurrentState == (State *)data_ov094_02136b40)
        return 1;
    mModelAnim.speed = mAnimSpeed;
    mModelAnim.Advance();
    {
        State *m = mCurrentState;
        if ((m == (State *)data_ov094_02136b50 || m == (State *)data_ov094_02136b60 ||
             m == (State *)data_ov094_02136b30) &&
            (unsigned short)(mModelAnim.currFrame >> 0xc) == 0) {
            func_02012694(0x139, &mCamSpacePosX);
        }
    }
    /* The ROM tests this state twice; the else is unreachable. */
    if (mCurrentState == (State *)data_ov094_02136b70) {
        if (mCurrentState == (State *)data_ov094_02136b70) {
            func_ov094_021362e0(this);
            mAngleX = mPrevAngleX;
            mAngleY = mPrevAngleY;
            mAngleZ = mPrevAngleZ;
            UpdateWMClsn(mWithMeshClsn, 0);
        } else {
            func_ov094_021361d8(this);
        }
        return 1;
    }
    {
        int fallSpeed = mVertSpeed + mVertAccel;
        int clamped = mTerminalVelocity;
        int keep = unk_0ac;
        if (fallSpeed >= clamped)
            clamped = fallSpeed;
        mVertSpeed = clamped;
        unk_0ac = keep;
    }
    UpdatePosWithOnlySpeed(&mdCcAcPos_c);
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    func_ov094_021361d8(this);
    if (mCurrentState == (State *)data_ov094_02136b60 && unk_3d4 == 2) {
        func_ov094_021357a4((char *)this);
    }
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    return 1;
}

// @symbol _ZN7daOwl_c13InitResourcesEv
int daOwl_c::InitResources()
{
    int v0[3];
    void *f;

    f = Model::LoadFile(*(SharedFilePtr *)data_ov094_02136ae0);
    mModelAnim.SetFile((BMD_File *)f, 1, -1);
    mShadowModel.InitCylinder();
    Animation::LoadFile(*(SharedFilePtr *)data_ov094_02136af8);
    Animation::LoadFile(*(SharedFilePtr *)data_ov094_02136ae8);
    Animation::LoadFile(*(SharedFilePtr *)data_ov094_02136af0);
    v0[0] = data_ov094_02136a1c[0];
    v0[1] = data_ov094_02136a1c[1];
    v0[2] = data_ov094_02136a1c[2];
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, v0, 0x64000, 0x64000, 0x800004, 0);

    unk_3cc = 0;
    mdCcAcPos_c.flags |= 2;
    mTerminalVelocity = -0x1e000;
    mAnimSpeed = 0x1000;
    unk_3e4 = 0x1f;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x50000, 0x64000, (void *)0, 0);
    func_ov094_02136188((C *)this, (PMF *)&data_ov094_02136b40);

    if (data_0209f2f8 != 7)
        goto ret1;
    if (data_0209f220 != 1) {
        if (IsStarCollectedInCurLevel(1) != 0)
            goto ret1;
    }
    MarkForDestruction();
    return 0;
ret1:
    return 1;
}
