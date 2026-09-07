//cpp
/* daOts_c -- the shared base of the three Bully variants, ov064 0x02115ee0..0x02116d1c.
 *
 * One translation unit, twenty-two functions, the way the cartridge's own build
 * had it. This replaces twenty-two one-function shards; their content is
 * unchanged except where several of them carried stand-in `ModelAnim` and
 * `dActor_c` structs that cannot coexist with include/daOts_c.h in a single TU,
 * and where four different spellings of the same two ROM symbols had to be
 * collapsed into one (see the extern block below).
 *
 * THIS TU OWNS THE CLASS VTABLE. CleanupResources (vtable slot 3) is the first
 * virtual daOts_c declares out of line -- the destructor is inline in the class
 * body on purpose, so that the three derived classes inline its vptr store --
 * which makes CleanupResources the key function and anchors _ZTV7daOts_c
 * (ov064 0x0211b768) here, along with the D1/D0 pair the table points at.
 *
 * The RTTI records do NOT live in this overlay. _ZTI7daOts_c and _ZTS7daOts_c
 * are vague-linkage symbols the linker kept in ov027 (0x021138bc and
 * 0x021138a4), next to daIDonketu_c's own pair; ov064's vtable header word at
 * 0x0211b764 relocates across to them. None of that data lies inside this
 * entry's licensed .text range, so dsd supplies the cartridge's own bytes and
 * production isolation discards the emitted duplicates; the manifest's
 * compiler_only_output block licenses each one at its measured ROM home.
 *
 * The shards for D1 and D0 each needed a forcing scaffold (`p->~daOts_c()` and
 * a stand-in `CleanupResources` returning 0). This file needs neither: it owns
 * the real key function, so the vtable emission drags both variants in by
 * itself.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm 2004/b56 emits one
 * .text section per function in reverse source order, so the highest-address
 * ROM function is written first. Do not reorder.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02115ee0  src/_ZN7daOts_cD1Ev.cpp
 *   [1] 0x02115f28  src/_ZN7daOts_cD0Ev.cpp
 *   [2] 0x02115f84  src/_ZN7daOts_c16OnAimedAtWithEggEv.cpp
 *   [3] 0x02115f98  src/func_ov064_02115f98.c
 *   [4] 0x02116110  src/func_ov064_02116110.cpp
 *   [5] 0x0211616c  src/func_ov064_0211616c.c
 *   [6] 0x02116220  src/func_ov064_02116220.cpp
 *   [7] 0x02116348  src/_ZN7daOts_c14PlayDeathSoundEv.cpp
 *   [8] 0x0211635c  src/_ZN7daOts_c17PlayShellHitSoundEv.cpp
 *   [9] 0x02116360  src/_ZN7daOts_c12PlayHitSoundEv.cpp
 *   [10] 0x02116374  src/_ZN7daOts_c13PlayStepSoundEv.cpp
 *   [11] 0x021163bc  src/_ZN7daOts_c16UpdateDeathStateEv.cpp
 *   [12] 0x021163c0  src/func_ov064_021163c0.cpp
 *   [13] 0x02116460  src/func_ov064_02116460.cpp
 *   [14] 0x02116560  src/func_ov064_02116560.cpp
 *   [15] 0x021165d4  src/_ZN7daOts_c14UpdateRunStateEv.cpp
 *   [16] 0x021165d8  src/func_ov064_021165d8.cpp
 *   [17] 0x021166f0  src/func_ov064_021166f0.c
 *   [18] 0x02116754  src/func_ov064_02116754.cpp
 *   [19] 0x02116bac  src/func_ov064_02116bac.cpp
 *   [20] 0x02116ca0  src/_ZN7daOts_c16CleanupResourcesEv.cpp
 *   [21] 0x02116cf0  src/_ZN7daOts_c6RenderEv.cpp
 */
/* daOts_c.h FIRST: it pulls common.h ahead of Model.h through dEnemyBase_c.h,
 * which is the Matrix4x3-spelling rule those headers record. */
#include "daOts_c.h"
#include "common.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"

/* Local stand-ins with no header of their own. */

/* The five-entry resource block mFileTable points at; only the Fix12i at +0x24
 * is proven, by this class's own OnAimedAtWithEgg. */
struct BullyResourceConfig {
    u8 pad_00[0x24];
    Fix12i eggAimSpeed;
};

/* Spelled and guarded exactly as include/Particle__System.h spells it, so this
 * TU agrees with that header if it is ever pulled in. Only ever used here as a
 * null pointer argument. */
#ifndef VECTOR3_16F_DEFINED
#define VECTOR3_16F_DEFINED
struct Vector3_16f { s16 x, y, z; };
#endif
struct Callback {};

/* ROM symbols with no header of their own. `extern` on every data declaration:
 * without it the block form defines rather than declares. */
extern "C" {
extern s16 data_02082214[];
extern void Vec3_Sub(int* out, int* a, int* b);
extern int Vec3_HorzLen(int* v);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned effect, int x, int y, int z);
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 id, u32 param, Fix12i x, Fix12i y, Fix12i z, const Vector3_16f* pos, struct Callback* cb);
extern void _ZN7fBase_c18MarkForDestructionEv(void* self);
extern void func_0201267c(u32 soundID, const Vector3 *pos);
extern int func_02037e38(unsigned int* p);
extern void Matrix4x3_FromRotationY(void* m, short angle);
short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);

/* ONE SPELLING FOR ONE SYMBOL. The shards carried four declarations of
 * _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj that differed only in parameter
 * spelling and return type; five of the six call sites drop the result and
 * func_ov064_02116560 tail-forwards it, so the `int` return is the spelling all
 * six agree with. It stays the mangled free function rather than
 * ModelAnim::SetAnim: the ROM name carries a by-value Fix12<int>, which mwccarm
 * passes differently at the call site (wall 6az). Same story for
 * _Z14ApproachLinearRsss, which arrived as a reference form, a pointer form and
 * a plain `ApproachLinear`. */
int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, BCA_File *f, int a, int b, unsigned int c);
int _Z14ApproachLinearRsss(short &cur, short tgt, short step);
int _Z14ApproachLinearRiii(int *dst, int target, int rate);

int _ZN8dActor_c13ClosestPlayerEv(char *self);
void _ZN8dActor_c9UpdatePosEP5dCc_c(void* self, void* c);
int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void* self, int fix12);
int _ZN8dActor_c15IsPlayerInRangeE5Fix12IiES1_S1_i(void*,int,int,int,int);
int _ZN8dActor_c15IsPlayerInRangeERK7Vector3i(dActor_c *t, const Vector3 *v, int i);
void* _ZN8dActor_c10FindWithIDEj(u32 id);
int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void* c, void* clsn, void* player);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* a, void* sm, void* mtx, int rad, int h, unsigned int x);
int _ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi(void* self, void* wm, int fix12, short s, int b1, int b2, int t);
void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void* self, void* wm, unsigned int j);
int _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(void* c, void* v, void* r4, s32 flag);
int _ZNK10dBgCh_Actr10IsOnGroundEv(void* self);
int _ZN6Player9IsOnShellEv(void* p);
int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const Vector3* v, u32 a, s32 f, u32 b, u32 c, u32 d);

void func_ov064_02115f98(char* a0, char* a1);
void func_ov064_02116220(char* c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- _ZN7daOts_c6RenderEv, 0x02116cf0, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c6RenderEv
/* recovered: named members + shared header, real C++ method -- vtable slot 9
 *
 * WAS _ZN5Bully6RenderEv, and misattributed: slot 9 holds 0x02116cf0 in daOts_c,
 * Bully AND daIDonketu_c. BigBully is the only one of the three that overrides
 * it (0x0211764c), which is exactly the pattern of an inherited method with one
 * child that replaces it.
 */
int daOts_c::Render()
{
    mModelAnim.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN7daOts_c16CleanupResourcesEv, 0x02116ca0, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method -- vtable slot 3
 *
 * WAS _ZN5Bully16CleanupResourcesEv, and that was a misattribution, not a
 * spelling choice: slot 3 holds 0x02116ca0 in daOts_c's table AND in all three
 * children's, so Bully does not override this -- it inherits it.
 *
 * THE KEY FUNCTION. It is the first virtual daOts_c declares out of line, so
 * this TU emits _ZTV7daOts_c and the destructor pair the table points at.
 *
 * Releases the five SharedFilePtrs the file table points at. mFileTable is the
 * base's field, which is the other half of the same evidence: all three
 * children declare it.
 */
int daOts_c::CleanupResources()
{
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+0)))->Release();
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+4)))->Release();
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+8)))->Release();
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+0xc)))->Release();
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+0x10)))->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov064_02116bac, 0x02116bac, size 0xf4 */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov064_02116bac(void* c){
  char* r6 = (char*)c;
  Matrix4x3_FromRotationY(r6+0x12c, *(short*)(r6+0x8e));
  *(int*)(r6+0x150) = *(int*)(r6+0x5c) >> 3;
  *(int*)(r6+0x154) = (*(int*)(r6+0x60) + *(int*)(r6+0x3ec)) >> 3;
  *(int*)(r6+0x158) = *(int*)(r6+0x64) >> 3;
  int d = *(int*)(r6+0x60) - *(int*)(r6+0x3f4);
  if(d <= 0x1000) d = 0x1000;
  int rad = (int)(((long long)d * 0x180 + 0x800) >> 12);
  int h = *(int*)(r6+0x3f0) - rad;
  if(h < 0xa000) h = 0xa000;
  Matrix4x3_FromRotationY(r6+0x3b4, *(short*)(r6+0x8e));
  *(int*)(r6+0x3d8) = *(int*)(r6+0x5c) >> 3;
  *(int*)(r6+0x3dc) = *(int*)(r6+0x60) >> 3;
  *(int*)(r6+0x3e0) = *(int*)(r6+0x64) >> 3;
  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(r6, r6+0x370, r6+0x3b4, h, d+0x28000, 0xf);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov064_02116754, 0x02116754, size 0x458 */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov064_02116754(char* c)
{
    void* r4;
    s32 r1;
    u32 id;

    if (*(s32*)(c + 0x398) == 2)
        return;
    id = *(u32*)(c + 0x360);
    if (id == 0)
        return;

    r4 = _ZN8dActor_c10FindWithIDEj(id);
    if (!r4)
        return;

    {
        int isBf = (int)(*(u16*)((char*)r4 + 0xc) == 0xbf);
        if (!isBf)
            return;
    }

    r1 = *(s32*)(c + 0x35c);
    if ((r1 & 0x7c0) || *(u8*)((char*)r4 + 0x6f9) != 0) {
        *(s16*)(c + 0x94) = *(s16*)((char*)r4 + 0x8e);
        if (*(s32*)((char*)r4 + 8) == 2)
            *(s32*)(c + 0x98) = 0x32000;
        else
            *(s32*)(c + 0x98) = 0x28000;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, *(BCA_File**)(*(char**)(*(char**)(c + 0x330) + 8) + 4), 0, 0x1000, 0);
        func_ov064_02115f98(c, (char*)r4);
        *(s32*)(c + 0x398) = 2;
        *(u8*)(c + 0x3f9) = 0;
        ((daOts_c*)c)->PlayHitSound();
        return;
    }

    if (r1 & 0x2000) {
        *(s16*)(c + 0x94) = *(s16*)((char*)r4 + 0x94);
        *(s32*)(c + 0x98) = 0x28000;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, *(BCA_File**)(*(char**)(*(char**)(c + 0x330) + 8) + 4), 0, 0x1000, 0);
        func_ov064_02115f98(c, (char*)r4);
        *(s32*)(c + 0x398) = 2;
        *(u8*)(c + 0x3f9) = 0;
        ((daOts_c*)c)->PlayHitSound();
        return;
    }

    {
        int isD8 = (int)(*(u16*)(c + 0xc) == 0xd8);
        if (!isD8 && (r1 & 0x10)) {
            s16 v[3];
            v[0] = 0x2000;
            v[1] = 0;
            v[2] = 0;
            _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, v, r4, *(s32*)(*(char**)(c + 0x330) + 0x24));
            ((daOts_c*)c)->PlayHitSound();
            return;
        }
    }

    if (r1 & 0x40000) {
        *(s16*)(c + 0x94) = *(s16*)((char*)r4 + 0x8e);
        *(s32*)(c + 0x98) = 0x39800;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, *(BCA_File**)(*(char**)(*(char**)(c + 0x330) + 8) + 4), 0, 0x1000, 0);
        func_ov064_02115f98(c, (char*)r4);
        *(s32*)(c + 0x398) = 2;
        *(u8*)(c + 0x3f9) = 0;
        ((daOts_c*)c)->PlayHitSound();
        return;
    }

    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(c, c + 0x33c, r4) != 0) {
        *(s16*)(c + 0x94) = *(s16*)((char*)r4 + 0x8e);
        if (*(s32*)((char*)r4 + 8) == 2)
            *(s32*)(c + 0x98) = 0x32000;
        else
            *(s32*)(c + 0x98) = 0x28000;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, *(BCA_File**)(*(char**)(*(char**)(c + 0x330) + 8) + 4), 0, 0x1000, 0);
        func_ov064_02115f98(c, (char*)r4);
        *(s32*)(c + 0x398) = 2;
        *(u8*)(c + 0x3f9) = 0;
        ((daOts_c*)c)->PlayHitSound();
        return;
    }

    if (_ZN6Player9IsOnShellEv(r4) != 0) {
        *(s32*)((char*)r4 + 0x98) = -*(s32*)(c + 0x98);
        *(s16*)(c + 0x94) = (s16)(*(s16*)(c + 0x8e) + 0x8000);
        *(s32*)(c + 0x98) = 0x28000;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, *(BCA_File**)(*(char**)(*(char**)(c + 0x330) + 0x10) + 4), 0, 0x1000, 0);
        ((daOts_c*)c)->PlayShellHitSound();
        func_ov064_02115f98(c, (char*)r4);
        *(s32*)(c + 0x398) = 2;
        *(u8*)(c + 0x3f9) = 0;
        ((daOts_c*)c)->PlayHitSound();
        return;
    }

    if (*(u8*)((char*)r4 + 0x6fb) != 0)
        return;

    *(s32*)(c + 0x398) = 2;
    *(u8*)(c + 0x3f9) = 0;
    ((daOts_c*)c)->PlayHitSound();

    {
        Vector3 v;
        v.x = *(s32*)(c + 0x5c);
        v.y = *(s32*)(c + 0x60);
        v.z = *(s32*)(c + 0x64);
        if (_ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, &v, 0, 0x14000, 1, 0, 1) != 0) {
            func_ov064_02115f98(c, (char*)r4);
        }
    }

    *(s16*)(c + 0x94) = (s16)(*(s16*)(c + 0x8e) + 0x8000);
    {
        int isD8 = (int)(*(u16*)(c + 0xc) == 0xd8);
        if (!isD8)
            *(s32*)(c + 0x98) = 0x14000;
        else
            *(s32*)(c + 0x98) = 0xa000;
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, *(BCA_File**)(*(char**)(*(char**)(c + 0x330) + 0x10) + 4), 0, 0x1000, 0);
    ((daOts_c*)c)->PlayShellHitSound();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov064_021166f0, 0x021166f0, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov064_021166f0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov064_021166f0(char *t)
{
    Vector3 v;
    *(short *)(t + 0x3e4) = Vec3_HorzAngle((Vector3 *)(t + 0x5c), (Vector3 *)(t + 0x3a8));
    _Z14ApproachLinearRsss(*(short *)(t + 0x94), *(short *)(t + 0x3e4), 0x140);
    v.x = *(int *)(t + 0x3a8);
    v.y = *(int *)(t + 0x3ac);
    v.z = *(int *)(t + 0x3b0);
    return _ZN8dActor_c15IsPlayerInRangeERK7Vector3i((dActor_c *)t, &v, 0x320);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov064_021165d8, 0x021165d8, size 0x118 */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov064_021165d8(unsigned char* c)
{
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, 0);

    if (_ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi(
            c, c + 0x174,
            *(int*)(*(int*)(c + 0x330) + 0x2c),
            0x2888, 0, 1, 0x32000) != 0
        && *(int*)(c + 0x398) != 2
        && *(int*)(c + 0x398) != 3) {
        *(int*)(c + 0x5c) = *(int*)(c + 0x39c);
        *(int*)(c + 0x64) = *(int*)(c + 0x3a4);
        *(int*)(c + 0x398) = 3;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            c + 0x110,
            *(BCA_File**)(*(int*)(*(int*)(c + 0x330) + 0xc) + 4),
            0, 0x1000, 0);
    }

    _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(c, c + 0x174, 0);

    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x174) != 0
        && _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(c, 0x5dc000) != 0) {
        *(int*)(((int)c + 0xb0)) |= 1;
    } else {
        *(int*)(((int)c + 0xb0)) &= ~1;
    }

    ((daOts_c*)c)->PlayStepSound();

    func_ov064_02116220((char*)c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- _ZN7daOts_c14UpdateRunStateEv, 0x021165d4, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c14UpdateRunStateEv
int daOts_c::UpdateRunState()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov064_02116560, 0x02116560, size 0x74 */
/* -------------------------------------------------------------------------- */
extern "C" int func_ov064_02116560(char* c){
  ((daOts_c*)c)->UpdateRunState();
  int r=_ZN8dActor_c15IsPlayerInRangeE5Fix12IiES1_S1_i(c,*(int*)(c+0x3a8),*(int*)(c+0x60),*(int*)(c+0x3b0),0x3e8);
  if(r) return r;
  *(int*)(c+0x398)=0;
  return _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x110, (BCA_File*)*(int*)(*(int*)(*(int*)(c+0x330)+0x10)+4), 0, 0x1000, 0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov064_02116460, 0x02116460, size 0x100 */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov064_02116460(char *self)
{
    if (_Z14ApproachLinearRiii((int *)(self + 0x98), 0, *(int *)(self + 0x3e8)) != 0) {
        int b = (*(unsigned short *)(self + 0xc) == 0xd8);
        int lim = b ? 0x14 : 0xa;
        if (*(unsigned char *)(self + 0x3f9) < 2) {
            if (((dBgCh_Actr *)(self + 0x174))->IsOnGround()) {
                unsigned char *p = (unsigned char *)(self + 0x3f9);
                *(int *)(self + 0xa8) = 0xf000;
                *p = *p + 1;
            }
            *(short *)(self + 0x100) = 0;
            return;
        }
        if (*(unsigned short *)(self + 0x100) < (unsigned int)lim)
            return;
        *(int *)(self + 0x398) = 1;
        *(short *)(self + 0x94) = *(short *)(self + 0x8e);
        {
            BCA_File *anim = (BCA_File *)*(int *)(*(int *)(*(int *)(self + 0x330) + 0x10) + 4);
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((ModelAnim *)(self + 0x110), anim, 0, 0x2000, 0);
        }
        return;
    }
    *(short *)(self + 0x100) = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov064_021163c0, 0x021163c0, size 0xa0 */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov064_021163c0(char *c)
{
    if (*(unsigned short *)(c + 0x100) == 0) {
        if (((dBgCh_Actr *)(c + 0x174))->IsOnGround()) {
            *(short *)(((int)c + 0x94)) = (short)((*(short *)(((int)c + 0x94))) + 0x8000);
        }
    }
    *(int *)(c + 0x98) = 0x5000;
    _Z14ApproachLinearRsss(*(short *)(c + 0x8e), *(short *)(c + 0x94), 0x200);
    if (*(unsigned short *)(c + 0x100) < 0xf)
        return;
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    *(int *)(c + 0x398) = 0;
    BCA_File *f = *(BCA_File **)(*(char **)(*(char **)(c + 0x330) + 0x10) + 4);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((ModelAnim *)(c + 0x110), f, 0, 0x1000, 0);
    return;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- _ZN7daOts_c16UpdateDeathStateEv, 0x021163bc, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c16UpdateDeathStateEv
void daOts_c::UpdateDeathState()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- _ZN7daOts_c13PlayStepSoundEv, 0x02116374, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c13PlayStepSoundEv
void daOts_c::PlayStepSound()
{
    if (mModelAnim.WillHitFrame(4) == 0) {
        if (mModelAnim.WillHitFrame(7) == 0)
            return;
    }
    func_0201267c(0xcc, (const Vector3 *)&mCamSpacePosX);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- _ZN7daOts_c12PlayHitSoundEv, 0x02116360, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c12PlayHitSoundEv
void daOts_c::PlayHitSound()
{
    func_0201267c(0xcd, (const Vector3 *)&mCamSpacePosX);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN7daOts_c17PlayShellHitSoundEv, 0x0211635c, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c17PlayShellHitSoundEv
void daOts_c::PlayShellHitSound()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN7daOts_c14PlayDeathSoundEv, 0x02116348, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c14PlayDeathSoundEv
void daOts_c::PlayDeathSound()
{
    func_0201267c(0xce, (const Vector3 *)&mCamSpacePosX);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov064_02116220, 0x02116220, size 0x128 */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov064_02116220(char* c){
  dBgCh_Gnd rg;
  Vector3 v;
  int y = *(int*)(c+0x60);
  int yoff = *(int*)(c+0x3ec);
  int z = *(int*)(c+0x64);
  int x = *(int*)(c+0x5c);
  int sum = y + yoff;
  int yv = sum + 0x96000;
  v.x = x;
  v.y = yv;
  v.z = z;

  rg.SetObjAndPos(v, (dActor_c*)c);
  if (rg.DetectClsn() != 0) {
    *(int*)(c+0x3f4) = rg.clsnY;
    if (*(int*)(c+0x60) <= rg.clsnY + 0x14000) {
      int r = func_02037e38((unsigned int*)&rg.surface);
      if (r == 4 || r == 5 || r == 0x13) {
        *(int*)(c+0x398) = 5;
      } else if (r == 1) {
        *(int*)(c+0x398) = 4;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x110,
            *(BCA_File**)(*(int*)(*(int*)(c+0x330) + 4) + 4), 0, 0x1000, 0);
        *(unsigned int *)(c + 0xb0) &= ~1u;
        ((daOts_c*)c)->PlayDeathSound();
        *(int*)(c+0x3a8) = *(int*)(c+0x5c);
        *(int*)(c+0x3ac) = *(int*)(c+0x60);
        *(int*)(c+0x3b0) = *(int*)(c+0x64);
        *(int*)(c+0x3ac) = rg.clsnY + 0x5000;
      }
    }
  }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov064_0211616c, 0x0211616c, size 0xb4 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov064_0211616c(char* c) {
    if (*(unsigned short*)(c + 0x100) > 0x1e) {
        _ZN7fBase_c18MarkForDestructionEv(c);
        return 1;
    }
    int *p = (int*)((int)c + 0x60);
    *p = *p - 0x5000;
    *(void**)(c + 0x334) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c + 0x334),
        *(u32*)(*(char**)(c + 0x330) + 0x30),
        *(Fix12i*)(c + 0x3a8),
        *(Fix12i*)(c + 0x3ac),
        *(Fix12i*)(c + 0x3b0),
        (const Vector3_16f*)0,
        (struct Callback*)0);
    *(void**)(c + 0x338) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c + 0x338),
        *(u32*)(*(char**)(c + 0x330) + 0x30) + 1,
        *(Fix12i*)(c + 0x3a8),
        *(Fix12i*)(c + 0x3ac),
        *(Fix12i*)(c + 0x3b0),
        (const Vector3_16f*)0,
        (struct Callback*)0);
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov064_02116110, 0x02116110, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov064_02116110
extern "C" int func_ov064_02116110(char *self, short step){
    char *p = (char*)(unsigned int)_ZN8dActor_c13ClosestPlayerEv(self);
    if(p != 0){
        short ang = Vec3_HorzAngle((const Vector3*)(self+0x5c),(const Vector3*)(p+0x5c));
        if(_Z14ApproachLinearRsss(*(short*)(self+0x94), ang, step)) return 1;
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov064_02115f98, 0x02115f98, size 0x178 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov064_02115f98(char* a0, char* a1)
{
    int scratch[3];
    int pos0[3];
    int pos1[3];
    int diff[3];
    int scale;
    int angH;
    int angV;
    int idxV, idxH;
    s16 sinV, cosV, sinH, cosH;
    int lenfix, ycomp, t0, t1;
    int y0;
    int* p1;
    int tmpy;

    p1 = (int*)(a1 + 0x5c);
    pos0[0] = *(int*)(a0 + 0x5c);
    y0 = *(int*)(a0 + 0x60);
    pos0[1] = y0;
    pos0[2] = *(int*)(a0 + 0x64);
    pos1[0] = *p1;
    tmpy = p1[1];
    pos1[1] = tmpy;
    pos1[2] = p1[2];
    tmpy = tmpy + 0x46000;

    scale = *(int*)(*(int*)(a0 + 0x330) + 0x14);
    pos0[1] = y0 + *(int*)(a0 + 0x3ec);
    pos1[1] = tmpy;

    Vec3_Sub(diff, pos1, pos0);

    scratch[0] = diff[0];
    scratch[1] = diff[1];
    scratch[2] = diff[2];
    angH = _ZN4cstd5atan2E5Fix12IiES1_(scratch[0], scratch[2]);
    {
        int hl = Vec3_HorzLen(scratch);
        angV = _ZN4cstd5atan2E5Fix12IiES1_(scratch[1], hl);
    }

    idxV = (int)((u16)angV >> 4);
    idxH = (int)((u16)angH >> 4);

    sinV = data_02082214[idxV * 2 + 1];
    cosV = data_02082214[idxV * 2];
    sinH = data_02082214[idxH * 2 + 1];
    cosH = data_02082214[idxH * 2];

    {
        s64 p = (s64)scale * sinV;
        lenfix = (int)((p + 0x800) >> 12);
    }
    {
        s64 p = (s64)scale * cosV;
        ycomp = (int)((p + 0x800) >> 12);
    }
    {
        s64 p = (s64)lenfix * cosH;
        t0 = (int)((p + 0x800) >> 12);
    }
    {
        s64 p = (s64)lenfix * sinH;
        t1 = (int)((p + 0x800) >> 12);
    }

    pos0[0] = pos0[0] + t0;
    pos0[1] = pos0[1] + ycomp;
    pos0[2] = pos0[2] + t1;

    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xf6, pos0[0], pos0[1], pos0[2]);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN7daOts_c16OnAimedAtWithEggEv, 0x02115f84, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c16OnAimedAtWithEggEv
int daOts_c::OnAimedAtWithEgg()
{
    BullyResourceConfig *config = (BullyResourceConfig *)mFileTable;
    Fix12i aimSpeed = 0x14000;
    if (config != 0)
        aimSpeed = config->eggAimSpeed;

    return aimSpeed;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN7daOts_cD0Ev, 0x02115f28, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_cD0Ev
/* recovered: real C++ deleting destructor, defined inline in the header
 *
 * ~daOts_c is defined in the class body -- the three classes derived from it
 * inline its vptr store rather than calling it, which the compiler can only do
 * from a visible body. So this file cannot define it, and the one-function
 * shard needed a stand-in CleanupResources to make the compiler emit the
 * vtable. Nothing forces D0 here: this TU defines the real CleanupResources
 * (slot 3), the class's key function, so the vtable is emitted here and drags
 * both destructor variants with it.
 *
 * D0 is the deleting half: destroy through daOts_c's four members and
 * dEnemyBase_c, then hand the object back through Memory::Deallocate.
 */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN7daOts_cD1Ev, 0x02115ee0, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_cD1Ev
/* recovered: real C++ destructor, defined inline in the header
 *
 * The body the key function forces out is the class's own layout evidence: the
 * ROM destroys a ShadowModel at 0x370, a dCcAc_c at 0x33c, a dBgCh_Actr at
 * 0x174 and a ModelAnim at 0x110, then chains to _ZN12dEnemyBase_cD2Ev, and
 * every one of those offsets is where the members' asserted sizes put them.
 * Nothing forces D1 here either -- see the D0 note above.
 */
