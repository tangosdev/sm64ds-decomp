//cpp
/* Production translation unit for ov065/daBasabasa_c.
 * 21 functions, .text 0x02116f98..0x02117ee8. The swoop (profile BASABASA).
 *
 * NAME: the vtable address point is ov065 0x0211cca0. The word at -4 is
 * 0x0211cc60 (_ZTI12daBasabasa_c), which reads [__si_class_type_info,
 * 0x0211cc6c, _ZTI12dEnemyBase_c]: _ZTS12daBasabasa_c at 0x0211cc6c is the
 * cartridge string "12daBasabasa_c", and the single base is dEnemyBase_c.
 * The class was coined Swoop before the ROM name was read (factory alias
 * Swoop_Spawn); the _ZTV5Swoop label of that vtable is gone.
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS and the ancestor RTTI. Under `#pragma defer_codegen off` it comes out
 * D1 (0x02116f98), D0 (0x02116fe8), then a D2 the cartridge has no home for
 * (manifest: deadstrip). The same pragma lays .text down in source order, so
 * this file is ROM-ascending.
 *
 * daBasabasa_c_classInit at 0x02117ee8 is the next function and is not absorbed.
 * Nothing here reads Matrix4x3::t, so common.h comes first and the flat matrix
 * spelling stands.
 */

#pragma defer_codegen off
#include "common.h"
#include "daBasabasa_c.h"
#include "SharedFilePtr.h"
#include "Player.h"

/* func_ov065_021177e4's unused stack temporary. The three-int constructor
 * is what schedules the stores; a plain Vector3 assignment does not. The
 * empty destructor is homeless and licensed deadstrip. */
struct PoofPos {
    int x, y, z;
    PoofPos(int a, int b, int c) : x(a), y(b), z(c) {}
    ~PoofPos() {}
};

struct C;
typedef int (C::*PMF)();
struct C { char pad[0x420]; PMF *pp; };

extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* m, void* f, int a, int fix, unsigned sp0);
extern void func_ov002_020aea30(void *self, void *actor, void *collision);
extern void func_02012694(int a, void* b);
extern void _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(char* c, short* v, char* p);
extern void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(char* c, Vector3* pos, unsigned n, int fix, short s);
extern void _ZN6Player6BounceE5Fix12IiE(char* p, int fix);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(char* p, Vector3* pos, unsigned a, int fix, unsigned b, unsigned d, unsigned e);
extern int func_ov065_02117944(C *c, PMF *p);
extern char data_ov065_0211d6e0[];
extern char data_ov065_0211d6f0[];
int Vec3_Dist(const void* a, const void* b);
short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
void Matrix4x3_FromRotationY(void* m, int angle);
void ApproachAngle(short* a, int b, int c, int d, int e);
void MulVec3Mat4x3(const void* in, const void* m, void* out);
extern int data_ov065_0211d700;
extern Matrix4x3 data_020a0e68;
extern unsigned int RandomIntInternal(int* seed);
extern int data_0209e650;
extern void _Z14ApproachLinearRsss(short* p, short t, short step);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* m, short ang);
extern int func_ov065_021177e4(int *t);
extern void* data_ov065_0211d710;
extern void Vec3_Asr(Vector3* d, Vector3* s, int sh);
extern void Matrix4x3_FromTranslation(Matrix4x3* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* sm, Matrix4x3* m, int fx, int t, unsigned int u);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern SharedFilePtr data_ov065_0211d698;
extern SharedFilePtr data_ov065_0211d6a8;
extern SharedFilePtr data_ov065_0211d690;
extern SharedFilePtr data_ov065_0211d6a0;
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, dActor_c* a, int r, int h, unsigned int e, unsigned int g);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, int r, int h, Vector3_16* p, Vector3_16* q);
}


/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN12daBasabasa_cD1Ev, 0x02116f98, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBasabasa_cD1Ev
// @symbol _ZN12daBasabasa_cD0Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and six destructor calls, every one a consequence of
 * `struct daBasabasa_c : dEnemyBase_c` and the members that declaration types:
 *   0x3c8 ShadowModel, 0x364 and 0x300 the two ModelAnims,
 *   0x144 dBgCh_Actr, 0x110 dCcAc_c,
 * then dEnemyBase_c::~dEnemyBase_c, in reverse declaration order.
 *
 * This body is the evidence for the header. Each member's size closes exactly
 * on the next one's offset.
 */
daBasabasa_c::~daBasabasa_c()
{
}
/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov065_0211704c, 0x0211704c, size 0x3b8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_0211704c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov065_0211704c(char* c)
{
    char* p;
    int flags;
    int b;

    if (*(int*)(c + 0x134) == 0) return;
    p = (char *)dActor_c::FindWithID(*(int*)(c + 0x134));
    if (p == 0) return;
    flags = *(int*)(c + 0x130);
    if (flags & 0x2400) {
        *(unsigned char*)(c + 0x43c) = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, (void *)((int *)&data_ov065_0211d690)[1], 0, 0x1000, 0);
        *(int*)(c + 0x10c) = 2;
        func_ov002_020aea30(c, p, 0);
        func_02012694(0x112, c + 0x74);
        return;
    }
    if (flags & 0x10) {
        short v[3];
        v[0] = -0x2000;
        v[1] = 0;
        v[2] = 0;
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, v, p);
        func_02012694(0x112, c + 0x74);
        return;
    }
    if (flags & 0x40000) {
        *(int*)(c + 0x10c) = 4;
        *(unsigned char*)(c + 0x43c) = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, (void *)((int *)&data_ov065_0211d690)[1], 0, 0x1000, 0);
        func_ov002_020aea30(c, p, 0);
        func_02012694(0x112, c + 0x74);
        return;
    }
    if (flags & 0x4380) {
        *(unsigned char*)(c + 0x43c) = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, (void *)((int *)&data_ov065_0211d690)[1], 0, 0x1000, 0);
        *(int*)(c + 0x10c) = 3;
        func_ov002_020aea30(c, p, 0);
        func_02012694(0x112, c + 0x74);
        return;
    }
    if (flags & 0x40) {
        *(unsigned char*)(c + 0x43c) = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, (void *)((int *)&data_ov065_0211d690)[1], 0, 0x1000, 0);
        *(int*)(c + 0x10c) = 2;
        func_ov002_020aea30(c, p, 0);
        func_02012694(0x112, c + 0x74);
        return;
    }
    if (flags & 0x20) {
        *(int*)(c + 0x10c) = 1;
        func_ov002_020aea30(c, p, 0);
        func_02012694(0x112, c + 0x74);
        return;
    }

    b = (int)(*(unsigned short*)(p + 0xc) == 0xbf);
    if (b == 0)
        return;

    if (((dActor_c *)c)->BumpedUnderneathByPlayer(*(Player *)p) == 1)
        goto kill;
    if (((Player *)p)->IsOnShell() == 1)
        goto kill;
    if (*(unsigned char*)(p + 0x6f9) != 1)
        goto jumped;

kill:
    {
        int pos[3];
        ((dActor_c *)c)->SmallPoofDust();
        ((dActor_c *)c)->KillAndTrackInDeathTable();
        func_02012694(0x112, c + 0x74);
        pos[0] = *(int*)(c + 0x5c);
        pos[1] = *(int*)(c + 0x60);
        pos[2] = *(int*)(c + 0x64);
        _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(
            c, (Vector3*)pos, *(unsigned char*)(c + 0x10a) + 1, 0xa000, 0);
        return;
    }

jumped:
    if (((dActor_c *)c)->JumpedOnByPlayer(*(dCc_c *)(c + 0x110), *(Player *)p) != 0) {
        _ZN6Player6BounceE5Fix12IiE(p, 0x28000);
        *(int*)(c + 0x10c) = 1;
        func_ov002_020aea30(c, p, 0);
        func_02012694(0x112, c + 0x74);
        return;
    }
    {
        int pos[3];
        pos[0] = *(int*)(c + 0x5c);
        pos[1] = *(int*)(c + 0x60);
        pos[2] = *(int*)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, (Vector3*)pos, 1, 0xc000, 1, 0, 1);
    }
    if (*(char**)(c + 0x420) != data_ov065_0211d6e0)
        return;
    *(int*)(c + 0x430) = 0;
    func_ov065_02117944((C *)c, (PMF *)data_ov065_0211d6f0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov065_02117404, 0x02117404, size 0x1ac */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02117404
/* recovered: shared common types */
extern "C" int func_ov065_02117404(unsigned char* thiz)
{
    Vector3 v;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    int dist = Vec3_Dist((Vector3*)(thiz + 0x5c), (Vector3*)(thiz + 0x424));
    if (dist <= 0x1f4000) {
        if (((dBgCh_Actr *)(thiz + 0x144))->IsOnWall() == 0) goto skip;
    }
    *(short*)(thiz + 0x434) = Vec3_HorzAngle((Vector3*)(thiz + 0x5c), (Vector3*)(thiz + 0x424));
    if (*(unsigned short*)(thiz + 0x100) < 0x14) *(unsigned short*)(thiz + 0x100) = 0x14;
skip:
    if (dist < 0x1f4000 && (*(int*)(thiz + 0xb0) & 8) != 0) {
        int* ip1 = (int*)((int)thiz + 0x430);
        *ip1 = *ip1 + 1;
        if (*(int*)(thiz + 0x430) > 0x64) {
            *(int*)(thiz + 0x430) = 0;
            int* ip2 = (int*)((int)thiz + 0xb0);
            *ip2 = *ip2 | 1;
            *(short*)(thiz + 0x92) = (short)-0x8000;
            *(short*)(thiz + 0x8c) = *(short*)(thiz + 0x92);
            *(int*)(thiz + 0x5c) = *(int*)(thiz + 0x424);
            *(int*)(thiz + 0x60) = *(int*)(thiz + 0x428);
            *(int*)(thiz + 0x64) = *(int*)(thiz + 0x42c);
            *(int*)(thiz + 0xa4) = 0;
            *(int*)(thiz + 0xa8) = 0;
            *(int*)(thiz + 0xac) = 0;
            func_ov065_02117944((C *)thiz, (PMF *)&data_ov065_0211d700);
            return 1;
        }
    }

    ApproachAngle((short*)(thiz + 0x92), 0, 0xa, 0x200, 0x100);
    ApproachAngle((short*)(thiz + 0x94), *(short*)(thiz + 0x434), 0xa, 0x200, 0x100);
    v.z = 0xa000;
    Matrix4x3_FromRotationY(&data_020a0e68, *(short*)(thiz + 0x8e));
    MulVec3Mat4x3(&v, &data_020a0e68, (Vector3*)(thiz + 0xa4));
    if (*(unsigned short*)(thiz + 0x100) == 0) {
        func_ov065_02117944((C *)thiz, (PMF *)&data_ov065_0211d6f0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov065_021175b0, 0x021175b0, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_021175b0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov065_021175b0(char* c)
{
    unsigned int r;
    r = (unsigned int)RandomIntInternal(&data_0209e650);
    *(short*)(c + 0x434) = ((r >> 8) & 0xf) << 0xc;
    r = (unsigned int)RandomIntInternal(&data_0209e650);
    *(short*)(c + 0x100) = ((r >> 8) & 0x1f) + 0x32;
    ((void (*)(int, void *, void *, int))func_02012694)(
        0xf9, c + 0x74, c + 0x100, ((r >> 8) & 0x1f) + 0x32);
    *(int*)(c + 0x35c) = 0x1000;
    (*(int *)(((int)c + 0xb0))) &= ~1;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov065_02117624, 0x02117624, size 0xd8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02117624
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov065_02117624(char* c) {
    int v[3];
    v[0] = 0; v[1] = 0; v[2] = 0;
    *(short*)(c + 0x434) = (short)((dActor_c *)c)->HorzAngleToCPlayer();
    _Z14ApproachLinearRsss((short*)(c + 0x94), *(short*)(c + 0x434), 0x500);
    _Z14ApproachLinearRsss((short*)(c + 0x92), 0x100, 0x500);
    v[2] = 0xa000;
    Matrix4x3_FromRotationY(&data_020a0e68, *(short*)(c + 0x8e));
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short*)(c + 0x92));
    MulVec3Mat4x3(v, &data_020a0e68, c + 0xa4);
    if (Vec3_Dist(c + 0x5c, c + 0x424) > 0x5dc000 || *(unsigned short*)(c + 0x100) == 0) {
        *(int*)(c + 0x430) = 0;
        func_ov065_02117944((C *)c, (PMF *)data_ov065_0211d6f0);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov065_021176fc, 0x021176fc, size 0x84 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_021176fc
extern "C" {
int func_ov065_021176fc(char* c){
  *(int*)(c+0x9c) = 0;
  *(char*)(c+0x43c) = 1;
  *(int*)(c+0xb0) = 0x10000003;
  *(short*)(c+0x100) = 0x64;
  *(int*)(c+0x35c) = 0x2000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x300, (void*)(void *)((int *)&data_ov065_0211d690)[1], 0, 0x1000, 0);
  *(short*)(c+0x400+0x34) = (short)(((RandomIntInternal(&data_0209e650) >> 8) & 0xf) << 0xc);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov065_02117780, 0x02117780, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02117780
/* func_ov065_02117780 at 0x02117780 (ov065), size 0x64
 * Compiler mwccarm 1.2/sp2p3 */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov065_02117780(char *c) {
    int r2, v60;
    char *p;
    *(short *)(int)(c + 0x94) += 0x1000;
    p = (char *)((dActor_c *)c)->ClosestPlayer();
    v60 = *(int *)(c + 0x60);
    r2 = v60 - 0xc8000;
    if (p != 0) {
        r2 = *(int *)(p + 0x644) + 0x50000;
    }
    if (*(unsigned short *)(c + 0x100) == 0 || r2 > v60) {
        func_ov065_02117944((C *)c, (PMF *)&data_ov065_0211d6e0);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov065_021177e4, 0x021177e4, size 0xa4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_021177e4
/* NOT daYurei_Mucho_c's, and not a vtable slot. This file used to carry the
   coined name `Snufit_Kill` and the attribution "daYurei_Mucho_c::Kill --
   recovered from vtable slot identity". Three ROM reads refute both halves, so
   the promotion that retired the coined class name `Snufit` retired this
   spelling with it rather than transplanting it onto the ROM RTTI name:

     * 0x021177e4 appears in NO vtable in this overlay. Reading the 33 words at
       0x0211cb9c (_ZTV15daYurei_Mucho_c storage) and at 0x0211cc98
       (_ZTV12daBasabasa_c / _ZTV12daBasabasa_c storage) out of
       extracted/overlays/overlay_0065.bin, neither contains it.
     * The address sits inside the NEIGHBOUR's text run, between
       _ZN12daBasabasa_cD0Ev (0x02116fe8) and _ZN12daBasabasa_c16CleanupResourcesEv
       (0x02117aa4), whose factory is daBasabasa_c_classInit at 0x02117ee8.
       daYurei_Mucho_c's run ends at 0x02116f98.
     * The body types `t` against a layout daYurei_Mucho_c does not have: it
       calls ModelAnim::SetAnim on `t + 0x364`, where daYurei_Mucho_c holds a
       ShadowModel (its ModelAnim is at 0x300), and it reads
       data_ov065_0211d6a0, outside daYurei_Mucho_c's bss band
       0x0211d600..0x0211d690.

   Naming this function therefore belongs to the daBasabasa_c owner, on
   that class's evidence. It keeps its address-derived symbol until then. */
int func_ov065_021177e4(int *t)
{
    t[0x27] = -0x1000;
    t[0x28] = -0xa000;
    *((char *)t + 0x43c) = 0;
    t[0xf0] = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)t + 0x364, (void *)((int *)&data_ov065_0211d6a0)[1], 0, 0x1000, 0);
    PoofPos vec(t[0x17], t[0x18] + 0x64000, t[0x19]);
    *(short *)((char *)t + 0x100) = 0x19;
    if (t[0x18] == 0x898000)
        *(short *)((char *)t + 0x100) = 0x32;
    func_02012694(0xf9, (char *)t + 0x74);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov065_02117888, 0x02117888, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02117888
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov065_02117888(char* c)
{
    char* p = (char *)((dActor_c *)c)->ClosestNonVanishPlayer();
    if (p) {
        Vector3 v;
        int* q = (int*)(((int)p + 0x5c));
        v.x = q[0];
        v.y = q[1];
        v.z = q[2];
        if (Vec3_Dist((const Vector3*)(c + 0x5c), &v) < 0x3e8000) {
            *(short*)(c + 0x100) = 0x14;
            func_ov065_02117944((C *)c, (PMF *)&data_ov065_0211d710);
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov065_021178fc, 0x021178fc, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_021178fc
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov065_021178fc(void* c) {
    *(int*)((char*)c + 0xb0) = 3;
    *(unsigned char*)((char*)c + 0x43c) = 0;
    *(int*)((char*)c + 0x3c0) = 0x1000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x364, (void *)((int *)&data_ov065_0211d6a0)[1], 0, 0x1000, 0);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov065_02117944, 0x02117944, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02117944
extern "C" int func_ov065_02117944(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov065_02117994, 0x02117994, size 0x110 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02117994
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov065_02117994(char* self){
    struct Vector3 v;
    Vec3_Asr(&v, (struct Vector3*)(self + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(s16*)(self + 0x8c), *(s16*)(self + 0x8e), *(s16*)(self + 0x90));
    if (*(unsigned char*)(self + 0x43c) == 1) {
        *(struct Matrix4x3*)(self + 0x31c) = data_020a0e68;
    } else {
        *(struct Matrix4x3*)(self + 0x380) = data_020a0e68;
    }
    Matrix4x3_FromTranslation(&data_020a0e68,
        *(Fix12i*)(self + 0x5c) >> 3,
        (*(Fix12i*)(self + 0x60) - 0x18000) >> 3,
        *(Fix12i*)(self + 0x64) >> 3);
    *(struct Matrix4x3*)(self + 0x3f0) = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        self, self + 0x3c8, (struct Matrix4x3*)(self + 0x3f0), 0x32000, 0x258000, 0xf);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- _ZN12daBasabasa_c16CleanupResourcesEv, 0x02117aa4, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBasabasa_c16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the four files InitResources claimed.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
int daBasabasa_c::CleanupResources()
{
    data_ov065_0211d698.Release();
    data_ov065_0211d6a8.Release();
    data_ov065_0211d690.Release();
    data_ov065_0211d6a0.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- _ZN12daBasabasa_c16OnPendingDestroyEv, 0x02117aec, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBasabasa_c16OnPendingDestroyEv
/* recovered: shared header, real C++ method
 *
 * fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`.
 */
void daBasabasa_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- _ZN12daBasabasa_c6RenderEv, 0x02117af0, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBasabasa_c6RenderEv
/* recovered: named members + shared header, real C++ method
 *
 * The six-slot `struct Base` this file used to cast each ModelAnim to was
 * ModelAnim's own vtable, and the slot it called is Render. unk_43c picks
 * which of the two models is drawn.
 */
int daBasabasa_c::Render()
{
    int flag = (mFlags & 0x40000) != 0;
    if (flag != 0) return 1;
    if (unk_43c == 1) {
        mModelAnim1.Render(0);
    } else {
        mModelAnim2.Render(0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- _ZN12daBasabasa_c8BehaviorEv, 0x02117b64, size 0x21c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBasabasa_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* This file used to open with `struct dEnemyBase_c { char pad[0x800]; };` and work a
 * `char *c` through raw offsets. daBasabasa_c.h now supplies the real chain, so the
 * stand-in is gone and every offset below is a named field.
 *
 * The two ModelAnims are what the tail is choosing between: `c + 0x350` and
 * `c + 0x3b4` are each one's Animation base (+0x50), so the branch advances
 * mModelAnim1 or mModelAnim2.
 */
int daBasabasa_c::Behavior()
{
    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        mdCcAc_c.Clear();
        if (mEatenByYoshi != 0) {
            if (unk_104 == 0) {
                mdCcAc_c.Update();
            }
        }
        func_ov065_02117994((char *)this);
        return 1;
    }
    if (UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim1, 3) != 0) {
        return 1;
    }
    if (mDeathState != 0) {
        UpdateDeath(mWithMeshClsn);
        func_ov065_02117994((char *)this);
        return 1;
    }
    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    {
        State *q = mCurrentState;
        /* Reads the handler's pointer word directly rather than as `&q->mMain`:
           taking the ADDRESS of a pointer-to-member makes mwcc materialise the
           whole 8-byte pmf. Reading one to CALL it is free. */
        if (*(int *)((char *)q + 8) != 0) (this->*(q->mMain))();
    }
    {
        State *m = mCurrentState;
        if (m == (State *)data_ov065_0211d6e0 || m == (State *)data_ov065_0211d6f0) {
            /* Three wingbeat frames, one flap sound, at the camera-space
               position. */
            if (mModelAnim1.WillHitFrame(3) != 0 ||
                mModelAnim1.WillHitFrame(0xf) != 0 ||
                mModelAnim1.WillHitFrame(0x1b) != 0) {
                func_02012694(0xe1, &mCamSpacePosX);
            }
        }
    }
    {
        /* Gravity, clamped at terminal velocity. unk_0ac is read and written
           back unchanged -- the ROM really does reload and restore it here. */
        int fallSpeed = mVertSpeed + mVertAccel;
        int clamped = mTerminalVelocity;
        if (fallSpeed >= clamped)
            clamped = fallSpeed;
        int keep = unk_0ac;
        mVertSpeed = clamped;
        unk_0ac = keep;
    }
    UpdatePosWithOnlySpeed(&mdCcAc_c);
    UpdateWMClsn(mWithMeshClsn, 0);
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    func_ov065_02117994((char *)this);
    if (unk_43c == 1) {
        func_ov065_0211704c((char *)this);
    }
    mdCcAc_c.Clear();
    {
        char *p = (char *)ClosestPlayer();
        if (p != 0 && *(unsigned char *)(p + 0x6fb) == 0) {
            mdCcAc_c.Update();
        }
    }
    if (unk_43c == 1) {
        mModelAnim1.Advance();
    } else {
        mModelAnim2.Advance();
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN12daBasabasa_c13InitResourcesEv, 0x02117d80, size 0x12c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBasabasa_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * The stand-in typedefs this file used to carry (`typedef int Fix12;`, a
 * two-word SharedFilePtr, a three-short Vector3_16, opaque dActor_c/BMD_File)
 * all name real types now that daBasabasa_c.h pulls in the dEnemyBase_c chain, so they are
 * gone. Both ModelAnims are loaded here, which is the other half of the
 * evidence that 0x300 and 0x364 are two members and not one and a gap.
 *
 * dCcAc_c::Init and dBgCh_Actr::Init are still reached through
 * their mangled names because neither is declared as a method yet.
 */
int daBasabasa_c::InitResources()
{
    mModelAnim1.SetFile((BMD_File *)Model::LoadFile(data_ov065_0211d698), 1, -1);
    mModelAnim2.SetFile((BMD_File *)Model::LoadFile(data_ov065_0211d6a8), 1, -1);
    mShadowModel.InitCylinder();
    Animation::LoadFile(data_ov065_0211d690);
    Animation::LoadFile(data_ov065_0211d6a0);
    mTerminalVelocity = -0xa000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x28000, 0x28000, 0x200000, 0x7eff0);
    mAngleY = mPrevAngleY;
    mPrevAngleX = -0x8000;
    mAngleX = mPrevAngleX;
    unk_43c = 0;
    mModelAnim1.speed = 0x1000;
    mModelAnim2.speed = 0x1000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x28000, 0x1e000, 0, 0);
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    unk_108 = 1;
    func_ov065_02117944((C *)this, (PMF *)&data_ov065_0211d700);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN12daBasabasa_c16OnAimedAtWithEggEv, 0x02117eac, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBasabasa_c16OnAimedAtWithEggEv
// recovered name: Swoop_OnAimedAtWithEgg
/* recovered: renamed to Class_Method */
/* daBasabasa_c::OnAimedAtWithEgg - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. */
s32 daBasabasa_c::OnAimedAtWithEgg() {
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- _ZN12daBasabasa_c13OnTurnIntoEggER6Player, 0x02117eb4, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBasabasa_c13OnTurnIntoEggER6Player
// recovered name: Swoop_OnTurnIntoEgg
/* daBasabasa_c::OnTurnIntoEgg -- vtable slot 19, verified against ov065 relocs.txt:
 * _ZTV12daBasabasa_c (0x0211cca0) + 0x4c -> 0x02117eb4, exactly this placeholder's
 * former address (former name func_ov065_02117eb4).
 * Matched byte-for-byte with mwccarm 2004/b56 (ov065).
 */
void daBasabasa_c::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, (unsigned char)(unk_10a + 1), 0);
    KillAndTrackInDeathTable();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN12daBasabasa_c13OnYoshiTryEatEv, 0x02117ee0, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBasabasa_c13OnYoshiTryEatEv
/* recovered: renamed to Class_Method */
s32 daBasabasa_c::OnYoshiTryEat() {
    return 4;
}

