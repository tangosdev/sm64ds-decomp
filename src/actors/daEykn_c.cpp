//cpp
/* Production translation unit for ov071/daEykn_c.
 * 22 function(s), .text 0x02120668..0x02121734. Mr. I (EYEKUN / EYEKUN_BOSS).
 *
 * NAME: _ZTS8daEykn_c is "8daEykn_c" at ov071 0x02122cd8; _ZTI at 0x02122ce4
 * reads [__si_class_type_info, that string, _ZTI8dActor_c]. The vtable address
 * point _ZTV8daEykn_c is 0x02122d30 (offset-to-top 0 at 0x02122d28, typeinfo
 * 0x02122ce4 at 0x02122d2c). The tree previously called the class MrI (coined;
 * ov071/symbols.txt only aliased _ZTV3MrI to the same vtable address).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS (externalized to those addresses; the TU is text-only). Under
 * `#pragma defer_codegen off` it comes out D1 (0x02120668), D0 (0x021206b0),
 * then a D2 the cartridge has no home for (manifest: deadstrip); the same
 * pragma lays .text down in source order, so this file is ROM-ascending.
 *
 * InitResources (0x02121734..0x021219cc) stays in
 * src/_ZN8daEykn_c13InitResourcesEv.cpp: it byte-matches but has no
 * delinks.txt entry, so it is not part of the linked build and cannot be
 * licensed here. The two classInit factories after it (src/d_a_eykn_eyekun_boss.c,
 * src/d_a_eykn_eyekun.c) are separate units and are not absorbed.
 */

#pragma defer_codegen off

#include "daEykn_c.h"
#include "common.h"
#include "types.h"
#include "SharedFilePtr.h"

/* shadow struct 'Vector3_16f' */
struct Vector3_16f;

/* shadow typedef 'AnimData' */
typedef struct { int f0; void *f4; } AnimData;

/* shadow typedef 'Item16' */
typedef struct { int a, b, c, d; } Item16;

/* shadow struct 'Sub' */
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(void*); };

/* shadow struct 'BMD_File' */
struct BMD_File;

/* shadow struct 'BTP_File' */
struct BTP_File;

/* shadow struct 'BCA_File' */
struct BCA_File;

struct C;
typedef void (C::*PMF)();
struct C { char pad[0x1e4]; PMF *pp; };

extern "C" {
extern int AngleDiff(int, int);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern "C" char *_ZN8dActor_c22ClosestNonVanishPlayerEv(void *self);
extern "C" int Vec3_Dist(const void *a, const void *b);
extern "C" short Vec3_HorzAngle(const void *a, const void *b);
extern "C" int _ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b(void *self, Vector3 *a, Vector3 *b, bool c);
void* _ZN8dActor_c7FindEggER5dCc_c(void* self, void* c);
void* _ZN8dActor_c18FindExplosionActorER5dCc_c(void* self, void* c);
void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, const void* v);
void func_ov071_02121634(char *self, int a);
void* _ZN8dActor_c10FindWithIDEj(u32 id);
void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, int c, int d, int e, const void* v, void* cb);
u32 _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(u32 a, u32 b, int c, int d, int e, const Vector3_16f* v);
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const void* v, u32 a, int b, u32 c, u32 d, u32 e);
extern int _Z14ApproachLinearRiii(int *ref, int target, int step);
extern void func_0201267c(unsigned int id, void *p);
extern void *_ZN8Particle6System12FromUniqueIDEj(unsigned int id);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN9Animation7AdvanceEv(void *self);
extern int _ZN9Animation8FinishedEv(void *self);
extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as( unsigned int actorID, unsigned int param, Vector3 *pos, void *rot, s8 areaID, s16 deathTableID);
extern void _ZN8dActor_c8PoofDustEv(void *self);
extern void _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h( void *self, signed char *starFlag, unsigned int starID, Vector3 *pos, unsigned int how);
extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void *self);
extern void _ZN7fBase_c18MarkForDestructionEv(void *self);
extern s16 data_02082214[];
extern s8 data_0209f2f8;
extern AnimData data_ov071_02123048;
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int a, int speed, unsigned int d);
extern void _ZN9Animation8SetFlagsEi(void *self, int flags);
extern s16 Vec3_VertAngle(const Vector3 *a, const Vector3 *b);
extern void _Z11UpdateAngleRssis(s16 *p, s16 tgt, int div, s16 maxStep);
extern int func_ov071_02120a20(char *c);
extern void func_ov071_021209c8(char *c);
extern int func_ov071_02120860(char *c);
extern int func_ov071_0212070c(char *c);
extern void func_ov071_02120b14(void *c);
extern void _Z14ApproachLinearRsss(short *, short, short);
extern void func_ov071_02120a48(char *c);
extern Item16 data_ov071_02123088[];
extern void func_ov071_021215fc(void *self);
void UnloadBlueCoinModel(void *context);
extern SharedFilePtr data_ov002_0210da38;
extern SharedFilePtr data_ov071_02123050;
extern SharedFilePtr *data_ov071_021226a4[2];
extern SharedFilePtr *data_ov071_021226a0;
extern int _ZN15TextureSequence6UpdateER15ModelComponents(void*, void*);
void func_ov071_021215c0(void *c);
void func_0200f760(void *c, void *p);
void func_ov071_02120c90(char *c);
}

// @symbol _ZN8daEykn_cD1Ev
// @symbol _ZN8daEykn_cD0Ev
daEykn_c::~daEykn_c()
{
}


// @symbol func_ov071_0212070c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov071_0212070c(char* c)
{
    int delta;
    int eq;
    int hi;
    int *p;
    int v;
    short *b200;

    b200 = (short *)(c + 0x200);
    delta = (short)(*(short *)(c + 0x8e) - b200[6]);
    eq = (int)(*(unsigned short *)(c + 0xc) == 0x107);
    if (eq != 0)
        hi = 0x190;
    else
        hi = 0x320;

    if (delta > hi) {
        v = *(int *)(c + 0x1f4);
        if (v >= 0) {
            p = (int *)(((int)c + 0x1f4));
            *p = *p + delta;
            *(unsigned char *)(c + 0x216) = 0x2e;
        } else {
            if (*(unsigned char *)(c + 0x216) == 0)
                *(int *)(c + 0x1f4) = 0;
            DecIfAbove0_Byte((unsigned char *)(((int)c + 0x216)));
        }
    } else if (delta < -hi) {
        v = *(int *)(c + 0x1f4);
        /* fallthrough ADD when v <= 0 (target: bgt to Dec) */
        if (v <= 0) {
            p = (int *)(((int)c + 0x1f4));
            *p = *p + delta;
            *(unsigned char *)(c + 0x216) = 0x2e;
        } else {
            if (*(unsigned char *)(c + 0x216) == 0)
                *(int *)(c + 0x1f4) = 0;
            DecIfAbove0_Byte((unsigned char *)(((int)c + 0x216)));
        }
    } else {
        if (*(unsigned char *)(c + 0x216) == 0)
            *(int *)(c + 0x1f4) = 0;
        DecIfAbove0_Byte((unsigned char *)(((int)c + 0x216)));
    }

    v = *(int *)(c + 0x1f4);
    if (v > 0x17fff || v < -0x17fff) {
        *(int *)(c + 0x1f4) = 0;
        *(unsigned char *)(c + 0x216) = 0x2e;
        return 1;
    }
    return 0;
}
}


// @symbol func_ov071_02120860
extern "C" {
struct E { int w[2]; };
extern E data_ov071_02123038;
extern E data_ov071_02123040;
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void* ts, void* file, int a, int d, unsigned e);
void _ZN9Animation8SetFlagsEi(void* anim, int flags);
int _ZN9Animation8FinishedEv(void* anim);
void _ZN9Animation7AdvanceEv(void* anim);

int func_ov071_02120860(char* c)
{
    unsigned char* st;
    switch (*(unsigned char*)(c + 0x214)) {
    case 0:
        return 0;
    case 1:
    case 4:
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x138, (void*)data_ov071_02123038.w[1], 0, 0x1000, 0);
        _ZN9Animation8SetFlagsEi(c + 0x138, 0x40000000);
        *(int*)(c + 0x144) = 0x1000;
        *(int*)(c + 0x140) = 0;
        st = (unsigned char*)(((int)c + 0x214));
        *st = *st + 1;
        /* fall through */
    case 2:
    case 5:
        if (_ZN9Animation8FinishedEv(c + 0x138) != 0) {
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x138, (void*)data_ov071_02123040.w[1], 0, 0x1000, 0);
            _ZN9Animation8SetFlagsEi(c + 0x138, 0x40000000);
            *(int*)(c + 0x144) = 0x1000;
            *(int*)(c + 0x140) = 0;
            st = (unsigned char*)(((int)c + 0x214));
            *st = *st + 1;
        }
        _ZN9Animation7AdvanceEv(c + 0x138);
        return 0;
    case 3:
    case 6:
        if (_ZN9Animation8FinishedEv(c + 0x138) != 0) {
            st = (unsigned char*)(((int)c + 0x214));
            *st = *st + 1;
        }
        _ZN9Animation7AdvanceEv(c + 0x138);
        return 0;
    case 7:
        *(unsigned char*)(c + 0x214) = 0;
        return 1;
    default:
        return 0;
    }
}
}


// @symbol func_ov071_021209c8
extern "C" {
void func_ov071_021209c8(char* c){
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c+0x138, (void*)data_ov071_02123038.w[1], 0, 0x1000, 0);
  _ZN9Animation8SetFlagsEi(c+0x138, 0x40000000);
  *(int*)(c+0x144)=0x1000;
  *(int*)(c+0x140)=0;
  *(char*)(c+0x214)=0;
}
}


// @symbol func_ov071_02120a20
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov071_02120a20(char *c)
{
    if (*(unsigned char *)(c + 0x214) == 0) {
        (*(volatile unsigned char *)(((int)c + 0x214)))++;
        return 1;
    }

    return 0;
}
}


// @symbol func_ov071_02120a48
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" void func_ov071_02120a48(char *c)
{
    char *p = _ZN8dActor_c22ClosestNonVanishPlayerEv(c);
    if (p == 0)
        return;
    if (Vec3_Dist(c + 0x5c, p + 0x5c) > 0x5dc000)
        return;
    if (AngleDiff(Vec3_HorzAngle(c + 0x5c, p + 0x5c), *(short *)(c + 0x8e)) > 0x190)
        return;
    int px = *(int *)(p + 0x5c);
    int pz = *(int *)(p + 0x64);
    int py = *(int *)(p + 0x60) + 0x8c000;
    Vector3 v;
    v.x = px;
    v.y = py;
    v.z = pz;
    if (_ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b(c, &v, (Vector3 *)(c + 0x5c), false) != 0)
        return;
    *(char **)(c + 0x1ec) = p;
    *(unsigned char *)(c + 0x216) = 0x2e;
    func_ov071_02121634(c, 1);
}


// @symbol func_ov071_02120b14
/* recovered: shared common types */
extern "C" void func_ov071_02120b14(void* self)
{
    u8* c = (u8*)self;

    void* egg = _ZN8dActor_c7FindEggER5dCc_c(self, (void*)(c+0x174));
    if (egg != 0) {
        int isEgg9 = (int)(*(u16*)((u8*)egg+0xc) == 9);
        if (isEgg9) goto playSound;
    }

    if (_ZN8dActor_c18FindExplosionActorER5dCc_c(self, (void*)(c+0x174)) == 0) goto idCheck;

playSound:
    _ZN5Sound9PlayBank0EjRK7Vector3(9, (void*)(c+0x74));
    func_ov071_02121634((char *)self, 2);
    return;

idCheck:

    if (*(u32*)(c+0x198) == 0) return;

    u8* f = (u8*)_ZN8dActor_c10FindWithIDEj(*(u32*)(c+0x198));
    if (f == 0) return;

    int isbf = (int)(*(u16*)(f+0xc) == 0xbf);
    if (!isbf) return;

    if (*(s32*)(c+0x194) & 0x40000) {
        void* p1 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(u32*)(c+0x204), 0x13a, *(s32*)(c+0x5c), *(s32*)(c+0x60), *(s32*)(c+0x64), 0, 0);
        *(void**)(c+0x204) = p1;

        u32 p2 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(*(u32*)(c+0x208), 0x13b, *(s32*)(c+0x5c), *(s32*)(c+0x60), *(s32*)(c+0x64), 0);
        *(u32*)(c+0x208) = p2;

        func_ov071_02121634((char *)self, 2);
        return;
    }

    struct Vector3 hv;
    hv.x = *(s32*)(c+0x5c);
    hv.y = *(s32*)(c+0x60);
    hv.z = *(s32*)(c+0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(f, &hv, 2, 0xc000, 1, 0, 1);
}


// @symbol func_ov071_02120c90
extern "C" {
void Matrix4x3_FromRotationXYZExt(void* m, int x, int y, int z);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* sm, void* mtx, int a, int b, unsigned int g);

void func_ov071_02120c90(char* c) {
  Matrix4x3_FromRotationXYZExt(c+0xf0, *(s16*)(c+0x8c), *(s16*)(c+0x8e), *(s16*)(c+0x90));
  *(int*)(c+0x114) = *(int*)(c+0x5c) >> 3;
  *(int*)(c+0x118) = *(int*)(c+0x60) >> 3;
  *(int*)(c+0x11c) = *(int*)(c+0x64) >> 3;
  *(int*)(c+0x1d8) = *(int*)(c+0x5c) >> 3;
  *(int*)(c+0x1dc) = *(int*)(c+0x60) >> 3;
  *(int*)(c+0x1e0) = *(int*)(c+0x64) >> 3;
  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c+0x14c, c+0x1b4, *(int*)(c+0x1f0) * 0xb4, *(int*)(c+0x200), 0xf);
}
}


// @symbol func_ov071_02120d30
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov071_02120d30(char *c)
{
    unsigned int id1;

    _Z14ApproachLinearRiii((int *)(c + 0x98), *(s16 *)(c + 0x20e), 300);
    *(s16 *)(c + 0x8e) = (s16)(*(s16 *)(c + 0x8e) + *(int *)(c + 0x98));
    *(int *)(c + 0x1f8) = *(int *)(c + 0x1f8) + *(int *)(c + 0x98);
    if (*(int *)(c + 0x1f8) / 131070 != 0) {
        func_0201267c(0x119, c + 0x74);
    }
    {
        int *spin = (int *)(c + 0x1f8);
        *spin %= 131070;
    }

    id1 = *(unsigned int *)(c + 0x204);
    if (id1 != 0 && *(unsigned int *)(c + 0x208) != 0) {
        void *p1;
        void *p2;
        *(unsigned int *)(c + 0x204) = (unsigned int)
            _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                id1, 0x13a, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64), 0, 0);
        *(unsigned int *)(c + 0x208) =
            _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
                *(unsigned int *)(c + 0x208), 0x13b, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64), 0);
        p1 = _ZN8Particle6System12FromUniqueIDEj(*(unsigned int *)(c + 0x204));
        p2 = _ZN8Particle6System12FromUniqueIDEj(*(unsigned int *)(c + 0x208));
        if (p1 != 0) {
            *(int *)((char *)p1 + 0x50) = 0x7fff;
        }
        if (p2 != 0) {
            *(int *)((char *)p2 + 0x50) = 0x7fff;
        }
    }

    switch ((unsigned char)c[0x214]) {
    case 0: {
        s16 sinv = data_02082214[((int)*(unsigned short *)(c + 0x210) >> 4) * 2];
        *(s16 *)(c + 0x8c) = (s16)((int)(((s64)*(int *)(c + 0x1fc) * sinv + 0x800) >> 12));
        *(s16 *)(c + 0x210) = (s16)(*(s16 *)(c + 0x210) + 0xe000);
        _Z14ApproachLinearRiii((int *)(c + 0x1fc), 0, 0x1b);
        if (DecIfAbove0_Byte((unsigned char *)c + 0x215) == 0) {
            unsigned char *st = (unsigned char *)c + 0x214;
            *st = (unsigned char)(*st + 1);
        }
        break;
    }
    case 1:
        _ZN9Animation7AdvanceEv(c + 0x124);
        if (_ZN9Animation8FinishedEv(c + 0x124) != 0) {
            unsigned char *st = (unsigned char *)c + 0x214;
            *st = (unsigned char)(*st + 1);
        }
        break;
    case 2: {
        int scale;
        unsigned short kind;
        int isBig;
        if (_Z14ApproachLinearRiii((int *)(c + 0x1f0), 0xa4, 0xa4) != 0) {
            unsigned char *st = (unsigned char *)c + 0x214;
            *st = (unsigned char)(*st + 1);
        }
        scale = *(int *)(c + 0x1f0);
        *(int *)(c + 0x80) = scale;
        *(int *)(c + 0x84) = scale;
        *(int *)(c + 0x88) = scale;
        kind = *(unsigned short *)(c + 0xc);
        isBig = (int)(kind == 0x106);
        if (isBig != 0) {
            *(int *)(c + 0x178) = *(int *)(c + 0x1f0) * 0x55;
        } else {
            int isSmall = (int)(kind == 0x107);
            if (isSmall != 0) {
                *(int *)(c + 0x178) = *(int *)(c + 0x1f0) * 0x55;
            }
        }
        break;
    }
    case 3: {
        unsigned short kind = *(unsigned short *)(c + 0xc);
        int isBig = (int)(kind == 0x106);
        if (isBig != 0) {
            int yadj, zcopy, y, z, x;
            Vector3 pos;
            y = *(int *)(c + 0x60);
            z = *(int *)(c + 0x64);
            yadj = 0x78000;
            yadj = y + yadj;
            zcopy = z;
            x = *(int *)(c + 0x5c);
            pos.x = x;
            pos.z = zcopy;
            pos.y = yadj;
            _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                0x122, 2, &pos, 0, *(signed char *)(c + 0xcc), -1);
            _ZN8dActor_c8PoofDustEv(c);
        } else {
            int isSmall = (int)(kind == 0x107);
            if (isSmall != 0) {
                unsigned char star = (unsigned char)(*(unsigned int *)(c + 8) & 0xf);
                _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h(
                    c, (signed char *)(c + 0x217), star, (Vector3 *)(c + 0x5c), 4);
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                    0x124, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                    0x125, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                    0x126, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
            }
        }
        func_0201267c(0xc4, c + 0x74);
        if (data_0209f2f8 == 0x2e) {
            _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
        } else {
            _ZN7fBase_c18MarkForDestructionEv(c);
        }
        break;
    }
    }
    return 1;
}
}


// @symbol func_ov071_0212110c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov071_0212110c(char *self)
{
    short diff = *(short*)(self + 0x8e) - *(short*)(self + 0x20c);
    *(int*)(self + 0x98) = diff;
    if (*(int*)(self + 0x98) > 0)
        *(short*)(self + 0x20e) = 0x2500;
    else
        *(short*)(self + 0x20e) = -0x2500;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0xd4, data_ov071_02123048.f4, 0, 0x1000, 0);
    _ZN9Animation8SetFlagsEi(self + 0x124, 0x40000000);
    *(int*)(self + 0x130) = 0x2800;
    *(int*)(self + 0x12c) = 0;
    *(unsigned char*)(self + 0x214) = 0;
    *(unsigned char*)(self + 0x215) = 0x2e;
    *(short*)(self + 0x210) = 0;
    func_0201267c(0x119, self + 0x74);
    *(int *)(((int)self + 0xb0)) &= ~1;
    *(int*)(self + 0x1e8) = 2;
    *(int*)(self + 0x1fc) = 0x500;
    return 1;
}
}


// @symbol func_ov071_021211e0
/* daEykn_c (ov071) tracking behaviour: turns toward the player, fires a bullet actor
 * (0x108) from its facing when the state byte reaches 7, and hands off to the
 * timeout/distance/blocked-by-terrain exits. The three target vectors are built
 * with the same load-then-assign shape: the second one must be spelled exactly
 * like the first or the pointer/y registers swap (the previous draft used
 * volatile field stores there and sat at div 7). */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov071_021211e0(char *c)
{
    Vector3_16 rot;
    Vector3 target1;
    Vector3 target2;
    Vector3 pos;
    Vector3 target3;
    s16 ang;

    {
        s32 *pl;
        s32 py;
        s32 pz;
        s32 yoff;

        pl = *(s32 **)(c + 0x1ec);
        py = pl[0x60 / 4];
        pz = pl[0x64 / 4];
        yoff = py + 0x78000;
        target1.x = pl[0x5c / 4];
        target1.y = yoff;
        target1.z = pz;
    }

    Vec3_HorzAngle((Vector3 *)(c + 0x5c), &target1);
    ang = Vec3_VertAngle((Vector3 *)(c + 0x5c), &target1);
    _Z11UpdateAngleRssis((s16 *)(c + 0x8c), ang, 2, 0x320);
    ang = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &target1);
    _Z11UpdateAngleRssis((s16 *)(c + 0x8e), ang, 2, 0x8fc);

    if ((s16)(*(s16 *)(c + 0x8e) - *(s16 *)(c + 0x20c)) == 0) {
        if (DecIfAbove0_Byte((u8 *)(c + 0x213)) == 0 && func_ov071_02120a20(c) != 0)
            *(u8 *)(c + 0x213) = 0x53;
        *(u8 *)(c + 0x212) = 0xf0;
    }

    if (*(u8 *)(c + 0x214) == 7) {
        s32 *pl2;
        s32 py2;
        s32 pz2;
        s32 yoff2;

        pl2 = *(s32 **)(c + 0x1ec);
        py2 = pl2[0x60 / 4];
        pz2 = pl2[0x64 / 4];
        yoff2 = py2 + 0x4b000;
        target2.x = pl2[0x5c / 4];
        target2.y = yoff2;
        target2.z = pz2;

        {
            s32 px;
            s32 pz2;
            s32 scale;
            s32 round;
            int idx;
            s16 s;
            int isBig;
            int param;

            px = *(s32 *)(c + 0x5c);
            scale = 0x50000;
            pos.x = px;
            pos.y = *(s32 *)(c + 0x60);
            pz2 = *(s32 *)(c + 0x64);
            round = 0x800;
            pos.z = pz2;

            {
                unsigned short rx = *(unsigned short *)(c + 0x8c);
                unsigned short ry = *(unsigned short *)(c + 0x8e);
                rot.y = ry;
                rot.x = rx;
                unsigned short rz = *(unsigned short *)(c + 0x90);
                rot.z = rz;
            }

            idx = *(u16 *)(c + 0x8e) >> 4;
            s = data_02082214[idx * 2];
            pos.x = px + (s32)(((s64)s * scale + round) >> 12);

            idx = *(u16 *)(c + 0x8e) >> 4;
            s = data_02082214[idx * 2 + 1];
            pos.z = pz2 + (s32)(((s64)s * scale + round) >> 12);

            rot.x = Vec3_VertAngle(&pos, &target2);

            isBig = (int)(*(u16 *)(c + 0xc) == 0x107);
            if (isBig != 0)
                param = 1;
            else
                param = 0;
            _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                0x108, param, &pos, &rot, *(s8 *)(c + 0xcc), -1);
            func_0201267c(0x165, c + 0x74);
            *(u8 *)(c + 0x216) = 0x2e;
            *(u8 *)(c + 0x212) = 0xf0;
            *(u8 *)(c + 0x213) = 0x53;
            *(s32 *)(c + 0x1f4) = 0;
            func_ov071_021209c8(c);
        }
    }

    func_ov071_02120860(c);

    {
        s32 *p3;
        s32 y3;
        s32 z3;
        s32 yoff3;

        p3 = *(s32 **)(c + 0x1ec);
        y3 = p3[0x60 / 4];
        z3 = p3[0x64 / 4];
        yoff3 = y3 + 0x8c000;
        target3.x = p3[0x5c / 4];
        target3.y = yoff3;
        target3.z = z3;
    }

    if (func_ov071_0212070c(c) != 0) {
        func_ov071_02121634(c, 2);
    } else if (Vec3_Dist((Vector3 *)(c + 0x5c), &target1) > 0x5dc000) {
        func_ov071_02121634(c, 0);
    } else if (*(u8 *)(*(char **)(c + 0x1ec) + 0x6fb) != 0) {
        func_ov071_02121634(c, 0);
    } else if (_ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b(c, &target3, (Vector3 *)(c + 0x5c), 0) != 0) {
        func_ov071_02121634(c, 0);
    }

    func_ov071_02120b14(c);
    return 1;
}
}


// @symbol func_ov071_021214f4
// recovered name: Scuttlebug_Kill
/* recovered: renamed to Class_Method */
/* daSpd_c::Kill - recovered from vtable slot identity */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov071_021214f4(char* c) {
    *(unsigned char*)(c+0x212) = 0xf0;
    *(unsigned char*)(c+0x213) = 0;
    *(int*)(c+0x1f4) = 0;
    *(int*)(c+0x1e8) = 1;
    func_ov071_021209c8(c);
    return 1;
}
}


// @symbol func_ov071_0212152c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov071_0212152c(void *c)
{
    _Z14ApproachLinearRsss((short *)((char *)c + 0x8c), 0, 0x320);
    *(short *)((char *)c + 0x8e) = *(short *)((char *)c + 0x8e) + *(int *)((char *)c + 0x98);
    func_ov071_02120a48((char *)c);
    func_ov071_02120b14(c);
    return 1;
}
}


// @symbol func_ov071_02121570
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov071_02121570(char* c) {
    short v = *(short*)(c+0x8e) - *(short*)(c+0x20c);
    if (v >= 0) *(int*)(c+0x98) = 0xc8;
    else *(int*)(c+0x98) = -0xc8;
    func_ov071_021209c8(c);
    *(int*)(c+0x1ec) = 0;
    *(int*)(c+0x1e8) = 0;
    return 1;
}
}


// @symbol func_ov071_021215c0
extern "C" void func_ov071_021215c0(void *raw) { C *c = (C *)raw; PMF *p = c->pp + 1; (c->**p)(); }


// @symbol func_ov071_021215fc
extern "C" void func_ov071_021215fc(void *raw) { C *c = (C *)raw; PMF *p = c->pp; (c->**p)(); }


// @symbol func_ov071_02121634
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov071_02121634(char *self, int idx)
{
    *(Item16 **)(self + 0x1e4) = &data_ov071_02123088[idx];
    func_ov071_021215fc(self);
}
}


// @symbol _ZN8daEykn_c16CleanupResourcesEv
s32 daEykn_c::CleanupResources()
{
    UnloadBlueCoinModel(this);
    data_ov002_0210da38.Release();
    data_ov071_02123050.Release();
    for (s32 i = 0; i < 2; ++i) {
        data_ov071_021226a4[i]->Release();
    }
    data_ov071_021226a0->Release();
    return 1;
}


// @symbol _ZN8daEykn_c16OnPendingDestroyEv
/* daEykn_c::OnPendingDestroy -- vtable slot 12. The ROM body is empty: the
 * override exists only to occupy the slot. */
void daEykn_c::OnPendingDestroy()
{
}


// @symbol _ZN8daEykn_c6RenderEv
/* recovered: named members + shared header, real C++ method */
int daEykn_c::Render()
{
  _ZN15TextureSequence6UpdateER15ModelComponents(((char*)this)+0x138, ((char*)this)+0xdc);
  ((Sub*)((char*)&mModelAnim))->g5((char*)&mScaleX);
  return 1;
}


// @symbol _ZN8daEykn_c8BehaviorEv
/* recovered: named members + shared header, real C++ method */
/* dCc_c comes from the real dCcAc_c chain now that daEykn_c.h types mdCcAcPos_c;
   the ad-hoc redeclaration that used to stand in for it ICEd mwccarm
   (CClass.c:3328) once the real class was visible. Clear and Update are
   non-virtual there, so the direct bl is unchanged. */
int daEykn_c::Behavior()
{
    func_ov071_021215c0(((char *)this));
    func_0200f760(((char *)this), ((char *)this) + 0x174);
    mTurnRefAngleY = mAngleY;
    ((dCc_c*)((char *)&mdCcAcPos_c))->Clear();
    ((dCc_c*)((char *)&mdCcAcPos_c))->Update();
    func_ov071_02120c90(((char *)this));
    return 1;
}
