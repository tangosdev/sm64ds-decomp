//cpp
/* Production translation unit for ov096/daSanbo_c -- the Pokey.
 *
 * SM64DS RTTI names this class daSanbo_c. ov096 0x02137980 holds the
 * null-terminated string '9daSanbo_c'; _ZTI9daSanbo_c at 0x0213798c
 * points its name word back at that string and its base word at
 * _ZTI8dActor_c in arm9; the vtable object
 * at 0x021379d0 points its typeinfo word at the _ZTI. The tree used to carry
 * the class under the coined English name Pokey, which appears nowhere in the
 * cartridge. This file and include/daSanbo_c.h use the cartridge's spelling,
 * and Pokey survives only as prose. That rename is not cosmetic here: a coined
 * name emits '5Pokey' and its terminator (seven bytes total), which matches no
 * configured ROM record, so the RTTI group would have been discarded
 * unverified.
 *
 * The base is dActor_c, read from _ZTI9daSanbo_c+8 -> arm9 0x0208e390, and
 * _ZTV9daSanbo_c is 31 slots, the dActor_c length. daSanbo_c declares no new
 * virtual; it overrides ten inherited slots -- 0 InitResources, 3
 * CleanupResources, 6 Behavior, 9 Render, 12 OnPendingDestroy, 16 D1, 17 D0,
 * 18 OnYoshiTryEat, 19 OnTurnIntoEgg and 29 OnAimedAtWithEgg -- and every one
 * of them lies inside this run. The other 21 slots hold dActor_c's own words.
 *
 * THE DESTRUCTOR IS INLINE IN THE HEADER. The cartridge puts D1 at 0x02135700
 * above D0 at 0x02135748 and carries no D2 anywhere, which is what mwccarm
 * 2004/b56 emits for an inline in-class destructor; the out-of-line form emits
 * D2/D0/D1 and adds a homeless D2. Both bodies below are therefore the
 * compiler's own, written from the empty body in include/daSanbo_c.h and the
 * typed member list beside it, which is what owns the Model, ShadowModel,
 * dCcAc_c and dBgCh_Actr teardowns and the chain into _ZN8dActor_cD2Ev.
 *
 * mwccarm emits one .text section per function in the REVERSE of source order,
 * so this file is written ROM-DESCENDING: daSanbo_c_classInit_SANBO
 * (0x02136d60) first and the destructor pair last, at the bottom of the run,
 * which is where the cartridge has them. Do not reorder.
 *
 * The promoted run is 36 functions, 0x02135700..0x02136db0. Before the fold,
 * tools/tu_map.py reported 34 and cut a boundary at 0x02136d10, because it
 * segmented on symbol name and
 * neither classInit spelling looks like a member; the two functions above that
 * cut are the ACTOR_SPAWN_TABLE factories for SANBO and SANBO_BODY, and both
 * allocate this class's 0x3b0 bytes and store this class's vptr, so they are
 * included in this reconstruction. The delink span runs without a gap from 0x02135700 to
 * 0x02136db0, where daTor_c's own run begins.
 *
 * A daSanbo_c is not a container of segments. SANBO and SANBO_BODY construct
 * the SAME class and the head/segment split is a runtime test on the fBase_c
 * actor id; mPrevSegment and mNextSegment link the separate instances. There is no
 * out-of-line constructor anywhere in the image -- both factories inline it.
 *
 * common.h is included FIRST on purpose. daSanbo_c.h reaches math/Matrix.h,
 * which spells Matrix4x3 structurally, where common.h spells it flat. With the
 * two swapped, InitResources scalarises its trailing whole-struct Matrix4x3
 * copy and comes out 0x23c bytes against the cartridge's 0x220. The order
 * below is load-bearing; it is not stylistic.
 *
 * The state table remains outside this TU. __sinit_ov096_0213770c copies
 * twelve constant PMF records from .data 0x02137920..0x02137980 into
 * .bss 0x02137b48..0x02137ba8, reordering them into six entry/update pairs.
 * func_ov096_02136928 selects a pair at 0x02137b48 + state * 16 and stores
 * its address at this+0x384. The dispatchers use that runtime pair, not the
 * constants in address order. The existing initializer remains a separate
 * source owner; this text-only promotion does not reconstruct its table.
 *
 * ov096 delinks no .data at all, so this is a text-only entry: it licenses one
 * .text span and nothing else. Owning every virtual makes mwcc emit the vtable
 * and the whole RTTI group regardless of declaration order, and each of those
 * records is licensed away against its configured ROM home in
 * config/tu_manifest.d/ov096/daSanbo_c.json, which is also where the
 * function-by-function verification evidence lives. The full class dossier --
 * vtable extent proof, base-class evidence, destructor disassembly, field
 * provenance -- is notes/data/class-facts/daSanbo_c.json.
 */

/* ORDER IS LOAD-BEARING -- see the Matrix4x3 note above. */
#include "common.h"
#include "daSanbo_c.h"
#include "types.h"
#include "SharedFilePtr.h"
#include "decl_common.h"
#include "Player.h"

/* Declarations retained from the individually matched fragments. These ABI
 * bridges and raw helper layouts remain reconstruction work; their presence
 * does not imply that the corresponding class interfaces are unavailable. */
/* shadow namespace 'cstd' */
namespace cstd { int fdiv(int,int); }

/* shadow struct 'Mtx43' */
struct Mtx43 { Fix12i a[12]; };

/* shadow enum 'Bool' */
enum Bool { FALSE, TRUE };

extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned size);
extern void _ZN8dActor_cC2Ev(void *self);
extern void _ZN5ModelC1Ev(void *self);
extern void _ZN11ShadowModelC1Ev(void *self);
extern void _ZN7dCcAc_cC1Ev(void *self);
extern void _ZN10dBgCh_ActrC1Ev(void *self);
extern "C" void Vec3_Asr(struct Vector3* d, struct Vector3* s, int sh);
extern "C" void Matrix4x3_FromTranslation(struct Mtx43* m, Fix12i x, Fix12i y, Fix12i z);
extern "C" void Matrix4x3_ApplyInPlaceToTranslation(void* m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationZXYExt(void* m, int x, int y, int z);
extern "C" void Matrix4x3_FromRotationY(void* m, int angle);
extern "C" void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j( void* self, void* sm, void* mtx, Fix12i fx, int t, u32 u);
extern struct Mtx43 data_020a0e68;
extern void* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int, unsigned int, const void*, const void*, int, int);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, void*);
extern void func_ov096_02135800(char* c);
extern short Vec3_HorzAngle(const void* a, const void* b);
extern int RandomIntInternal(void*);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);
extern int data_0209e650[];
extern s16 data_02082214[];
extern void func_ov096_02135948(char* c);
extern void _ZN5dCc_c5ClearEv(char* p);
extern void _ZN5dCc_c6UpdateEv(char* p);
extern void func_ov096_0213585c(void *t);
extern char *func_ov096_021357b4(char *c);
extern char data_ov096_02137b48;
extern void func_ov096_021368f0(char *c);
extern int Vec3_HorzDist(const void* a, const void* b);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void func_02038414(void *p);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void *p);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *c, void *clsn);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *p);
void UnloadBlueCoinModel(void *);
extern int data_ov096_02137b20[];
extern int data_ov096_02137b28[];
extern int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void *c, int d);
void LoadBlueCoinModel(void* actor);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, void* actor, int r, int h, unsigned int d, unsigned int e);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, void* actor, int b, int c, void* v, int e);
void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern Matrix4x3 IDENTITY_MATRIX4X3;
}

/* The two ACTOR_SPAWN_TABLE factories are grouped with this class because
 * both store its vptr, both allocate its 0x3b0 bytes, and both
 * inline the same constructor body byte for byte apart from their literal
 * pools. There is no out-of-line daSanbo_c constructor anywhere in the image.
 *
 * A daSanbo_c is not a container of segments: SANBO (spawn-table index 0xf0)
 * and SANBO_BODY (index 0xf1) construct the SAME class, and the head/segment
 * split is a runtime test on the fBase_c actor id at [this+0x0c]. mPrevSegment and
 * mNextSegment link the separate instances.
 *
 * Reconstructed source-style names: SM64DS proves the RTTI class daSanbo_c, the
 * SANBO / SANBO_BODY registry IDs, the allocation size and the vtable identity;
 * later EAD lineage supplies the classInit spelling. The exact original SM64DS
 * symbols are not preserved. Historical project aliases: Pokey_Spawn and
 * PokeySegment_Spawn. */

/* THE `[2]` IS LOAD-BEARING, and it is what consolidation changed. Each shard
 * wrote the bare `(int)_ZTV9daSanbo_c` and that was right there: the symbol was
 * UNDEF and config/arm9/overlays/ov096/symbols.txt binds it to the address
 * point at 0x021379d8. Here the same spelling binds to this TU's own
 * definition, and mwcc's symbol addresses the vtable OBJECT at 0x021379d0 --
 * the offset-to-top and typeinfo words lower. `[2]` on an int[] is exactly that
 * eight-byte bias, so the compiler computes it and no addend is hand-edited; it
 * also makes these stores agree with the addend-8 vptr store the compiler emits
 * in the destructor itself. Raw match.compare wildcards relocated words;
 * whole-TU verification also checks relocation types, addends and destinations.
 * Linked-byte verification is required in addition to a masked byte match.
 *
 * The declaration is include/decl_common.h's, which this file already includes.
 * It is deliberately not restated here: an `extern _ZTV` array in a source file
 * is the hand-rolled-vptr idiom the langmode ratchet counts, and this class is
 * a real polymorphic type whose vtable this TU emits, so the count would be
 * measuring nothing. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- daSanbo_c_classInit_SANBO, 0x02136d60, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol daSanbo_c_classInit_SANBO
extern "C" int *daSanbo_c_classInit_SANBO(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(944);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)&_ZTV9daSanbo_c[2];
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN11ShadowModelC1Ev((char *)p + 0x124);
        _ZN7dCcAc_cC1Ev((char *)p + 0x14c);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x180);
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- daSanbo_c_classInit_SANBO_BODY, 0x02136d10, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol daSanbo_c_classInit_SANBO_BODY
extern "C" int *daSanbo_c_classInit_SANBO_BODY(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(944);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)&_ZTV9daSanbo_c[2];
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN11ShadowModelC1Ev((char *)p + 0x124);
        _ZN7dCcAc_cC1Ev((char *)p + 0x14c);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x180);
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- _ZN9daSanbo_c13OnTurnIntoEggER6Player, 0x02136cd0, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSanbo_c13OnTurnIntoEggER6Player
/* daSanbo_c::OnTurnIntoEgg -- vtable slot 19, recovered from vtable slot identity.
 * Only the head segment (actorID 0xf0) pays out a coin; every segment marks
 * itself for destruction. */
void daSanbo_c::OnTurnIntoEgg(Player &player)
{
    int flag = (actorID == 0xf0);
    if (flag)
        GivePlayerCoins(player, 1, 2);
    MarkForDestruction();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- _ZN9daSanbo_c13InitResourcesEv, 0x02136ab0, size 0x220 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSanbo_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
int daSanbo_c::InitResources()
{
    int t;

    t = (actorID == 0xf0);
    if (t != false) {
        void* m = Model::LoadFile(*(SharedFilePtr *)data_ov096_02137b20);
        mModel.SetFile((BMD_File *)m, 1, 1);
        Model::LoadFile(*(SharedFilePtr *)data_ov096_02137b28);
        LoadBlueCoinModel(((char*)this));
        unk_3a8 = 1;
    } else {
        t = (actorID == 0xf1);
        if (t != false) {
            void* m = Model::LoadFile(*(SharedFilePtr *)data_ov096_02137b28);
            if (mModel.SetFile((BMD_File *)m, 1, 1) == 0)
                return 0;
        }
    }

    if (mShadowModel.InitCylinder() == 0)
        return 0;

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this) + 0x14c, ((char*)this), 0x3c000, 0x78000, 0x200004, 0x6eff0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x180, ((char*)this), 0x3c000, 0x3c000, 0, 0);

    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = 0x3000;

    t = (actorID == 0xf0);
    if (t != false) {
        mRootPosX = mPosX;
        mRootPosY = mPosY;
        mRootPosZ = mPosZ;
        mScaleX = 0x1000;
        mScaleY = 0x1000;
        mScaleZ = 0x1000;
        mPrevSegment = 0;
        mNextSegment = 0;
    } else {
        t = (actorID == 0xf1);
        if (t != false) {
            mScaleX = 0;
            mScaleY = 0;
            mScaleZ = 0;
            mPrevSegment = (daSanbo_c *)dActor_c::FindWithID(param1);
            mNextSegment = 0;
            {
                int *p = &mPrevSegment->mRootPosX;
                mRootPosX = p[0];
                mRootPosY = p[1];
                mRootPosZ = p[2];
            }
        }
    }

    func_ov096_02136928(((char*)this), 1);
    mMatrix = IDENTITY_MATRIX4X3;
    func_ov096_02135efc(((char*)this));
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- _ZN9daSanbo_c8BehaviorEv, 0x02136a50, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSanbo_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daSanbo_c::Behavior()
{
    int s = mState;
    if (s != 2 && s != 5) {
        if (_ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(((char *)this), 0x5dc000)) return 1;
    }
    func_ov096_021368b4(((char *)this));
    MakeVanishLuigiWork(mdCcAc_c);
    func_ov096_02135efc(((char *)this));
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- _ZN9daSanbo_c6RenderEv, 0x021369fc, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSanbo_c6RenderEv
/* recovered: named members + shared header, real C++ method */
int daSanbo_c::Render()
{
    unsigned int f = mFlags;
    int b = ((f & 0x40000) != 0);
    if(b) return 1;
    Model *model = &mModel;
    model->Render((const Vector3 *)&mScaleX);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- _ZN9daSanbo_c16OnPendingDestroyEv, 0x021369b0, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSanbo_c16OnPendingDestroyEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
void daSanbo_c::OnPendingDestroy()
{
    int r1 = *(unsigned short *)((char *)&actorID);
    r1 = (r1 == 0xf1);
    if (r1) return;
    daSanbo_c *p = mNextSegment;
    if (!p) return;
    do {
        func_ov096_0213585c(p);
        p = p->mNextSegment;
    } while (p);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- _ZN9daSanbo_c16CleanupResourcesEv, 0x02136944, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSanbo_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
int daSanbo_c::CleanupResources()
{
  int id = actorID;
  int a = (id == 0xf0);
  if (a) {
    UnloadBlueCoinModel(((char *)this));
    ((SharedFilePtr *)(data_ov096_02137b20))->Release();
    ((SharedFilePtr *)(data_ov096_02137b28))->Release();
  } else {
    a = (id == 0xf1);
    if (a) {
      ((SharedFilePtr *)(data_ov096_02137b28))->Release();
    }
  }
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov096_02136928, 0x02136928, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_02136928
extern "C" {
void func_ov096_02136928(void *cc, int a) {
    daSanbo_c *c = (daSanbo_c *)cc;
    c->mStateFunctions = (daSanbo_c::StateFunc *)(&data_ov096_02137b48 + (a << 4));
    func_ov096_021368f0((char *)c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov096_021368f0, 0x021368f0, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_021368f0
extern "C" void func_ov096_021368f0(char *cc)
{
    daSanbo_c *c = (daSanbo_c *)cc;
    daSanbo_c::StateFunc *p = c->mStateFunctions;
    (c->**p)();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov096_021368b4, 0x021368b4, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_021368b4
extern "C" void func_ov096_021368b4(void *cc)
{
    daSanbo_c *c = (daSanbo_c *)cc;
    daSanbo_c::StateFunc *p = c->mStateFunctions + 1;
    (c->**p)();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov096_021368a4, 0x021368a4, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_021368a4
extern "C" {
int func_ov096_021368a4(int *p)
{
    p[227] = 0; return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov096_02136754, 0x02136754, size 0x150 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_02136754
extern "C" {
int func_ov096_02136754(char* self)
{
    char *other = *(char**)(self + 0x394);
    if (!other) {
        func_ov096_02136928(self, 1);
    } else {

        char *p = self + 0x60;
        int sp, t, a0, z, lim, b;
        s16 *tbl, c, sn;
        u16 ang;

        *(int*)(self+0x5c) = *(int*)(other+0x5c);
        other = *(char**)(self+0x394);
        t = *(int*)(other+0x64);
        sp = 0xe000;
        *(int*)(self+0x64) = t;

        t = *(int*)(self+0xa8) - 0x2000;
        a0 = *(int*)(self+0xa0);
        if (t >= a0) a0 = t;
        *(int*)(self+0xa8) = a0;
        *(int*)p += *(int*)(self+0xa8);
        other = *(char**)(self+0x394);
        t = *(int*)(self+0x60);
        lim = *(int*)(other+0x60) + 0x6e000;
        if (t < lim) {
            *(int*)(self+0x60) = lim;
            *(int*)(self+0xa8) = 0;
        }
        other = *(char**)(self+0x394);
        tbl = data_02082214;
        c = *(s16*)(other+0x8e);
        z = 0;
        *(s16*)(self+0x8e) = c;
        other = *(char**)(self+0x394);
        other = other + 0x300;
        c = *(s16*)(other+0xaa);
        b = 0x800;
        c = (s16)(c + 0x13000);
        *(s16*)(self + 0x3aa) = c;
        ang = *(u16*)(self + 0x3aa);
        c = tbl[(ang>>4)*2];
        *(int*)(self+0x378) = (int)(((s64)c * sp + b) >> 12);
        *(int*)(self+0x37c) = z;
        ang = *(u16*)(self + 0x3aa);
        sn = tbl[(ang>>4)*2+1];
        *(int*)(self+0x380) = (int)(((s64)sn * sp + b) >> 12);

    }
    func_ov096_02135948(self);
    _ZN5dCc_c5ClearEv(self + 0x14c);
    _ZN5dCc_c6UpdateEv(self + 0x14c);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov096_0213670c, 0x0213670c, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_0213670c
extern "C" {
void func_ov096_0213670c(void *c) {
    s32 r4 = (s32)c;
    void *ret = func_ov096_021357b4((char *)c);
    u8 r0 = *(u8*)((char*)ret + 0x3a8);
    if (r0 != 0) r0 = 0; else r0 = 0x5a;
    *(u8*)((char*)(s32)r4 + 0x3ac) = r0;
    r0 = *(u8*)((char*)(s32)r4 + 0x3ac);
    if (r0 == 0) {
        *(s32*)((char*)(s32)r4 + 0x80) = 0x1000;
        *(s32*)((char*)(s32)r4 + 0x84) = 0x1000;
        *(s32*)((char*)(s32)r4 + 0x88) = 0x1000;
    }
    *(s32*)((char*)(s32)r4 + 0x38c) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov096_021365d4, 0x021365d4, size 0x138 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_021365d4
/* recovered: shared common types */
extern "C" {
void _Z14ApproachLinearRiii(int* dst, int target, int step);
void func_ov096_021358c8(char* c);
int func_ov096_02135838(char* c);
void func_ov096_02135e2c(int* self, void* clsn);
void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int a, int b, int c);

int func_ov096_021365d4(char* c) {
    int t = *(int*)(c + 0x80);
    int v;
    _Z14ApproachLinearRiii(&t, 0x1000, 0x12c);
    v = t;
    *(int*)(c + 0x80) = v;
    *(int*)(c + 0x84) = v;
    *(int*)(c + 0x88) = v;
    *(short*)(c + 0x3aa) = *(short*)(c + 0x3aa) + 0x500;
    *(int*)(c + 0x378) = 0;
    *(int*)(c + 0x37c) = 0;
    *(int*)(c + 0x380) = 0;
    func_ov096_021358c8(c);
    if (func_ov096_02135838(c) < 4) {
        if (DecIfAbove0_Byte((unsigned char*)(c + 0x3ac)) == 0) {
            void* sp = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                0xf1, *(unsigned int*)(c + 4), (struct Vector3*)(c + 0x5c), c + 0x8c,
                *(signed char*)(c + 0xcc), -1);
            if (sp != 0) {
                *(void**)(c + 0x394) = sp;
                func_ov096_02136928(c, 4);
                if (func_ov096_02135838(c) >= 3) {
                    *(unsigned char*)(func_ov096_021357b4(c) + 0x3a8) = 0;
                }
            }
        }
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x14c);
    func_ov096_02135e2c((int*)c, c + 0x180);
    func_ov096_02135948(c);
    _ZN5dCc_c5ClearEv(c + 0x14c);
    _ZN5dCc_c6UpdateEv(c + 0x14c);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x180) != 0) {
        _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov096_02136534, 0x02136534, size 0xa0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_02136534
extern "C" int func_ov096_02136534(char *c)
{
    int b = (int)(*(unsigned short *)(c + 0xc) == 0xf0);
    if (b != 0) {
        dActor_c::Spawn(0x122, 2, *(Vector3 *)(c + 0x5c), (Vector3_16 *)0, *(signed char *)(c + 0xcc), -1);
    }
    func_ov096_02135800(c);
    *(short *)(c + 0x94) = Vec3_HorzAngle((Vector3 *)(*(char **)(c + 0x388) + 0x5c), (Vector3 *)(c + 0x5c));
    *(int *)(c + 0xa8) = 0x14000;
    if (*(int *)(*(char **)(c + 0x388) + 8) == 2)
        *(int *)(c + 0x98) = 0x28000;
    else
        *(int *)(c + 0x98) = 0x14000;
    *(int *)(c + 0x38c) = 2;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov096_02136434, 0x02136434, size 0x100 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_02136434
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern "C" {

extern unsigned _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned a, unsigned b, int f, int t1, int t2, const void *v, void *cb);
extern unsigned _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(unsigned a, unsigned b, int f, int t1, int t2, const void *v);
extern void *_ZN8Particle6System12FromUniqueIDEj(unsigned id);

int func_ov096_02136434(void *c)
{
    char *s = (char*)c;
    struct Vector3 pos;
    void *p0, *p1;
    int x, y, z;
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, s+0x14c);
    if (*(unsigned*)(s+0x3a0) != 0 && *(unsigned*)(s+0x3a4) != 0) {
        x = *(int*)(s+0x5c);
        z = *(int*)(s+0x64);
        y = *(int*)(s+0x60) + 0x3c000;
        ((int*)&pos)[0] = x;
        ((int*)&pos)[1] = y;
        ((int*)&pos)[2] = z;
        *(unsigned*)(s+0x3a0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned*)(s+0x3a0), 0x13a, pos.x, pos.y, pos.z, 0, 0);
        *(unsigned*)(s+0x3a4) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            *(unsigned*)(s+0x3a4), 0x13b, pos.x, pos.y, pos.z, 0);
        p0 = _ZN8Particle6System12FromUniqueIDEj(*(unsigned*)(s+0x3a0));
        p1 = _ZN8Particle6System12FromUniqueIDEj(*(unsigned*)(s+0x3a4));
        if (p0) *(int*)((char*)p0+0x50) = 0x7fff;
        if (p1) *(int*)((char*)p1+0x50) = 0x7fff;
    }
    func_02038414(s+0x180);
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(s+0x180) != 0)
        func_ov096_0213585c(c);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov096_0213640c, 0x0213640c, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_0213640c
extern "C" {
int func_ov096_0213640c(char *c)
{
    func_ov096_02135800(c);
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0x38c) = 3;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov096_021363c4, 0x021363c4, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_021363c4
extern "C" {
int func_ov096_021363c4(void *c) {
    int r2 = *(int *)((char *)c + 0xb0);
    int r1 = (r2 & 0x20000) ? 1 : 0;
    if (r1 == 0) {
        r1 = (r2 & 0x40000) ? 1 : 0;
        if (r1 == 0)
            func_ov096_0213585c(c);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov096_021363b4, 0x021363b4, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_021363b4
extern "C" {
int func_ov096_021363b4(int *p)
{
    p[227] = 4; return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov096_02136264, 0x02136264, size 0x150 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_02136264
extern "C" {
int func_ov096_02136264(char* self)
{
    char *other = *(char**)(self + 0x394);
    if (!other) {
        func_ov096_02136928(self, 1);
    } else {
        int velx, r, sp, t, z, b;
        s64 prod;
        s16 *tbl, c, sn;
        u16 ang;

        velx = *(int*)(other + 0x80);
        prod = (s64)velx * 0x6e000;
        *(int*)(self + 0x5c) = *(int*)(other + 0x5c);
        r = (int)((prod + 0x800) >> 12);
        other = *(char**)(self + 0x394);
        *(int*)(self + 0x60) = r + *(int*)(other + 0x60);
        other = *(char**)(self + 0x394);
        t = *(int*)(other + 0x64);
        tbl = data_02082214;
        *(int*)(self + 0x64) = t;
        other = *(char**)(self + 0x394);
        sp = 0xe000;
        c = *(s16*)(other + 0x8e);
        *(s16*)(self + 0x8e) = c;
        other = *(char**)(self + 0x394);
        other = other + 0x300;
        c = *(s16*)(other + 0xaa);
        c = (s16)(c + 0x13000);
        *(s16*)(self + 0x3aa) = c;
        ang = *(u16*)(self + 0x3aa);
        c = tbl[(ang >> 4) * 2];
        b = 0x800;
        z = 0;
        *(int*)(self + 0x378) = (int)(((s64)c * sp + b) >> 12);
        *(int*)(self + 0x37c) = z;
        ang = *(u16*)(self + 0x3aa);
        sn = tbl[(ang >> 4) * 2 + 1];
        *(int*)(self + 0x380) = (int)(((s64)sn * sp + b) >> 12);
        if (r == 0x6e000)
            func_ov096_02136928(self, 0);
    }
    func_ov096_02135948(self);
    _ZN5dCc_c5ClearEv(self + 0x14c);
    _ZN5dCc_c6UpdateEv(self + 0x14c);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov096_02136134, 0x02136134, size 0x130 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_02136134
/* recovered: shared common types */
extern "C" int func_ov096_02136134(char* c){
  int cond = (*(unsigned short*)(c+0xc) == 0xf0);
  if(cond){
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x122, 2, (struct Vector3*)(c+0x5c), 0, *(signed char*)(c+0xcc), -1);
  }
  {
    _ZN5Sound9PlayBank0EjRK7Vector3(9, c+0x74);
    func_ov096_02135800(c);
    *(short*)(c+0x94) = Vec3_HorzAngle((char*)*(void**)(c+0x388)+0x5c, c+0x5c);
    int rnd = RandomIntInternal(data_0209e650);
    *(short*)(c+0x94) = *(short*)(c+0x94) + ((rnd & 0x7fff) - 0x4000);
    *(short*)(c+0x8e) = *(short*)(c+0x94) + 0x8000;
    *(int*)(c+0xa8) = 0x28000;
    *(int*)(c+0x98) = 0xa000;
    *(unsigned char*)(c+0x3ac) = 0x2d;
    daSanbo_c* o = (daSanbo_c*)c;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, *(int*)(c+0x5c), *(int*)(c+0x60)+o->OnAimedAtWithEgg(), *(int*)(c+0x64));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, *(int*)(c+0x5c), *(int*)(c+0x60)+o->OnAimedAtWithEgg(), *(int*)(c+0x64));
    *(int*)(c+0x38c) = 5;
  }
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov096_021360c4, 0x021360c4, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_021360c4
extern "C" {
int func_ov096_021360c4(char *c) {
    int eq = (*(unsigned short*)(c+0xc) == 0xf0) ? 1 : 0;
    if (eq != 0) {
        *(short*)(c+0x8c) = *(short*)(c+0x8c) - 0x1000;
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c+0x14c);
    func_02038414(c+0x180);
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(c+0x180) || !DecIfAbove0_Byte((unsigned char*)(c+0x3ac))) {
        func_ov096_0213585c(c);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov096_02135efc, 0x02135efc, size 0x1c8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_02135efc
/* recovered: shared common types */
extern "C" void func_ov096_02135efc(void* cv)
{
    char* c = (char*)cv;
    struct Vector3 v;
    enum Bool b;

    if (*(int*)(c + 0x38c) == 5) {
        b = (enum Bool)(*(unsigned short*)(c + 0xc) == 0xf0);
        if (b) {
            int y1, y2;

            Vec3_Asr(&v, (struct Vector3*)(c + 0x5c), 3);
            Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);

            y1 = ((daSanbo_c*)c)->OnAimedAtWithEgg() >> 3;
            Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y1, 0);

            Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68,
                *(s16*)(c + 0x8c), *(s16*)(c + 0x8e), *(s16*)(c + 0x90));

            y2 = (-((daSanbo_c*)c)->OnAimedAtWithEgg()) >> 3;
            Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y2, 0);

            *(struct Mtx43*)(c + 0xf0) = data_020a0e68;

            *(int*)(c + 0x360) = *(int*)(c + 0x5c) >> 3;
            *(int*)(c + 0x364) = *(int*)(c + 0x60) >> 3;
            *(int*)(c + 0x368) = *(int*)(c + 0x64) >> 3;

            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
                c, c + 0x124, c + 0x33c, 0xa0000, 0x2bc000, 0xf);
            return;
        }
    }

    Matrix4x3_FromRotationY(c + 0xf0, *(s16*)(c + 0x8e));

    *(int*)(c + 0x114) = (*(int*)(c + 0x5c) + *(int*)(c + 0x378)) >> 3;
    *(int*)(c + 0x118) = (*(int*)(c + 0x60) + 0x3c000) >> 3;
    *(int*)(c + 0x11c) = (*(int*)(c + 0x64) + *(int*)(c + 0x380)) >> 3;

    if (*(int*)(c + 0x394) == 0 || *(int*)(c + 0x38c) == 2 || *(int*)(c + 0x38c) == 5) {
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, c + 0x124, c + 0xf0, 0x82000, 0x2bc000, 0xf);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov096_02135e2c, 0x02135e2c, size 0xd0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_02135e2c
extern "C" {
void* _ZNK10dBgCh_Actr14GetFloorResultEv(void* c);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* s, int* out);
int _ZN4cstd4fdivEii(int a, int b);
int _ZNK10dBgCh_Actr8IsOnWallEv(void* c);
void* _ZNK10dBgCh_Actr13GetWallResultEv(void* c);

void func_ov096_02135e2c(int* self, void* clsn)
{
    int n0[3];
    int n1[3];
    func_02038414(clsn);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(clsn)) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(clsn)+4, n0);
        if (n0[1] != 0) {
            long long a = (long long)n0[0] * (long long)self[0xa4/4];
            long long b = (long long)n0[2] * (long long)self[0xac/4];
            int x = (int)((a + 0x800) >> 12);
            int y = (int)((b + 0x800) >> 12);
            self[0xa8/4] = -(_ZN4cstd4fdivEii(x + y, n0[1]) + 0x8000);
        }
    }
    if (_ZNK10dBgCh_Actr8IsOnWallEv(clsn)) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)_ZNK10dBgCh_Actr13GetWallResultEv(clsn)+4, n1);
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov096_02135948, 0x02135948, size 0x4e4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_02135948
/* recovered: shared common types, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Player.h"
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef short s16;
typedef long long s64;



extern "C" {
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, void* pos);
extern void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern s16 data_02082214[];
extern unsigned _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned a, unsigned b, int f, int t1, int t2, const void* v, void* cb);
extern unsigned _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(unsigned a, unsigned b, int f, int t1, int t2, const void* v);
extern void _ZN6Player16IncMegaKillCountEv(void* p);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, void* v, unsigned int a, int fix, unsigned int b, unsigned int d, unsigned int e);

void func_ov096_02135948(char* c)
{
    unsigned int id170;
    char* p;
    int r150;
    int angle;
    int a;
    s16 cs;
    int dist;
    s16 sn;
    int dist2;
    unsigned int flags;
    int st38c;
    char* q;
    struct Vector3 pos;
    int vv1[3];
    int vv2[3];
    int vv3[3];
    int vv4[3];
    int vv5[3];
    int x, y, z;

    if (_ZN8dActor_c7FindEggER5dCc_c(c, c + 0x14c) != 0 ||
        _ZN8dActor_c18FindExplosionActorER5dCc_c(c, c + 0x14c) != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, c + 0x74);
        func_ov096_02135800(c);
        {
            int b = (*(unsigned short*)(c + 0xc) == 0xf0);
            if (b) {
                _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0x122, 2, c + 0x5c, 0, *(signed char*)(c + 0xcc), -1);
            }
        }
        func_ov096_0213585c(c);
        return;
    }

    id170 = *(unsigned int*)(c + 0x170);
    if (id170 == 0)
        return;

    p = (char*)_ZN8dActor_c10FindWithIDEj(id170);
    if (p == 0)
        return;

    if (*(int*)(c + 0x38c) == 1) {
        int b = (*(unsigned short*)(p + 0xc) == 0x135);
        if (b) {
            r150 = *(int*)(c + 0x150);
            if (Vec3_HorzDist(c + 0x5c, p + 0x5c) < r150 + 0x1a9000) {
                angle = Vec3_HorzAngle(p + 0x5c, c + 0x5c);
                a = ((unsigned short)angle >> 4) * 2;
                cs = data_02082214[a];
                dist = *(int*)(c + 0x150) + 0x1a9000;
                *(int*)(c + 0x5c) = *(int*)(p + 0x5c) + (int)(((s64)dist * cs + 0x800) >> 12);
                sn = data_02082214[a + 1];
                dist2 = *(int*)(c + 0x150) + 0x1a9000;
                *(int*)(c + 0x64) = *(int*)(p + 0x64) + (int)(((s64)dist2 * sn + 0x800) >> 12);
            }
        }
    }

    {
        int bf = (*(unsigned short*)(p + 0xc) == 0xbf);
        if (!bf)
            return;
    }

    flags = *(unsigned int*)(c + 0x16c);
    if (!(flags & 0x8000)) {
        if (flags & 0x40000) {
            x = *(int*)(c + 0x5c);
            z = *(int*)(c + 0x64);
            y = *(int*)(c + 0x60) + 0x3c000;
            ((int*)&pos)[0] = x;
            ((int*)&pos)[1] = y;
            ((int*)&pos)[2] = z;
            *(unsigned*)(c + 0x3a0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                *(unsigned*)(c + 0x3a0), 0x13a, pos.x, pos.y, pos.z, 0, 0);
            *(unsigned*)(c + 0x3a4) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
                *(unsigned*)(c + 0x3a4), 0x13b, pos.x, pos.y, pos.z, 0);
            _ZN5Sound9PlayBank0EjRK7Vector3(9, c + 0x74);
            *(void**)(c + 0x388) = p;
            func_ov096_02136928(c, 2);
        } else if ((flags & 0x26fe0) || _ZN6Player9IsOnShellEv(p) != 0 ||
                   *(unsigned char*)(p + 0x6f9) != 0) {
            _ZN5Sound9PlayBank0EjRK7Vector3(9, c + 0x74);
            *(void**)(c + 0x388) = p;
            func_ov096_02136928(c, 2);
        } else if (*(unsigned int*)(c + 0x16c) & 0x10) {
            *(void**)(c + 0x388) = p;
            _ZN6Player16IncMegaKillCountEv(p);
            func_ov096_02136928(c, 5);
        } else {
            st38c = *(int*)(c + 0x38c);
            if (st38c == 0) {
                q = *(char**)(c + 0x390);
                if (q == 0) {
                    if (*(int*)(c + 0xa8) == 0) {
                        vv1[0] = *(int*)(c + 0x5c);
                        vv1[1] = *(int*)(c + 0x60);
                        vv1[2] = *(int*)(c + 0x64);
                        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, vv1, 2, 0xc000, 1, 0, 1);
                    }
                } else if (*(int*)(c + 0xa8) == 0 && *(int*)(q + 0xa8) == 0) {
                    vv2[0] = *(int*)(c + 0x5c);
                    vv2[1] = *(int*)(c + 0x60);
                    vv2[2] = *(int*)(c + 0x64);
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, vv2, 2, 0xc000, 1, 0, 1);
                }
            } else if (st38c == 1) {
                if (*(void**)(c + 0x390) == 0) {
                    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x180) != 0) {
                        vv3[0] = *(int*)(c + 0x5c);
                        vv3[1] = *(int*)(c + 0x60);
                        vv3[2] = *(int*)(c + 0x64);
                        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, vv3, 2, 0xc000, 1, 0, 1);
                    }
                } else if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x180) != 0 &&
                           *(int*)(*(char**)(c + 0x390) + 0xa8) == 0) {
                    vv4[0] = *(int*)(c + 0x5c);
                    vv4[1] = *(int*)(c + 0x60);
                    vv4[2] = *(int*)(c + 0x64);
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, vv4, 2, 0xc000, 1, 0, 1);
                }
            } else {
                vv5[0] = *(int*)(c + 0x5c);
                vv5[1] = *(int*)(c + 0x60);
                vv5[2] = *(int*)(c + 0x64);
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, vv5, 2, 0xc000, 1, 0, 1);
            }
        }
    }

    {
        int t = (*(unsigned int*)(c + 0xb0) & 0x20000) != 0;
        if (t) {
            func_ov096_02136928(c, 3);
        }
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov096_021358c8, 0x021358c8, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_021358c8
extern "C" {
int _ZN8dActor_c22ClosestNonVanishPlayerEv(void*);
void _Z14ApproachLinearRsss(short*, short, short);
int func_ov096_02135878(void*, int);
void func_ov096_021358c8(char* c){
  char* p = (char*)_ZN8dActor_c22ClosestNonVanishPlayerEv(c);
  char* tgt;
  if(p){
    int d = Vec3_HorzDist(c+0x36c, p+0x5c);
    if(d < 0x3e8000) tgt = p+0x5c; else tgt = c+0x36c;
  } else {
    tgt = c+0x36c;
  }
  int dist = Vec3_HorzDist(c+0x5c, tgt);
  short ang = Vec3_HorzAngle(c+0x5c, tgt);
  _Z14ApproachLinearRsss((short*)(c+0x8e), ang, 0x320);
  int r = func_ov096_02135878(c, dist);
  short v = *(short*)(c+0x8e);
  *(short*)(c+0x94) = v + r;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov096_02135878, 0x02135878, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_02135878
extern "C" int func_ov096_02135878(void* unused, int x){
  if(x>0x190000) return 0;
  int q=cstd::fdiv(0x4000,0xc8000);
  long long m=(long long)q*x;
  m+=0x800;
  int r=(int)(m>>12);
  r=0x8000-r;
  return (short)r;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov096_0213585c, 0x0213585c, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_0213585c
extern "C" {
void func_ov096_0213585c(void *t)
{
    daSanbo_c *segment = (daSanbo_c *)t;
    segment->PoofDust();
    segment->MarkForDestruction();
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov096_02135838, 0x02135838, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_02135838
extern "C" {
int func_ov096_02135838(char *c) {
    daSanbo_c *r1 = ((daSanbo_c *)c)->mPrevSegment;
    int r0 = 0;
    if (r1 == 0) return r0;
    do {
        r1 = r1->mPrevSegment;
        r0 = r0 + 1;
    } while (r1 != 0);
    return r0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov096_02135800, 0x02135800, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_02135800
extern "C" void func_ov096_02135800(char* c){
  daSanbo_c *segment = (daSanbo_c *)c;
  int b = (segment->actorID == 0xf0);
  if(b) return;
  daSanbo_c *next = segment->mNextSegment;
  daSanbo_c *prev = segment->mPrevSegment;
  prev->mNextSegment = next;
  next = segment->mNextSegment;
  if(next) next->mPrevSegment = segment->mPrevSegment;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov096_021357b4, 0x021357b4, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov096_021357b4
extern "C" {
char *func_ov096_021357b4(char *cc){
    daSanbo_c *c = (daSanbo_c *)cc;
    daSanbo_c *p = c->mPrevSegment;
    if(p==0) return (char *)c;
    while(p){
        unsigned r2 = (p->actorID != 0xf0) ? 1u : 0u;
        if(!r2) return (char *)p;
        p = p->mPrevSegment;
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN9daSanbo_c16OnAimedAtWithEggEv, 0x021357ac, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSanbo_c16OnAimedAtWithEggEv
/* daSanbo_c::OnAimedAtWithEgg -- vtable slot 29, recovered from vtable slot identity.
 * The ROM body ignores `this` and returns a constant. */
int daSanbo_c::OnAimedAtWithEgg()
{
    return 245760;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN9daSanbo_c13OnYoshiTryEatEv, 0x021357a4, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSanbo_c13OnYoshiTryEatEv
/* daSanbo_c::OnYoshiTryEat -- vtable slot 18, recovered from vtable slot identity.
 * The ROM body ignores `this` and returns a constant. */
int daSanbo_c::OnYoshiTryEat()
{
    return 4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN9daSanbo_cD0Ev, 0x02135748, size 0x5c */
/* -------------------------------------------------------------------------- */
/* _ZN9daSanbo_cD0Ev, 0x02135748, size 0x5c -- vtable slot 17.
 * No definition here and no @symbol marker: the destructor is written inline in
 * include/daSanbo_c.h, so mwccarm emits the vague-linkage D1/D0 pair itself, in
 * the cartridge's own D1-then-D0 order, and emits no homeless D2. Writing it out
 * of line here would emit D2,D0,D1 and put D0 below D1, which is not the ROM's
 * layout and which production isolation refuses. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN9daSanbo_cD1Ev, 0x02135700, size 0x48 */
/* -------------------------------------------------------------------------- */
/* _ZN9daSanbo_cD1Ev, 0x02135700, size 0x48 -- vtable slot 16. Also emitted from
 * the inline body in include/daSanbo_c.h: one vptr store, then mModel /
 * mShadowModel / mdCcAc_c / mWithMeshClsn destroyed in reverse declaration
 * order, then ~dActor_c. Every instruction of it is the compiler's. */
