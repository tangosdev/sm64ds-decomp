//cpp
/* daOts_c -- shared base of the three Bully variants (BULLY 215 / BIG_BULLY 216
 * / CHILL_BULLY 217 (debug ICE_DONKETU)), ov064 0x02115ee0..0x02116d1c.
 *
 * ov064 is mixed (treasure chest, metal net lift, LLL tilting platform, Bowser
 * puzzle, rotating firebar, lava bubble, bully, water ring, jet stream, clam).
 * RTTI names this class daOts_c; overlay_actors maps the three children to
 * BULLY 215 / BIG_BULLY 216 / CHILL_BULLY 217 (debug ICE_DONKETU). Ugly RTTI
 * name is final.
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
 * No factory: the class is abstract (slots 0 and 6 are zero). Children own
 * daDonketu_c_classInit / daBDonketu_c_classInit / daIDonketu_c_classInit.
 *
 *
 * deslop leftovers:
 * - SetAnim 6az: helpers pass Fix12<int> by value; header method form size-DIFFs.
 * - DropShadowRadHeight 6az: func_ov064_02116bac.
 * - IsTooFarAwayFromPlayer / IsPlayerInRange(Fix12,Fix12,Fix12,s32) 6az:
 *   func_ov064_021165d8 / 02116560. Header methods carry Fix12<int>.
 * - KillByInvincibleChar 6az: func_ov064_02116754.
 * - Particle::System::New / NewSimple 6az: func_ov064_0211616c / 02115f98.
 * - ApproachLinear stays mangled (Rsss / Riii); no header method form here.
 * - func_0201267c: PlayStepSound / PlayHitSound / PlayDeathSound at mCamSpacePosX.
 * - SharedFilePtr +4: SetAnim BCA loads; SharedFilePtr.h has no fields.
 * - 0x398..0x3f9 stay children's padding (annexing would shrink Bully /
 *   BigBully / daIDonketu_c pads; out of this TU). Helpers reach them as
 *   offset soup.
 * - func_ov064_* helpers keep cartridge addresses (no identifiers).
 * - daOts_c.h first: nested Matrix4x3 for mModelAnim.mat4x3.t (02116bac).
 *   common.h's flat m[12] would stand down if it came first.
 * - Vec3_Sub / HorzLen / atan2 take int*; 02115f98 keeps the int-array pos copy.
 * - (long long)/(s64) 20.12 muls in 02116bac / 02115f98.
 * - bool-widening `int isD8 = (int)(actorID == 0xd8)` / `isBf` in 02116754 /
 *   02116460: the temporary is load-bearing.
 * - CleanupResources keeps `&mFileTable` pointer-arithmetic; cfg->files[i]->Release()
 *   size-DIFFs (addressing shape).
 * - func_ov064_021163c0 keeps `(int)c + 0x94` / c+0x174 / c+0x110; named
 *   mPrevAngleY / mWithMeshClsn / mHorzSpeed size-DIFFs.
 * - mStateTimer compares stay unsigned short (ldrh) in 0211616c / 02116460 /
 *   021163c0; named s16 > / < DIFFs.
 */
/* daOts_c.h FIRST: it pulls math/Matrix.h ahead of common.h through ModelAnim,
 * which is the nested Matrix4x3 spelling 02116bac needs for .t. */
#include "daOts_c.h"
#include "common.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"

/* Local stand-ins with no header of their own. */

/* The resource block mFileTable points at. CleanupResources releases files[0..4];
 * SetAnim loads the BCA at each SharedFilePtr +4; OnAimedAtWithEgg reads
 * eggAimHeight (slot-29 return, added to pos.y); 02115f98 / 021165d8 / 0211616c
 * read the rest. */
struct BullyResourceConfig {
    SharedFilePtr *files[5];    /* +0x00 */
    s32 knockbackScale;         /* +0x14 */
    u8 pad_18[0xc];
    Fix12i eggAimHeight;        /* +0x24 */
    s32 unk_28;
    s32 cliffDown;              /* +0x2c */
    u32 particleId;             /* +0x30 */
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

int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void* self, int fix12);
int _ZN8dActor_c15IsPlayerInRangeE5Fix12IiES1_S1_i(void*,int,int,int,int);
int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void* c, void* clsn, void* player);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* a, void* sm, void* mtx, int rad, int h, unsigned int x);
int _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(void* c, void* v, void* r4, s32 flag);
int _ZN6Player9IsOnShellEv(void* p);
int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const Vector3* v, u32 a, s32 f, u32 b, u32 c, u32 d);

void func_ov064_02115f98(daOts_c* a0, char* a1);
void func_ov064_02116220(daOts_c* c);
}

/* -------------------------------------------------------------------------- */
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
    /* cfg->files[i]->Release() size-DIFFs. &mFileTable pointer-arithmetic is
       the MATCH form (addressing shape). */
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+0)))->Release();
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+4)))->Release();
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+8)))->Release();
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+0xc)))->Release();
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+0x10)))->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov064_02116bac(daOts_c* self){
  char* r6 = (char*)self;
  Matrix4x3_FromRotationY(&self->mModelAnim.mat4x3, self->mAngleY);
  self->mModelAnim.mat4x3.t.x = self->mPosX >> 3;
  self->mModelAnim.mat4x3.t.y = (self->mPosY + *(int*)(r6+0x3ec)) >> 3;
  self->mModelAnim.mat4x3.t.z = self->mPosZ >> 3;
  int d = self->mPosY - *(int*)(r6+0x3f4);
  if(d <= 0x1000) d = 0x1000;
  int rad = (int)(((long long)d * 0x180 + 0x800) >> 12);
  int h = *(int*)(r6+0x3f0) - rad;
  if(h < 0xa000) h = 0xa000;
  Matrix4x3_FromRotationY(r6+0x3b4, self->mAngleY);
  *(int*)(r6+0x3d8) = self->mPosX >> 3;
  *(int*)(r6+0x3dc) = self->mPosY >> 3;
  *(int*)(r6+0x3e0) = self->mPosZ >> 3;
  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(self, &self->mShadowModel, r6+0x3b4, h, d+0x28000, 0xf);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov064_02116754(daOts_c* self)
{
    dActor_c* r4;
    s32 r1;
    u32 id;
    char* c = (char*)self;

    if (*(s32*)(c + 0x398) == 2)
        return;
    id = self->mdCcAc_c.otherOwner;
    if (id == 0)
        return;

    r4 = dActor_c::FindWithID(id);
    if (!r4)
        return;

    {
        int isBf = (int)(r4->actorID == 0xbf);
        if (!isBf)
            return;
    }

    r1 = (s32)self->mdCcAc_c.hitFlags;
    if ((r1 & 0x7c0) || *(u8*)((char*)r4 + 0x6f9) != 0) {
        self->mPrevAngleY = r4->mAngleY;
        if (r4->param1 == 2)
            self->mHorzSpeed = 0x32000;
        else
            self->mHorzSpeed = 0x28000;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, *(BCA_File**)((char*)((BullyResourceConfig *)self->mFileTable)->files[2] + 4), 0, 0x1000, 0);
        func_ov064_02115f98(self, (char*)r4);
        *(s32*)(c + 0x398) = 2;
        *(u8*)(c + 0x3f9) = 0;
        self->PlayHitSound();
        return;
    }

    if (r1 & 0x2000) {
        self->mPrevAngleY = r4->mPrevAngleY;
        self->mHorzSpeed = 0x28000;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, *(BCA_File**)((char*)((BullyResourceConfig *)self->mFileTable)->files[2] + 4), 0, 0x1000, 0);
        func_ov064_02115f98(self, (char*)r4);
        *(s32*)(c + 0x398) = 2;
        *(u8*)(c + 0x3f9) = 0;
        self->PlayHitSound();
        return;
    }

    {
        int isD8 = (int)(self->actorID == 0xd8);
        if (!isD8 && (r1 & 0x10)) {
            s16 v[3];
            v[0] = 0x2000;
            v[1] = 0;
            v[2] = 0;
            _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(self, v, r4, ((BullyResourceConfig *)self->mFileTable)->eggAimHeight);
            self->PlayHitSound();
            return;
        }
    }

    if (r1 & 0x40000) {
        self->mPrevAngleY = r4->mAngleY;
        self->mHorzSpeed = 0x39800;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, *(BCA_File**)((char*)((BullyResourceConfig *)self->mFileTable)->files[2] + 4), 0, 0x1000, 0);
        func_ov064_02115f98(self, (char*)r4);
        *(s32*)(c + 0x398) = 2;
        *(u8*)(c + 0x3f9) = 0;
        self->PlayHitSound();
        return;
    }

    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(self, &self->mdCcAc_c, r4) != 0) {
        self->mPrevAngleY = r4->mAngleY;
        if (r4->param1 == 2)
            self->mHorzSpeed = 0x32000;
        else
            self->mHorzSpeed = 0x28000;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, *(BCA_File**)((char*)((BullyResourceConfig *)self->mFileTable)->files[2] + 4), 0, 0x1000, 0);
        func_ov064_02115f98(self, (char*)r4);
        *(s32*)(c + 0x398) = 2;
        *(u8*)(c + 0x3f9) = 0;
        self->PlayHitSound();
        return;
    }

    if (_ZN6Player9IsOnShellEv(r4) != 0) {
        r4->mHorzSpeed = -self->mHorzSpeed;
        self->mPrevAngleY = (s16)(self->mAngleY + 0x8000);
        self->mHorzSpeed = 0x28000;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, *(BCA_File**)((char*)((BullyResourceConfig *)self->mFileTable)->files[4] + 4), 0, 0x1000, 0);
        self->PlayShellHitSound();
        func_ov064_02115f98(self, (char*)r4);
        *(s32*)(c + 0x398) = 2;
        *(u8*)(c + 0x3f9) = 0;
        self->PlayHitSound();
        return;
    }

    if (*(u8*)((char*)r4 + 0x6fb) != 0)
        return;

    *(s32*)(c + 0x398) = 2;
    *(u8*)(c + 0x3f9) = 0;
    self->PlayHitSound();

    {
        Vector3 v;
        v.x = self->mPosX;
        v.y = self->mPosY;
        v.z = self->mPosZ;
        if (_ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, &v, 0, 0x14000, 1, 0, 1) != 0) {
            func_ov064_02115f98(self, (char*)r4);
        }
    }

    self->mPrevAngleY = (s16)(self->mAngleY + 0x8000);
    {
        int isD8 = (int)(self->actorID == 0xd8);
        if (!isD8)
            self->mHorzSpeed = 0x14000;
        else
            self->mHorzSpeed = 0xa000;
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, *(BCA_File**)((char*)((BullyResourceConfig *)self->mFileTable)->files[4] + 4), 0, 0x1000, 0);
    self->PlayShellHitSound();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov064_021166f0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov064_021166f0(daOts_c *t)
{
    Vector3 v;
    char *c = (char *)t;
    *(short *)(c + 0x3e4) = Vec3_HorzAngle((Vector3 *)&t->mPosX, (Vector3 *)(c + 0x3a8));
    _Z14ApproachLinearRsss(t->mPrevAngleY, *(short *)(c + 0x3e4), 0x140);
    v.x = *(int *)(c + 0x3a8);
    v.y = *(int *)(c + 0x3ac);
    v.z = *(int *)(c + 0x3b0);
    return t->IsPlayerInRange(v, 0x320);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov064_021165d8(daOts_c* c)
{
    char *p = (char *)c;
    c->UpdatePos(0);

    if (c->IsGoingOffCliff(
            c->mWithMeshClsn,
            ((BullyResourceConfig *)c->mFileTable)->cliffDown,
            0x2888, 0, 1, 0x32000) != 0
        && *(int*)(p + 0x398) != 2
        && *(int*)(p + 0x398) != 3) {
        c->mPosX = *(int*)(p + 0x39c);
        c->mPosZ = *(int*)(p + 0x3a4);
        *(int*)(p + 0x398) = 3;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &c->mModelAnim,
            *(BCA_File**)((char*)((BullyResourceConfig *)c->mFileTable)->files[3] + 4),
            0, 0x1000, 0);
    }

    c->UpdateWMClsn(c->mWithMeshClsn, 0);

    if (c->mWithMeshClsn.IsOnGround() != 0
        && _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(c, 0x5dc000) != 0) {
        c->mFlags |= 1;
    } else {
        c->mFlags &= ~1;
    }

    c->PlayStepSound();

    func_ov064_02116220(c);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c14UpdateRunStateEv
int daOts_c::UpdateRunState()
{
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
extern "C" int func_ov064_02116560(daOts_c* c){
  char *p = (char *)c;
  c->UpdateRunState();
  int r=_ZN8dActor_c15IsPlayerInRangeE5Fix12IiES1_S1_i(c,*(int*)(p+0x3a8),c->mPosY,*(int*)(p+0x3b0),0x3e8);
  if(r) return r;
  *(int*)(p+0x398)=0;
  return _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, (BCA_File*)*(int*)((char*)((BullyResourceConfig *)c->mFileTable)->files[4] + 4), 0, 0x1000, 0);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov064_02116460(daOts_c *self)
{
    char *c = (char *)self;
    if (_Z14ApproachLinearRiii(&self->mHorzSpeed, 0, *(int *)(c + 0x3e8)) != 0) {
        int b = (self->actorID == 0xd8);
        int lim = b ? 0x14 : 0xa;
        if (*(unsigned char *)(c + 0x3f9) < 2) {
            if (self->mWithMeshClsn.IsOnGround()) {
                unsigned char *p = (unsigned char *)(c + 0x3f9);
                self->mVertSpeed = 0xf000;
                *p = *p + 1;
            }
            *(short *)(c + 0x100) = 0;
            return;
        }
        if (*(unsigned short *)(c + 0x100) < (unsigned int)lim)
            return;
        *(int *)(c + 0x398) = 1;
        self->mPrevAngleY = self->mAngleY;
        {
            BCA_File *anim = (BCA_File *)*(int *)((char *)((BullyResourceConfig *)self->mFileTable)->files[4] + 4);
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, anim, 0, 0x2000, 0);
        }
        return;
    }
    *(short *)(c + 0x100) = 0;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov064_021163c0(char *c)
{
    /* Named mPrevAngleY / mWithMeshClsn / mHorzSpeed size-DIFF. The
       `(int)c + 0x94` add and c+0x174 IsOnGround are the MATCH form. */
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
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c16UpdateDeathStateEv
void daOts_c::UpdateDeathState()
{
}

/* -------------------------------------------------------------------------- */
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
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c12PlayHitSoundEv
void daOts_c::PlayHitSound()
{
    func_0201267c(0xcd, (const Vector3 *)&mCamSpacePosX);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c17PlayShellHitSoundEv
void daOts_c::PlayShellHitSound()
{
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c14PlayDeathSoundEv
void daOts_c::PlayDeathSound()
{
    func_0201267c(0xce, (const Vector3 *)&mCamSpacePosX);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov064_02116220(daOts_c* c){
  dBgCh_Gnd rg;
  Vector3 v;
  char *p = (char *)c;
  int y = c->mPosY;
  int yoff = *(int*)(p+0x3ec);
  int z = c->mPosZ;
  int x = c->mPosX;
  int sum = y + yoff;
  int yv = sum + 0x96000;
  v.x = x;
  v.y = yv;
  v.z = z;

  rg.SetObjAndPos(v, c);
  if (rg.DetectClsn() != 0) {
    *(int*)(p+0x3f4) = rg.clsnY;
    if (c->mPosY <= rg.clsnY + 0x14000) {
      int r = func_02037e38((unsigned int*)&rg.surface);
      if (r == 4 || r == 5 || r == 0x13) {
        *(int*)(p+0x398) = 5;
      } else if (r == 1) {
        *(int*)(p+0x398) = 4;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim,
            *(BCA_File**)((char*)((BullyResourceConfig *)c->mFileTable)->files[1] + 4), 0, 0x1000, 0);
        c->mFlags &= ~1u;
        c->PlayDeathSound();
        *(int*)(p+0x3a8) = c->mPosX;
        *(int*)(p+0x3ac) = c->mPosY;
        *(int*)(p+0x3b0) = c->mPosZ;
        *(int*)(p+0x3ac) = rg.clsnY + 0x5000;
      }
    }
  }
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov064_0211616c(daOts_c* c) {
    char *p = (char *)c;
    /* mStateTimer is s16; the ROM compares ldrh (unsigned). Named signed > DIFFs. */
    if (*(unsigned short *)((char *)c + 0x100) > 0x1e) {
        c->MarkForDestruction();
        return 1;
    }
    c->mPosY = c->mPosY - 0x5000;
    c->mParticle0 = (u32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        c->mParticle0,
        ((BullyResourceConfig *)c->mFileTable)->particleId,
        *(Fix12i*)(p + 0x3a8),
        *(Fix12i*)(p + 0x3ac),
        *(Fix12i*)(p + 0x3b0),
        (const Vector3_16f*)0,
        (struct Callback*)0);
    c->mParticle1 = (u32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        c->mParticle1,
        ((BullyResourceConfig *)c->mFileTable)->particleId + 1,
        *(Fix12i*)(p + 0x3a8),
        *(Fix12i*)(p + 0x3ac),
        *(Fix12i*)(p + 0x3b0),
        (const Vector3_16f*)0,
        (struct Callback*)0);
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov064_02116110
extern "C" int func_ov064_02116110(daOts_c *self, short step){
    dActor_c *p = (dActor_c *)self->ClosestPlayer();
    if(p != 0){
        short ang = Vec3_HorzAngle((const Vector3*)&self->mPosX,(const Vector3*)&p->mPosX);
        if(_Z14ApproachLinearRsss(self->mPrevAngleY, ang, step)) return 1;
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov064_02115f98(daOts_c* a0, char* a1)
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
    char *c = (char *)a0;

    p1 = (int*)&((dActor_c *)a1)->mPosX;
    pos0[0] = a0->mPosX;
    y0 = a0->mPosY;
    pos0[1] = y0;
    pos0[2] = a0->mPosZ;
    pos1[0] = *p1;
    tmpy = p1[1];
    pos1[1] = tmpy;
    pos1[2] = p1[2];
    tmpy = tmpy + 0x46000;

    scale = ((BullyResourceConfig *)a0->mFileTable)->knockbackScale;
    pos0[1] = y0 + *(int*)(c + 0x3ec);
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
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c16OnAimedAtWithEggEv
int daOts_c::OnAimedAtWithEgg()
{
    BullyResourceConfig *config = (BullyResourceConfig *)mFileTable;
    Fix12i aimHeight = 0x14000;
    if (config != 0)
        aimHeight = config->eggAimHeight;

    return aimHeight;
}

/* -------------------------------------------------------------------------- */
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
