//cpp
/* Production translation unit for ov021/daGrock_c.
 * 12 function(s), .text 0x021121e8..0x02112d64. The GORO_ROCK rolling rock.
 *
 * NAME: _ZTS9daGrock_c is "9daGrock_c" at ov021 0x0211480c; _ZTI at
 * 0x02114818 reads [__si_class_type_info, that string, _ZTI12dEnemyBase_c].
 * The vtable address point is 0x02114848, and the word at -4 is that _ZTI.
 * The tree previously called the class RollingRock (coined; vtable address
 * only).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x021121e8), D0
 * (0x02112230), then a D2 the cartridge has no home for (manifest:
 * compiler_only_output); the same pragma lays .text down in source order, so
 * this file is ROM-ascending. daGrock_c_classInit at 0x02112d64 abuts the run
 * and stays in src/d_a_grock.c.
 */

#pragma defer_codegen off

#include "decl_common.h"
#include "daGrock_c.h"
#include "Player.h"

/* Flat field view of the GORO_ROCK file handle. SharedFilePtr.h has the
 * method and no fields; InitResources reads .bmd, so this completes the
 * forward declaration with the layout that read uses. */
struct SharedFilePtr {
    void *file;
    void *bmd;
    void Release();
};

/* 02112544 copies three words as a value with an empty constructor and
 * destructor. A POD Vec3 here changes that function; InitResources keeps
 * its own POD Vec3 inside the function. */
struct Vec3 {
    int x, y, z;
    Vec3() {}
    ~Vec3() {}
};

/* The copy at +0x92 has to go through a struct whose only member is an array.
 * Vector3_16's s16 members scalarise to LDRSH and interleave the store;
 * an unsigned array keeps the ROM's LDRH load-load-store. */
struct AngleWords { u16 w[3]; };

extern "C" {
extern void Vec3_Asr(void *dst, void *src, int n);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, short a, short b, short c);
extern int _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *thiz, void *sm, void *m, int rad, int h, unsigned u);
extern struct Matrix4x3 data_020a0e68;
extern void func_02012694(int a, void *p);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, int x, int y, int z);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, void *pos);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *p, struct Vector3 *pos, u32 a, int b, u32 d, u32 e, u32 f);
extern void dBgCh_Actr_UpdateContinuous_Veneer(void *p);
extern void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *p);
extern void func_0201267c(int a, void *p);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int a, unsigned int b, int c, int d, int e, void *v, void *cb);
extern void *_ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, void *v, unsigned int d);
extern int Vec3_HorzLen(void *v);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern void *_ZNK10dBgCh_Actr13GetWallResultEv(void *p);
/* local extern: dActor_c.h types GetSubtraction(short, short); func_ov021_02112544 passes its int wall angle straight through (mov r2, r0) and the short spelling adds lsl/asr #16 */
extern int _ZN8dActor_c14GetSubtractionEss(void *a, s16 x, int y);
extern s16 _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void *a, int x, int y, s16 ang);
extern s16 data_02082214[];
extern void AddVec3(struct Vector3 *dst, struct Vector3 *a, struct Vector3 *b);
extern void func_0200f760(char *c, void *p);
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *w, void *a, int r, int rr, void *v, void *vv);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *c, void *a, void *pos, int r, int rr, u32 e, u32 f);
extern SharedFilePtr data_ov021_02114a50;
}

/* D1 is one vtable store and a destructor call per member, every one a
 * consequence of `struct daGrock_c : dEnemyBase_c` and the members it types,
 * destroyed in reverse declaration order, then dEnemyBase_c::~dEnemyBase_c.
 * That body is the evidence for the header: each member's size closes exactly
 * on the next one's offset. D0 adds dEnemyBase_c's inline operator delete. */
// @symbol _ZN9daGrock_cD1Ev
// @symbol _ZN9daGrock_cD0Ev
daGrock_c::~daGrock_c()
{
}

// @symbol _ZN9daGrock_c16OnAimedAtWithEggEv
s32 daGrock_c::OnAimedAtWithEgg() {
    return 0;
}

// @symbol func_ov021_02112294
extern "C" {
void func_ov021_02112294(void *c)
{
    if (*(unsigned char*)((char*)c + 0x3c1) == 0xff)
        return;
    if (*(unsigned char*)((char*)c + 0x3be) == 4) {
        ((dActor_c *)c)->UntrackAndSpawnStar(*(s8*)((char*)c + 0x3c0), *(unsigned char*)((char*)c + 0x3c1), *(Vector3*)((char*)c + 0x5c), 2);
    } else {
        ((dActor_c *)c)->UntrackAndSpawnStar(*(s8*)((char*)c + 0x3c0), *(unsigned char*)((char*)c + 0x3c1), *(Vector3*)((char*)c + 0x5c), 4);
    }
}
}

// @symbol func_ov021_021122fc
extern "C" {
int func_ov021_021122fc(char* c){
  int tmp[3];
  Vec3_Asr(tmp, c+0x5c, 3);
  Matrix4x3_FromTranslation(&data_020a0e68, tmp[0], tmp[1], tmp[2]);
  *(struct Matrix4x3*)(c+0x188) = data_020a0e68;
  Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, *(short*)(c+0x8c), *(short*)(c+0x8e), *(short*)(c+0x90));
  *(struct Matrix4x3*)(c+0x12c) = data_020a0e68;
  return _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c+0x160, c+0x188, 0x1f4000, 0x1f4000, 0xf);
}
}

// @symbol func_ov021_021123b0
extern "C" {
void func_ov021_021123b0(char *c)
{
    char *a;
    u32 fl;
    u32 id = *(u32*)(c + 0x1dc);

    if (id == 0) return;
    a = (char*)dActor_c::FindWithID(id);
    if (a == 0) return;
    fl = *(u32*)(c + 0x1d8);
    if ((fl & 0x10) != 0) {
        func_02012694(0x17a, c + 0x74);
        ((Player *)a)->IncMegaKillCount();
        ((dActor_c *)c)->TriplePoofDust();
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
            0x67, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
        func_ov021_02112294(c);
        ((fBase_c *)c)->MarkForDestruction();
        return;
    }

    {
        int b = (int)(*(u16*)(a + 0xc) == 0xbf);
        if (b == 0) return;
    }
    if (*(u8*)(a + 0x6fb) != 0) return;
    if ((fl & 0x3c0) != 0) {
        if (*(int*)(a + 8) == 2) {
            func_02012694(0x17a, c + 0x74);
            ((fBase_c *)c)->MarkForDestruction();
            ((dActor_c *)c)->TriplePoofDust();
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                0x67, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
            func_ov021_02112294(c);
            return;
        }
        _ZN5Sound9PlayBank0EjRK7Vector3(0xb5, c + 0x74);
        return;
    }

    if (*(u8*)(c + 0x3be) == 4) return;
    {
        struct Vector3 v;
        v.x = *(int*)(c + 0x5c);
        v.y = *(int*)(c + 0x60);
        v.z = *(int*)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &v, 3, 0xc000, 1, 0, 1);
    }
}
}

// @symbol func_ov021_02112544
extern "C" void func_ov021_02112544(char* self)
{
    Vec3 normal;
    Vec3 pos;
    Vec3 walln;

    dBgCh_Actr_UpdateContinuous_Veneer(self + 0x1f8);
    if (((dBgCh_Actr *)(self + 0x1f8))->IsOnGround() != 0) {
        ((SurfaceInfo *)((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(self + 0x1f8) + 4))->CopyNormalTo(*(Vector3 *)&normal);

        pos = *(Vec3*)(self + 0x5c);
        pos.x -= normal.x * 0x12c;
        pos.y -= normal.y * 0x12c;
        pos.z -= normal.z * 0x12c;

        if (((dBgCh_Actr *)(self + 0x1f8))->JustHitGround() != 0) {
            if (*(int*)(self + 0xa8) < (*(int*)(self + 0x9c) << 1) && normal.y >= 0xc00) {
                *(int*)(self + 0xa8) = -*(int*)(self + 0xa8) * 6 / 10;
            }
            pos.y += 0x3c000;
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x68, pos.x, pos.y, pos.z);
            func_0201267c(0x48, self + 0x74);
        } else {
            *(int*)(self + 0x3c4) = (int)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                *(int*)(self + 0x3c4), 0x69, pos.x, pos.y, pos.z, 0, 0);
            *(int*)(self + 0x3b8) = (int)_ZN5Sound8PlayLongEjjjRK7Vector3s(
                *(int*)(self + 0x3b8), 3, 0x8a, self + 0x74, 0);
        }
        {
            int *pa4 = (int *)(int)(self + 0xa4);
            int *pac = (int *)(int)(self + 0xac);
            *pa4 = *pa4 + normal.x * 5;
            *pac = *pac + normal.z * 5;
        }
        *(int*)(self + 0x98) = Vec3_HorzLen(self + 0xa4);
        *(s16*)(self + 0x94) = (s16)_ZN4cstd5atan2E5Fix12IiES1_(*(int*)(self + 0xa4), *(int*)(self + 0xac));
        if (*(int*)(self + 0x98) >= 0x22000) {
            *(int*)(self + 0x98) = 0x22000;
            *(int*)(self + 0xa4) = data_02082214[(*(u16*)(self + 0x94) >> 4) << 1] * 0x22;
            *(int*)(self + 0xac) = data_02082214[((*(u16*)(self + 0x94) >> 4) << 1) + 1] * 0x22;
        }
    }

    if (((dBgCh_Actr *)(self + 0x1f8))->IsOnWall() == 0)
        return;

    ((SurfaceInfo *)((char*)_ZNK10dBgCh_Actr13GetWallResultEv(self + 0x1f8) + 4))->CopyNormalTo(*(Vector3 *)&walln);
    {
        int wa = _ZN4cstd5atan2E5Fix12IiES1_(walln.x, walln.z);
        if (_ZN8dActor_c14GetSubtractionEss(self, *(s16*)(self + 0x94), wa) <= 0x1000)
            return;
        *(s16*)(self + 0x94) = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(self, walln.x, walln.z, *(s16*)(self + 0x94));
        ((dActor_c *)self)->UpdatePosWithHorzSpeedAndAng();
    }
}

// @symbol func_ov021_021127b4
extern "C" {
void func_ov021_021127b4(char* c){
  int sum = *(int*)(c+0xa8) + *(int*)(c+0x9c);
  int m = *(int*)(c+0xa0);
  if (sum >= m) m = sum;
  *(int*)(c+0xa8) = m;
  AddVec3((struct Vector3*)(c+0x5c), (struct Vector3*)(c+0xa4), (struct Vector3*)(c+0x5c));
}
}

// @symbol _ZN9daGrock_c16CleanupResourcesEv
int daGrock_c::CleanupResources()
{
    ((SharedFilePtr *)&data_ov021_02114a50)->Release();
    return 1;
}

// @symbol _ZN9daGrock_c6RenderEv
int daGrock_c::Render()
{
  if(mType >= 2)
    mModel.Render(0);
  return 1;
}

// @symbol _ZN9daGrock_c8BehaviorEv
int daGrock_c::Behavior()
{
    char *c = (char *)this;
    if (*(u8*)(c + 0x3be) >= 2) {
        func_0200f760(c, c + 0x1b8);
        func_ov021_021123b0(c);
        if (*(int*)(c + 0x10c) == 8) return 1;

        if (*(u8*)(c + 0x3be) != 4) {
            s16 *p8c;
            func_ov021_021127b4(c);
            p8c = (s16 *)(int)(c + 0x8c);
            *p8c = (s16)(*p8c + (*(int*)(c + 0x98) >> 12) * 0x43);
            *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
            func_ov021_02112544(c);
        }
        func_ov021_021122fc(c);
        ((dCc_c *)(c + 0x1b8))->Clear();

        if (*(int*)(c + 0x64) >= (int)0xfe82c000) {
            struct Vector3 v;
            v.x = 0;
            v.y = (int)0xffebb000;
            v.z = 0;
            ((dCcAcPos_c *)(c + 0x1b8))->SetPosRelativeToActor(v);
        } else {
            struct Vector3 v;
            v.x = 0;
            v.y = -0xe1000;
            v.z = 0;
            ((dCcAcPos_c *)(c + 0x1b8))->SetPosRelativeToActor(v);
        }
        ((dCc_c *)(c + 0x1b8))->dCc_c::Update();

        if (*(u8*)(c + 0x3be) != 4) {
            if (*(int*)(c + 0x60) < -0x3e8000) {
                ((fBase_c *)c)->MarkForDestruction();
                if (*(u32*)(c + 0x3b4) != 0) {
                    char *a = (char*)dActor_c::FindWithID(*(u32*)(c + 0x3b4));
                    if (a) *(u8*)(a + 0x3c2) = 0;
                }
                ((dActor_c *)c)->UntrackStar(*(s8*)(c + 0x3c0));
                ((dActor_c *)c)->TriplePoofDust();
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x67, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
                func_02012694(0x17a, c + 0x74);
            }
        }
    } else {
        int dist = ((dActor_c *)c)->DistToCPlayer();
        if (dist > 0x5dc000) {
            u32 th = (dist < 0x1770000) ? 0x70 : 0xe0;
            if (*(u16*)(c + 0x100) >= th) {
                struct Vector3_16 v16;
                u32 rnd;
                u8 *p3bf;
                p3bf = (u8 *)(int)(c + 0x3bf);
                *p3bf = (u8)(*p3bf + 1);
                *(u16*)(c + 0x100) = 0;
                *(AngleWords *)&v16 = *(AngleWords *)(c + 0x92);
                rnd = (u32)RandomIntInternal(&data_0209e650);
                v16.y = v16.y + (rnd >> 16) % 0xc00;
                if (*(u8*)(c + 0x3be) == 1 && *(u8*)(c + 0x3bf) >= 5 && *(u8*)(c + 0x3c2) == 0) {
                    char *a;
                    *(u8*)(c + 0x3c2) = 1;
                    a = (char*)dActor_c::Spawn(0xdd, (*(u8*)(c + 0x3c1) << 8) | 3, *(Vector3*)(c + 0x5c), &v16, *(s8*)(c + 0xcc), -1);
                    *(u8*)(c + 0x3bf) = 0;
                    if (a) *(u32*)(a + 0x3b4) = *(u32*)(c + 4);
                } else {
                    dActor_c::Spawn(0xdd, 2, *(Vector3*)(c + 0x5c), &v16, *(s8*)(c + 0xcc), -1);
                }
            }
        }
        {
            u16 *p100 = (u16 *)(int)(c + 0x100);
            *p100 = (u16)(*p100 + 1);
        }
    }
    return 1;
}

// @symbol _ZN9daGrock_c13InitResourcesEv
int daGrock_c::InitResources()
{
    struct Vec3 { s32 x, y, z; };
    u8* c = (u8*)((void*)this);
    *(u8*)(c+0x3be) = (u8)(*(u32*)(c+8) & 0xf);
    *(u8*)(c+0x3c1) = 0xff;
    *(signed char*)(c+0x3c0) = -1;
    *(u8*)(c+0x3bf) = 0;

    Model::LoadFile(data_ov021_02114a50);

    if (*(u8*)(c+0x3be) >= 2) {
        if (((ModelBase *)((void*)(c+0x110)))->SetFile((BMD_File *)data_ov021_02114a50.bmd, 1, 1) == 0)
            return 0;
        if (((ShadowModel *)((void*)(c+0x160)))->InitCylinder() == 0)
            return 0;

        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_((void*)(c+0x1f8), ((void*)this), 0x12c000, 0, 0, 0);
        ((dBgCh_Actr *)((void*)(c+0x1f8)))->SetLimMovFlag();
        *(s32*)(c+0x3b8) = 0;

        if (*(u8*)(c+0x3be) == 4) {
            Vec3 pos;
            *(u8*)(c+0x3c1) = (u8)((*(u32*)(c+8) >> 8) & 0xf);
            pos.x = 0; pos.y = 0; pos.z = 0;
            _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj((void*)(c+0x1b8), ((void*)this), &pos, 0x10e000, 0x226000, 0x200004, 0x3c0);
        } else {
            Vec3 pos;
            pos.x = 0; pos.y = 0; pos.z = 0;
            _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj((void*)(c+0x1b8), ((void*)this), &pos, 0xf3000, 0x226000, 0x200004, 0x3c0);

            if (*(u8*)(c+0x3be) == 3) {
                *(u8*)(c+0x3c1) = (u8)((*(u32*)(c+8) >> 8) & 0xf);
            }
            *(s32*)(c+0x9c) = -0x4000;
            *(s32*)(c+0xa0) = -0x28000;
            *(s32*)(c+0x98) = 0x1e000;
            *(s32*)(c+0xa4) = data_02082214[(*(u16*)(c+0x94)>>4)<<1] * 0x1e;
            *(s32*)(c+0xac) = data_02082214[((*(u16*)(c+0x94)>>4)<<1)+1] * 0x1e;
        }
    } else {
        if (*(u8*)(c+0x3be) == 1) {
            *(u8*)(c+0x3c1) = (u8)((*(u32*)(c+8) >> 8) & 0xf);
        }
    }

    if (*(u8*)(c+0x3be) != 1 && *(u8*)(c+0x3c1) != 0xff) {
        *(u8*)(c+0x3c0) = TrackStar(*(u8*)(c+0x3c1), 2);
    }
    *(s32*)(c+0x3c4) = 0;
    *(s32*)(c+0x10c) = 0;
    *(s32*)(c+0x3b4) = 0;
    *(u8*)(c+0x3c2) = 0;
    return 1;
}
