//cpp
/* da1up_c -- the 1-Up / Mega Mushroom actor, ov002.
 *
 * Reconstructed translation unit: the contiguous linker run
 * 0x020aee40..0x020b0530, ROM ordinals 0..35, 36 functions, assembled from the
 * 36 one-function legacy sources the promotion deletes and then reconciled by
 * hand. config/tu_manifest.d/ov002/da1up_c.json names every one of them.
 *
 * IDENTITY IS THE CARTRIDGE'S, NOT THE TREE'S. ov002 file offset 0x5ad10 ==
 * address 0x02108370 holds `7da1up_c\0`, the length-prefixed Itanium type-name
 * string, and _ZTI7da1up_c at 0x0210837c is the matching __si_class_type_info
 * whose +8 word reaches _ZTI12dEnemyBase_c at 0x021081c0. The project's former
 * spelling `OneUpMushroom` is absent from every image in every encoding tested,
 * so the class, its header and its nine mangled members move to the ROM name in
 * this change. The scout's fact file is still at its pre-rename path,
 * notes/data/class-facts/OneUpMushroom.json, and records `class: da1up_c`.
 *
 * SOURCE ORDER IS ROM-ASCENDING AND `#pragma defer_codegen off` IS
 * LOAD-BEARING; they are ONE decision, exactly as on ov006/dScMgPanel_c. With
 * codegen deferred (the default) mwccarm 2004/b56 emits one .text section per
 * function in the REVERSE of source order, which would demand a forward
 * declaration for all 36 members and force one canonical spelling on every
 * shared helper. Generating at parse time emits in source order instead, so
 * every callee but two is already defined above its caller and each member's
 * independently recovered view of a helper survives untouched.
 *
 * THE DESTRUCTOR STAYS OUT OF LINE AND IS DECLARED FIRST, SO THIS TU OWNS THE
 * CLASS'S KEY FUNCTION. The cartridge orders D1 (0x020aee40, 0x48) BELOW D0
 * (0x020aee88, 0x5c) and carries no D2; out-of-line + `defer_codegen off` emits
 * D1, D0, D2, which is that order with a homeless D2 trailing where it is
 * deadstripped. Ordinals 0 and 1 are two shards of ONE C++ definition, so only
 * one destructor body is written below. Owning the key function is also why
 * this TU emits the whole chain's vtable and typeinfo as vague-linkage
 * passengers -- see the manifest's compiler_only_output block.
 *
 * EACH `extern "C"` MEMBER KEEPS ITS OWN DECLARATIONS, INSIDE ITS OWN BODY.
 * mwccarm 2004/b56 gives a block-scope declaration in an `extern "C"` region C
 * linkage (measured on ov002/Player, 301 members), so contradictory recovered
 * views of one ROM symbol coexist without a call site being rewritten.
 * Measured on this file as it stands: `_ZN5Sound9PlayBank3EjRK7Vector3` is
 * declared at SEVEN sites and no two spellings are identical, and
 * `func_ov002_020aefb8` is declared at seven sites in five distinct spellings
 * -- `char *`, `unsigned char *` and `void *` first parameters, plus a
 * no-argument `(void)` in ordinal 24, which really does pass nothing.
 * THE EIGHT C++ DEFINITIONS ARE THE EXCEPTION, and they have to be: a class
 * member function cannot sit inside an `extern "C"` region, so the same
 * block-scope declaration there gets C++ LINKAGE and the reference mangles.
 * Their external FUNCTION declarations therefore live in two declaration-only
 * file-scope `extern "C"` regions, each placed immediately above the members
 * that need it. The file's full region census is 30 file-scope `extern "C"`
 * regions: 27 one-member wrappers, those 2, and one at the top of the file for
 * the two upward intra-TU calls. Their external DATA declarations do not: mwccarm leaves
 * a file-scope variable's name unmangled in C++, so those stay in the bodies and
 * `data_ov002_0210d9d8` keeps being a `void *` in ordinal 31 and a
 * `SharedFilePtr` in ordinal 35.
 *
 * `decl_common.h` IS DELIBERATELY NOT INCLUDED. It declares 11 of these 36
 * symbols and 3 of the 11 have different parameter views from their definitions:
 * `func_ov002_020af4ec` (`char *` vs the definition's `void *`),
 * `func_ov002_020afc68` (`char *` vs `unsigned char *`), and
 * `func_ov002_020af684` (`void *, int, int` vs `char *, int, char *`).
 * The shared void egg-turn contract reconciles the former return disagreements
 * for `func_ov002_020af684` and `func_ov002_020afa6c`. Pulling it in makes each an
 * `illegal function overloading`
 * error against a byte-matched body. ov002/Player and ov006/dScMgPanel_c, the
 * two largest promoted TUs, exclude it for the same reason.
 *
 * NINE OF THE 36 SYMBOLS ARE METHODS, written as EIGHT `da1up_c::`
 * definitions (ordinals 0 and 1 are two shards of the one destructor), and all
 * nine are this class's own vtable slots: ordinals 0/1 the destructor pair
 * (slots 16/17), 9 OnTurnIntoEgg (19), 10 OnYoshiTryEat (18), 31
 * CleanupResources (3), 32 OnPendingDestroy (12), 33 Render (9), 34 Behavior
 * (6) and 35 InitResources (0). These nine symbols already had native member
 * definitions in the shard sources; promotion consolidates and renames them.
 * The other 27 stay free functions. Fourteen of those are ROM-proven
 * non-static members of this class -- the 14 {function pointer, 0} descriptors
 * at 0x02108300..0x02108370 are pointer-to-member-function objects with a zero
 * `this` adjustment, and __sinit_ov002_02100adc copies them into the 14-element
 * dispatch array at 0x0210dc00 that ordinal 34 indexes by mMushroomType. Their
 * member-ness and their INDEX are proven; their original NAMES are not.
 * They retain address-derived spellings in this packaging step. Explicitly
 * coined names and typed methods remain reconstruction work; the other 13
 * helpers need separate membership evidence. The 14 are, by index:
 * 0 020aff10, 1 020afe4c, 2 020afd10,
 * 3 020afc44, 4 020afbb4, 5 020afa98, 6 020afa6c, 7 020af950, 8 020af924,
 * 9 020af838, 10 020af7cc, 11 020afa50, 12 020af908, 13 020af724.
 * Leftover fold adds the two classInit factories at 0x020b0530/0x020b0580,
 * so the licensed run is 38 functions through 0x020b05d0.
 *
 * deslop
 * Leftover (cited MATCH walls):
 * - dBgCh_Actr::Init / dCcAc_c::Init / DropShadowRadHeight / ReflectAngle 6az
 *   (Fix12i mangles as i; ROM is Fix12<int> -- method form Undefined)
 * - Particle::System::New / NewSimple: no method declaration in include/
 * - Behavior 0x100: named ++mStateTimer size-DIFF vs unsigned-short launder
 *   (0x38c stays unk_38c: naming it mStateTimer shadowed dEnemyBase_c::mStateTimer
 *   at 0x100. Distinct field; this leftover is the 0x100 u16 launder.)
 * - struct C PMF stand-in (mwccarm PMF representation depends on the class)
 * - SharedFilePtr has no recovered fields; handles stay data_ov002_*
 * - decl_common.h stays out (3 of 11 declarations disagree with MATCH bodies)
 */

#pragma defer_codegen off

#include "types.h"
#include "common.h"
#include "da1up_c.h"
#include "dBgCh_Actr.h"
#include "dBgCh_Gnd.h"
#include "Player.h"
#include "Sound.h"
#include "SharedFilePtr.h"

/* The only two intra-TU calls that run UPWARD in ROM address order, so the only
   two that ROM-ascending source order cannot satisfy from the definition above:
   ordinal 21 (0x020afa50) calls ordinal 22 (0x020afa6c), and ordinal 18
   (0x020af908) calls ordinal 19 (0x020af924). Both spellings are the
   definitions' own, so nothing below has to be adapted to them. */
extern "C" {
void func_ov002_020afa6c(char *c);
void func_ov002_020af924(char *c);
}

/* -------------------------------------------------------------------------- */
/*                         _ZN7da1up_cD0Ev, 0x020aee88, size 0x5c              */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_cD1Ev
// @symbol _ZN7da1up_cD0Ev
/* ONE definition, both variants. The complete-object destructor (D1) tears the
   four members down in exact reverse of the factories' construction order --
   ShadowModel at 0x350, Model at 0x300, dBgCh_Actr at 0x144, dCcAc_c at 0x110,
   then ~dEnemyBase_c -- and every one of those is a typed member of this class,
   so the body is empty and the compiler writes the chain. The deleting
   destructor (D0) inlines that same teardown and then calls
   Memory::Deallocate(this, GAME_HEAP_PTR) through dEnemyBase_c's own inline
   `operator delete`, which is why nothing below mentions a heap. Under
   `#pragma defer_codegen off` the variants come out D1, D0, D2; the trailing D2
   is homeless and is deadstripped. */
da1up_c::~da1up_c()
{
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020aeee4
extern "C" {
void func_ov002_020aeee4(char* c) {
    extern unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        unsigned int uniqueID, unsigned int effectID,
        int x, int y, int z, const void* dir, void* callback);
    extern unsigned char data_0209f2d8;

    int t1 = (*(unsigned short*)(c + 0xc) == 0x115);
    unsigned int effectID = 0;
    if (t1 != false) effectID = 0x108;

    int t2 = ((*(unsigned int*)(c + 0xb0) & 8) != 0);
    if (t2 != false) {
        int t3 = (data_0209f2d8 == 1);
        if (t3 == false) return;
    }

    Vector3 pos;
    pos.x = *(int*)(c + 0x5c);
    pos.y = *(int*)(c + 0x60);
    pos.z = *(int*)(c + 0x64);
    pos.y += 0x1e000;
    volatile Vector3* vp = &pos;
    *(unsigned int*)(c + 0x394) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(volatile unsigned int*)(c + 0x394), effectID, vp->x, vp->y, pos.z, 0, 0);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020aefa4
/* MEASURED: this definition must stay `void`. Declaring it `int` -- so that
   ordinal 7's `return func_ov002_020aefa4(c);` would type-check against a
   file-scope declaration -- costs four of this function's five words. Ordinal 7
   keeps its own `int` view at block scope instead, which is exactly the C
   linkage the enclosing `extern "C"` region gives it. */
extern "C" {
void func_ov002_020aefa4(char *self)
{
    *(unsigned int *)(self + 0x12c) |= 0x8000;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020aefb8
/* The shard carried shadow `dActor_c`/`dEnemyBase_c` tags to name three
   non-virtual methods. The merged TU has both real classes complete through
   da1up_c.h, so the shadows are gone and the calls go through the real types --
   which mangle identically, the class name being the whole of the difference.
   _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s stays spelt out. Its reconstructed
   symbol encodes by-value Fix12<int> parameters; the recorded typed-call
   experiment changed the bytes. This bridge remains pending further
   signature/codegen work. */
extern "C" {
void func_ov002_020aefb8(char* self) {
    extern short _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void *, int, int, short);
    extern int Vec3_HorzLen(void*);

    int *px;
    int *pz;
    ((dActor_c*)self)->UpdatePosWithHorzSpeedAndAng();
    if (((dBgCh_Actr*)(self + 0x144))->IsOnGround()) {
        px = (int*)(int)(self + 0xa4);
        *px += *(int*)(self + 0xd4) * 0xa;
        pz = (int*)(int)(self + 0xac);
        *pz += *(int*)(self + 0xdc) * 0xa;
        if (((dBgCh_Actr*)(self + 0x144))->JustHitGround()) {
            *(int*)(self + 0xa8) = -(*(int*)(self + 0xa8) << 2) / 10;
        } else {
            *(int*)(self + 0xa8) = 0;
        }
        if (Vec3_HorzLen(self + 0xa4) > *(int*)(self + 0x98)) {
            *(int*)(self + 0x98) = Vec3_HorzLen(self + 0xa4);
            if (*(int*)(self + 0x98) >= 0xf000) *(int*)(self + 0x98) = 0xf000;
        }
    }
    ((dActor_c*)self)->UpdatePosWithOnlySpeed((dCc_c*)(self + 0x110));
    ((dEnemyBase_c*)self)->UpdateWMClsn(*(dBgCh_Actr*)(self + 0x144), 0);
    if (!((dBgCh_Actr*)(self + 0x144))->IsOnWall()) return;
    *(short*)(self + 0x94) = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s((dActor_c*)self, *(int*)(self + 0xe0), *(int*)(self + 0xe8), *(short*)(self + 0x94));
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020af0c0
extern "C" {
void func_ov002_020af0c0(char* c){
    extern short data_02082214[];
    extern char* _ZN8dActor_c13ClosestPlayerEv(void*);
    extern int func_ov002_020d0d2c(void*);
    extern int Vec3_HorzLen(const Vector3*);
    extern int _ZN4cstd5atan2E5Fix12IiES1_(int, int);
    extern short Vec3_HorzAngle(const Vector3*, const Vector3*);
    extern void _Z14ApproachLinearRsss(short*, int, int);
    /* Forward: ordinal 11 sits above this one in ROM order. */
    extern void func_ov002_020af3a8(char* thiz);

    char* p = _ZN8dActor_c13ClosestPlayerEv(c);
    if(p != 0){
        Vector3 diff;
        Vector3 ppos;
        int* s = (int*)((void*)(int)((char*)(p) + 0x5c));
        ppos.x = s[0];
        ppos.y = s[1];
        ppos.z = s[2];
        diff.x = ppos.x - *(int*)(c+0x5c);
        if(func_ov002_020d0d2c(p) != 0)
            diff.y = ppos.y - *(int*)(c+0x60) - 0x50000;
        else
            diff.y = ppos.y - *(int*)(c+0x60) + 0x78000;
        diff.z = ppos.z - *(int*)(c+0x64);
        int len = Vec3_HorzLen(&diff);
        int pitch = _ZN4cstd5atan2E5Fix12IiES1_(len, diff.y);
        short yaw = Vec3_HorzAngle((Vector3*)(c+0x5c), &ppos);
        _Z14ApproachLinearRsss((short*)(c+0x94), yaw, 0x1000);
        _Z14ApproachLinearRsss((short*)(c+0x92), pitch, 0x1000);
        *(int*)(c+0xa8) = (short)data_02082214[(*(unsigned short*)(c+0x92) >> 4)*2+1] * (short)0x1e;
        *(int*)(c+0x98) = (short)data_02082214[(*(unsigned short*)(c+0x92) >> 4)*2] * (short)0x1e;
    }
    func_ov002_020af3a8(c);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020af1dc
extern "C" {
int func_ov002_020af1dc(char* c){
  extern int _ZN8dActor_c10FindWithIDEj(unsigned int);
  int r=0;
  unsigned int id=*(unsigned int*)(c+0x134);
  if(id && (r=_ZN8dActor_c10FindWithIDEj(id)) && (*(int*)(c+0x130)&0x400000))
    return r;
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020af218
/* The second parameter is DECLARED AND UNUSED, and that is the point: five
   callers below pass 0xbb8 in r1 while this body never reads it and leaves it
   for _ZN8dActor_c15IsPlayerInRangeEi, which the shard likewise calls with one
   argument. Declaring the parameter preserves each caller's argument setup and
   costs this function nothing. */
extern "C" {
int func_ov002_020af218(char* c, int range){
  extern int _ZN8dActor_c15IsPlayerInRangeEi(void*);
  extern int func_ov002_020aefa4(void*);
  *(char*)(c+0x38e)=(char)_ZN8dActor_c15IsPlayerInRangeEi(c);
  unsigned char v=*(unsigned char*)(c+0x38e);
  if(v==0) return v;
  return func_ov002_020aefa4(c);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020af248
extern "C" {
int func_ov002_020af248(char* c, int n){
  extern int _ZN8dActor_c24KillAndTrackInDeathTableEv(void*);
  int v = ((da1up_c*)c)->unk_38c;
  if(v < n) return 0;
  if(v < n + 0x28){
    *(unsigned char*)(c+0x38f) = (v & 1) != 0;
  } else {
    _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
    return 1;
  }
  return 0;
}
}

/* The first of the two file-scope `extern "C"` regions. Ordinal 9 is a class
   member function, so it cannot sit in one and a declaration written in its body
   would mangle; these three have to be here. The shared actor egg-turn hook
   and its forwarding helpers return void. Promotion had changed ordinal 14
   from void to int to agree with the old hook declaration despite its void
   terminal call. Ordinals 9, 19 and 22 now use this one void declaration;
   ordinals 19 and 22 retain the player lookup result and its null test. */
extern "C" {
void GiveLives(int count);
void func_ov002_020af684(char* self, int target, char* player);
void func_ov002_020bdf8c(Player* player);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c13OnTurnIntoEggER6Player
/* Vtable slot 19, verified against config/arm9/overlays/ov002/relocs.txt:
   _ZTV7da1up_c (0x021083c8) + 0x4c relocates to 0x020af2b0, this address. */
void da1up_c::OnTurnIntoEgg(Player &player)
{
    if (mMushroomType == 0xb) {
        return func_ov002_020af684((char*)this, 5, (char*)&player);
    }
    if (mMushroomType == 0xc) {
        return func_ov002_020af684((char*)this, 7, (char*)&player);
    }
    unsigned isMatch = (actorID == 0x114);
    if (isMatch) {
        Vector3 vec;
        Sound::PlayBank3(0x6e, *(Vector3 *)&mCamSpacePosX);
        GiveLives(1);
        vec.x = mPosX;
        vec.y = mPosY;
        vec.z = mPosZ;
        vec.y += 0xb4000;
        Spawn(0x14b, 8, vec, 0, mAreaId, -1);
        KillAndTrackInDeathTable();
    } else {
        func_ov002_020bdf8c(&player);
        KillAndTrackInDeathTable();
    }
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c13OnYoshiTryEatEv
/* Vtable slot 18. Two instructions: mov r0,#4; bx lr. */
s32 da1up_c::OnYoshiTryEat()
{
    return 4;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020af3a8
extern "C" {
void func_ov002_020af3a8(char* c)
{
    extern void _ZN5Sound9PlayBank3EjRK7Vector3(u32 id, struct Vector3* v);
    extern void _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 a, u32 b, struct Vector3* v, void* rot, int e, int f);
    extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void* thiz);
    extern void func_ov002_020bdf8c(int);

    int r = func_ov002_020af1dc(c);
    if (r == 0)
        return;

    unsigned short h = *(unsigned short*)(c + 0xc);
    unsigned is115 = (h == 0x115);
    if (is115) {
        func_ov002_020bdf8c(r);
    } else {
        unsigned is114 = (h == 0x114);
        if (is114) {
            struct Vector3 vec;
            _ZN5Sound9PlayBank3EjRK7Vector3(0x6e, (struct Vector3*)(c + 0x74));
            GiveLives(1);
            vec.x = *(Fix12i*)(c + 0x5c);
            vec.y = *(Fix12i*)(c + 0x60);
            vec.z = *(Fix12i*)(c + 0x64);
            vec.y += 0xb4000;
            _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                0x14b, 8, &vec, 0, *(signed char*)(c + 0xcc), -1);
        }
    }
    _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020af474
extern "C" {
void func_ov002_020af474(char* o)
{
    extern s16 data_02082214[];
    int a;

    if (*(u16*)(o + 0x100) < 5) {
        *(int*)(o + 0xa8) = 0x28000;
        return;
    }

    {
        s16* p = (s16*)(o + 0x92);
        *p = *p - 0x1000;
    }

    a = (int)*(u16*)(o + 0x92) >> 4;
    *(int*)(o + 0xa8) = (s16)data_02082214[a * 2 + 1] * (s16)0x1e + 0x2000;

    a = (int)*(u16*)(o + 0x92) >> 4;
    *(int*)(o + 0x98) = (s16)data_02082214[a * 2] * (s16)-0x1e;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020af4ec
extern "C" {
void func_ov002_020af4ec(void* self)
{
    extern void Matrix4x3_FromRotationY(void* m, int angle);
    extern void Vec3_Asr(struct Vector3* d, struct Vector3* s, int sh);
    extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
    extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* thiz);
    extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* shadow, void* mtx, int height, int rad, unsigned int x);

    int rad;
    char* c = (char*)self;
    int height;
    struct Vector3 v2;
    struct Vector3 v1;

    if ((unsigned)(*(int*)(c + 0x384) - 0xb) <= 1) {
        Matrix4x3_FromRotationY(c + 0x31c, *(short*)(c + 0x8e));
        *(int*)(c + 0x340) = *(int*)(c + 0x5c) >> 3;
        *(int*)(c + 0x344) = *(int*)(c + 0x60) >> 3;
        *(int*)(c + 0x348) = *(int*)(c + 0x64) >> 3;
    } else {
        Vec3_Asr(&v1, (struct Vector3*)(c + 0x5c), 3);
        Matrix4x3_FromTranslation(c + 0x31c, v1.x, v1.y, v1.z);
    }

    if (*(unsigned char*)(c + 0x38e) == 0) return;

    if ((unsigned)(*(int*)(c + 0x384) - 0xb) <= 1) {
        height = 0x50000;
        rad = 0x50000;
    } else if (!_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x144)) {
        int y = *(int*)(c + 0x60);
        int z = *(int*)(c + 0x64);
        int adjustedY;
        int x = *(int*)(c + 0x5c);
        adjustedY = y + 0x28000;
        v2.x = x;
        v2.y = adjustedY;
        v2.z = z;
        dBgCh_Gnd rg;
        rg.SetObjAndPos(v2, 0);
        rad = v2.y;
        if (rg.DetectClsn()) {
            rad = rg.clsnY;
        }
        rad = *(int*)(c + 0x60) - rad;
        if (rad <= 0x1000) rad = 0x1000;
        height = (*(int*)(c + 0x114) - 0xa000) * 2 - (int)(((long long)rad * 0x180 + 0x800) >> 12);
        if (height < 0xa000) height = 0xa000;
        rad += 0x3c000;
    } else {
        rad = 0x3c000;
        height = (*(int*)(c + 0x114) - 0xa000) * 2;
    }

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x350, c + 0x31c, height, rad, 0xf);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020af684
/* Shared with the egg-turn hook and dispatch indices 8 and 6. This helper
   finishes by killing the actor and returns no value. Its callers use the same
   void contract; the lookup result in ordinals 19 and 22 is still needed as
   the player argument. This reconstructs a consistent interface, not an
   original return type recovered from an unused register. */
extern "C" {
void func_ov002_020af684(char* self, int target, char* player){
    struct dActor_c;
    extern dActor_c* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int actorID, dActor_c* prev);
    extern void GiveCoins(int idx, int amount);
    extern void _ZN6Player4HealEi(void* p, int amt);
    extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void* pos);
    extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void* thiz);

    char* p = player;
    dActor_c* found = 0;
    for (;;) {
        found = _ZN8dActor_c15FindWithActorIDEjPS_(*(unsigned short*)(self + 0xc), found);
        if (found == 0)
            break;
        if (target == *(int*)((char*)found + 0x384)) {
            (*(int *)(((int)found + 0x390)))--;
            break;
        }
    }
    if ((unsigned int)(*(int*)(self + 0x384) - 0xb) <= 1) {
        GiveCoins(*(unsigned char*)(p + 0x6d8), 1);
        _ZN6Player4HealEi(p, 0x100);
        if (*(unsigned char*)(p + 0x706))
            _ZN5Sound9PlayBank3EjRK7Vector3(0x12, self + 0x74);
        else
            _ZN5Sound9PlayBank3EjRK7Vector3(0x11, self + 0x74);
    }
    _ZN8dActor_c24KillAndTrackInDeathTableEv(self);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020af724
/* Dispatch-table index 13. */
extern "C" {
void func_ov002_020af724(unsigned char *self)
{
    extern void func_ov002_020aefb8(void *c);
    extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, struct Vector3 *v);
    extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *c);
    extern void func_ov002_020af3a8(void *c);
    extern int func_ov002_020af218(void *c, int a);
    extern void func_ov002_020aeee4(void *c);

    func_ov002_020aefb8(self);
    switch (*(int *)(self + 0x388)) {
    case 0:
        _ZN5Sound9PlayBank3EjRK7Vector3(0x69, (struct Vector3 *)(self + 0x74));
        *(int *)((int)self + 0x388) += 1;
        break;
    case 1:
        if (_ZNK10dBgCh_Actr10IsOnGroundEv((void *)((int)self + 0x144)) != 0) {
            *(int *)((int)self + 0x128) &= ~1;
            *(int *)((int)self + 0x388) += 1;
        }
        break;
    case 2:
        func_ov002_020af3a8(self);
        break;
    }
    func_ov002_020af218(self, 0xbb8);
    func_ov002_020aeee4(self);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020af7cc
/* Dispatch-table index 10. */
extern "C" {
void func_ov002_020af7cc(char* c)
{
    *(unsigned char*)(c + 0x38e) = 1;
    if (*(int*)(c + 0x60) >= *(int*)(c + 0x37c) + 0x64000) return;
    *(int*)(c + 0x60) += 0x5000;
    if (*(int*)(c + 0x60) < *(int*)(c + 0x37c) + 0x64000) return;
    *(int*)(c + 0x60) = *(int*)(c + 0x37c) + 0x64000;
    *(int*)(c + 0x384) = 0;
    *(int*)(c + 0x388) = 0;
    *(unsigned short*)(c + 0x100) = 0xffff;
    ((da1up_c*)c)->unk_38c = 0xffff;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020af838
/* Dispatch-table index 9. */
extern "C" {
void func_ov002_020af838(char* c)
{
    extern void _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 a, u32 b, struct Vector3* v, void* rot, int e, int f);
    extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void* thiz);

    struct Vector3 vec;

    vec.x = *(Fix12i*)(c + 0x378);
    vec.y = *(Fix12i*)(c + 0x37c);
    vec.z = *(Fix12i*)(c + 0x380);
    vec.y = *(Fix12i*)(c + 0x37c) + 0x32000;
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        *(unsigned short*)(c + 0xc), 0x25, &vec, 0, *(signed char*)(c + 0xcc), -1);

    vec.y = *(Fix12i*)(c + 0x37c);
    vec.x = *(Fix12i*)(c + 0x378) - 0x1f4000;
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        *(unsigned short*)(c + 0xc), 0xb, &vec, 0, *(signed char*)(c + 0xcc), -1);

    vec.x = *(Fix12i*)(c + 0x378) + 0x1f4000;
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        *(unsigned short*)(c + 0xc), 0xb, &vec, 0, *(signed char*)(c + 0xcc), -1);

    _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020af908
/* Dispatch-table index 12. */
extern "C" {
void func_ov002_020af908(char *self) {
    *(short *)(int)(self + 0x8e) += 0xc00;
    func_ov002_020af924(self);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020af924
/* Dispatch-table index 8. */
extern "C" {
void func_ov002_020af924(char* c){
  extern int func_ov002_020af1dc(void*);
  int r=func_ov002_020af1dc(c);
  if(!r) return;
  func_ov002_020af684(c, 7, (char*)r);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020af950
/* Dispatch-table index 7. */
extern "C" {
void func_ov002_020af950(char *self)
{
  extern void func_ov002_020aefa4(char *thiz);
  extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int, const void *);
  extern void func_ov002_020af0c0(char *thiz);
  extern void func_ov002_020aefb8(char *thiz);
  extern void func_ov002_020aeee4(char *thiz);
  extern void func_ov002_020af474(char *thiz);

  switch (*((int *)(self + 0x388)))
  {
    case 0:
      *((unsigned char *)(self + 0x38e)) = 0;
      if (*((int *)(self + 0x390)) != 0)
        return;

      *((int *)(self + 0xa8)) = 0x28000;
      *((int *)(self + 0x388)) = 3;
      *((unsigned char *)(self + 0x38e)) = 1;
      func_ov002_020aefa4(self);
      _ZN5Sound9PlayBank3EjRK7Vector3(0x68, self + 0x74);
      *((unsigned int *)(self + 0xb0)) &= ~1;
      return;

    case 1:
      func_ov002_020af0c0(self);
      func_ov002_020aefb8(self);
      return;

    case 3:
      func_ov002_020aefb8(self);
      if (*((unsigned short *)(self + 0x100)) > 0x11)
        func_ov002_020aeee4(self);
      func_ov002_020af474(self);
      if (*((unsigned short *)(self + 0x100)) != 0x25)
        return;

      *((unsigned int *)(self + 0x128)) &= ~1;
      *((int *)(self + 0x388)) = 1;
      *((int *)(self + 0x9c)) = 0;
      *((int *)(self + 0x98)) = 0xa000;
      return;
  }
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020afa50
/* Dispatch-table index 11. */
extern "C" {
void func_ov002_020afa50(char *self) {
    *(short *)(int)(self + 0x8e) += 0xc00;
    func_ov002_020afa6c(self);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020afa6c
/* Dispatch-table index 6. */
extern "C" {
void func_ov002_020afa6c(char* c){
  extern int func_ov002_020af1dc(void*);
  int r=func_ov002_020af1dc(c);
  if(!r) return;
  func_ov002_020af684(c, 5, (char*)r);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020afa98
/* Dispatch-table index 5. */
extern "C" {
void func_ov002_020afa98(char *c)
{
    extern void _ZN5Sound9PlayBank3EjRK7Vector3(u32 id, struct Vector3 *v);
    extern void func_ov002_020aefa4(char *thiz);
    extern void func_ov002_020aefb8(char *thiz);
    extern void func_ov002_020afde4(char *thiz);
    extern void func_ov002_020aeee4(char *thiz);
    extern void func_ov002_020af3a8(char *thiz);
    extern void func_ov002_020af474(char *thiz);
    extern int func_ov002_020af248(char *thiz, int n);

    switch (*(s32 *)(c + 0x388)) {
    case 0:
        *(u8 *)(c + 0x38e) = 0;
        if (*(s32 *)(c + 0x390) != 0)
            return;
        *(u32 *)(c + 0xa8) = 0x28000;
        *(s32 *)(c + 0x388) = 3;
        *(u8 *)(c + 0x38e) = 1;
        func_ov002_020aefa4(c);
        _ZN5Sound9PlayBank3EjRK7Vector3(0x68, (struct Vector3 *)(c + 0x74));
        *(u32 *)(c + 0xb0) &= ~1;
        return;
    case 1:
        func_ov002_020aefb8(c);
        func_ov002_020afde4(c);
        func_ov002_020aeee4(c);
        return;
    case 2:
        func_ov002_020aefb8(c);
        func_ov002_020af3a8(c);
        func_ov002_020af248(c, 0x1e);
        return;
    case 3:
        func_ov002_020aefb8(c);
        if (*(u16 *)(c + 0x100) > 0x11) {
            func_ov002_020aeee4(c);
        }
        func_ov002_020af474(c);
        if (*(u16 *)(c + 0x100) != 0x25)
            return;
        *(u32 *)(c + 0x128) &= ~1;
        *(s32 *)(c + 0x388) = 1;
        *(u32 *)(c + 0x98) = 0x8000;
        return;
    }
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020afbb4
/* Dispatch-table index 4. The two `func_ov002_020aefb8()` calls really do pass
   no argument -- r0 already carries the object -- so this member keeps its own
   nullary view of that symbol at block scope. */
extern "C" {
void func_ov002_020afbb4(char* c)
{
    extern int _ZN8dActor_c15IsPlayerInRangeEi(char* thiz, int r);
    extern void func_ov002_020aefb8(void);
    extern void func_ov002_020afde4(char* thiz);
    extern void func_ov002_020aeee4(char* thiz);
    extern void func_ov002_020af3a8(char* thiz);
    extern int func_ov002_020af248(char* thiz, int n);
    extern int func_ov002_020af218(char* thiz, int n);

    switch (*(int*)(c + 0x388)) {
    case 0:
        if (_ZN8dActor_c15IsPlayerInRangeEi(c, 0x3e8)) {
            *(int*)(c + 0xa8) = 0x28000;
            *(int*)(c + 0x388) = 1;
        }
        break;
    case 1:
        func_ov002_020aefb8();
        func_ov002_020afde4(c);
        func_ov002_020aeee4(c);
        break;
    case 2:
        func_ov002_020aefb8();
        func_ov002_020af3a8(c);
        func_ov002_020af248(c, 0x1e);
        break;
    }
    func_ov002_020af218(c, 0xbb8);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020afc44
/* Dispatch-table index 3. */
extern "C" {
int func_ov002_020afc44(char* c){
  extern int func_ov002_020af3a8(void*);
  extern int func_ov002_020af218(void*, int);
  func_ov002_020af3a8(c);
  return func_ov002_020af218(c, 0xbb8);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020afc68
extern "C" {
void func_ov002_020afc68(unsigned char *self)
{
    extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *c);
    extern int _ZN4cstd4fdivEii(int a, int b);
    extern int _ZN8dActor_c15IsPlayerInRangeEi(void *thiz, int r);

    if (_ZNK10dBgCh_Actr10IsOnGroundEv(self + 0x144) != 0) {
        int *p = (int *)(((int)self + 0x98));
        *p += 0x19000;
        *(int *)(self + 0xa8) = 0;
    } else {
        int t = (int)(((s64)*(int *)(self + 0x98) * 0xfae + 0x800) >> 12);
        *(int *)(self + 0x98) = _ZN4cstd4fdivEii(t, 0x1000);
    }
    if (*(int *)(self + 0x98) > 0x28000) {
        *(int *)(self + 0x98) = 0x28000;
    }
    if (_ZN8dActor_c15IsPlayerInRangeEi(self, 0x1388) == 0) {
        *(int *)(self + 0x388) = 2;
    }
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020afd10
/* Dispatch-table index 2. */
extern "C" {
void func_ov002_020afd10(char* c)
{
    extern int _ZN8dActor_c15IsPlayerInRangeEi(char* thiz, int r);
    extern void func_ov002_020aefb8(char* thiz);
    extern void func_ov002_020afc68(char* thiz);
    extern void func_ov002_020af3a8(char* thiz);
    extern void func_ov002_020aeee4(char* thiz);
    extern int func_ov002_020af218(char* thiz, int n);
    extern int func_ov002_020af248(char* thiz, int n);

    volatile Fix12i v[3];

    if (*(int*)(c + 0x388) != 0) {
        v[0] = *(Fix12i*)(c + 0xa4);
        v[1] = *(Fix12i*)(c + 0xa8);
        v[2] = *(Fix12i*)(c + 0xac);
        func_ov002_020aefb8(c);
        *(Fix12i*)(c + 0xa4) = v[0];
        *(Fix12i*)(c + 0xac) = v[2];
    }

    switch (*(int*)(c + 0x388)) {
    case 0:
        func_ov002_020af218(c, 0xbb8);
        if (_ZN8dActor_c15IsPlayerInRangeEi(c, 0x3e8)) {
            *(Fix12i*)(c + 0x9c) = -0x4000;
            *(int*)(c + 0x388) = 1;
        }
        break;
    case 1:
        func_ov002_020afc68(c);
        break;
    case 2:
        func_ov002_020af248(c, 0x1e);
        break;
    }

    func_ov002_020af3a8(c);
    func_ov002_020aeee4(c);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020afde4
extern "C" {
void func_ov002_020afde4(char* c){
  extern char* _ZN8dActor_c13ClosestPlayerEv(char*);
  extern short Vec3_HorzAngle(void*, void*);
  extern void func_ov002_020af3a8(char*);
  extern int _ZNK10dBgCh_Actr8IsOnWallEv(char*);
  extern int _ZN8dActor_c15IsPlayerInRangeEi(char*, int);
  char* p = _ZN8dActor_c13ClosestPlayerEv(c);
  if(p){
    *(short*)(c+0x94) = Vec3_HorzAngle(c+0x5c, p+0x5c) + 0x8000;
  }
  func_ov002_020af3a8(c);
  if(_ZNK10dBgCh_Actr8IsOnWallEv(c+0x144)) *(int*)(c+0x388)=2;
  if(_ZN8dActor_c15IsPlayerInRangeEi(c, 0xbb8)==0) *(int*)(c+0x388)=2;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020afe4c
/* Dispatch-table index 1. */
extern "C" {
void func_ov002_020afe4c(char* c) {
    extern void func_ov002_020aefb8(char* thiz);
    extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned a, void* v);
    extern void func_ov002_020af474(char* thiz);
    extern void func_ov002_020afde4(char* thiz);
    extern int func_ov002_020af248(char* thiz, int n);
    extern void func_ov002_020af3a8(char* thiz);
    extern int func_ov002_020af218(char* thiz, int n);
    extern void func_ov002_020aeee4(char* thiz);

    func_ov002_020aefb8(c);
    switch (*(int*)(c+0x388)) {
    case 0:
        if (*(unsigned short*)(c+0x100) == 0) {
            _ZN5Sound9PlayBank3EjRK7Vector3(0x68, c+0x74);
        }
        func_ov002_020af474(c);
        if (*(unsigned short*)(c+0x100) == 0x25) {
            *(int *)(((int)c + 0x128)) &= ~1;
            *(int*)(c+0x388) = 1;
            *(int*)(c+0x98) = 0x8000;
        }
        break;
    case 1:
        func_ov002_020afde4(c);
        break;
    case 2:
        func_ov002_020af248(c, 0x1e);
        func_ov002_020af3a8(c);
        break;
    }
    func_ov002_020af218(c, 0xbb8);
    func_ov002_020aeee4(c);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020aff10
/* Dispatch-table index 0 -- the state the actor starts in. */
extern "C" {
void func_ov002_020aff10(char* c){
  extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int a, void* v);
  extern void func_ov002_020aefb8(char* thiz);
  extern void func_ov002_020af474(char* thiz);
  extern void func_ov002_020af3a8(char* thiz);
  extern void func_ov002_020aeee4(char* thiz);
  extern void func_ov002_020af248(char* thiz, int n);
  extern void func_ov002_020af218(char* thiz, int n);

  func_ov002_020aefb8(c);
  switch(*(int*)(c+0x388)){
  case 0:
    if(*(unsigned short*)(c+0x100) == 0) _ZN5Sound9PlayBank3EjRK7Vector3(0x68, c+0x74);
    func_ov002_020af474(c);
    if(*(unsigned short*)(c+0x100) != 0x25) break;
    *(int*)(((int)c + 0x128)) &= ~1;
    *(int*)(c+0x388) = 1;
    *(int*)(c+0x98) = 0x2000;
    break;
  case 1:
    if(*(unsigned short*)(c+0x100) > 0x12c) *(int*)(c+0x388) = 2;
    func_ov002_020af3a8(c);
    break;
  case 2:
    func_ov002_020af248(c, 0x1e);
    func_ov002_020af3a8(c);
    break;
  }
  func_ov002_020af218(c, 0xbb8);
  func_ov002_020aeee4(c);
}
}

/* The second file-scope `extern "C"` region, for the five class members below.
   Every one of these is spelt as the shard that uses it recovered it; none of
   the 29 members above declares any of them, so nothing here overrides a
   recovered view. `func_ov002_020af4ec` needs no entry -- ordinal 13's
   definition is already visible with C linkage. */
extern "C" {
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* thiz, dActor_c* a, int r, int h, unsigned int e, unsigned int g);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* thiz, dActor_c* a, int r, int h, Vector3_16* p, int q);
int IsStarCollectedInCurLevel(int a);
}

/* Ordinal 34 dispatches through a pointer-to-member-function, and mwccarm's
   representation of one depends on the class it names, so the shard's own
   opaque stand-in is kept rather than pointed at the real da1up_c: merging a
   shared shadow struct is a codegen hazard, measured on ov006/dScMgSound_c. */
struct C;
typedef void (C::*PMF)();
struct C {
  char pad[0x500];
};

/* Ordinal 35's view of data_ov002_0210d9b8: a cached model handle whose second
   word is the BMD file pointer. */
struct ModelCache { int pad0; BMD_File* file; };

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c16CleanupResourcesEv
/* Vtable slot 3. */
int da1up_c::CleanupResources()
{
  extern SharedFilePtr data_ov002_0210d9d8;
  extern SharedFilePtr data_ov002_0210da30;

  int s = mMushroomType;
  if (s != 0xb && s != 0xc){
    int b = (actorID == 0x114);
    if (b != 0) data_ov002_0210d9d8.Release();
    else data_ov002_0210da30.Release();
  }
  if ((unsigned int)(mMushroomType - 0xb) <= 1)
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd2, mPosX, mPosY + 0x28000, mPosZ);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c16OnPendingDestroyEv
/* Vtable slot 12. One instruction: bx lr. */
void da1up_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c6RenderEv
/* Vtable slot 9. */
int da1up_c::Render()
{
    if (unk_38e == 0 || unk_38f == 0)
        return 1;
    {
        int b = (mFlags & 0x40000) ? 1 : 0;
        if (b)
            return 1;
    }
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c8BehaviorEv
/* Vtable slot 6, and the ONLY reader of the 14-element dispatch array at
   0x0210dc00 anywhere in the ROM besides __sinit_ov002_02100adc, which fills it
   from the 14 descriptors at 0x02108300..0x02108370. mMushroomType is the
   index. The array stays `extern`: this TU claims .text only, so the sinit, the
   descriptors and the array itself remain their own shards. */
int da1up_c::Behavior()
{
  extern PMF data_ov002_0210dc00[];

  if(UpdateYoshiEat(mWithMeshClsn) != 0){
    func_ov002_020af4ec(((char*)this));
    mdCcAc_c.Clear();
    return 1;
  }
  mEatingPlayer = 0;
  {
    int old = unk_388;
    C* self = (C*)((char*)this);
    (self->*data_ov002_0210dc00[mMushroomType])();
    /* Named ++mStateTimer / mStateTimer = 0 size-DIFF vs this recovered
       unsigned-short launder; keep MATCH form. */
    ++*(unsigned short*)((void*)(int)(((char*)this) + 0x100));
    ++unk_38c;
    if(old != unk_388){
      *(unsigned short*)((void*)(int)(((char*)this) + 0x100)) = 0;
      unk_38c = 0;
    }
  }
  mdCcAc_c.Clear();
  mdCcAc_c.Update();
  func_ov002_020af4ec(((char*)this));
  return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c13InitResourcesEv
/* Vtable slot 0, the largest member in the TU, and -- because the destructor is
   declared out of line above it -- NOT this class's key function. */
int da1up_c::InitResources()
{
    extern ModelCache data_ov002_0210d9b8;
    extern SharedFilePtr data_ov002_0210d9d8;
    extern SharedFilePtr data_ov002_0210da30;
    extern signed char data_0209f2f8;
    extern unsigned char data_0209f220;
    extern unsigned char data_ov002_020ff040[];
    extern unsigned char data_ov002_020ff050[];

    BMD_File* f;
    int isKind0, isKind115;

    mMushroomType = param1 & 0xf;

    isKind0 = (actorID == 0x114);
    if (isKind0) {
        if ((unsigned int)(mMushroomType - 0xb) <= 1) {
            if (mModel.SetFile(data_ov002_0210d9b8.file, 1, 1) == 0)
                return 0;
        } else {
            f = (BMD_File*)Model::LoadFile(data_ov002_0210d9d8);
            if (mModel.SetFile(f, 1, 1) == 0)
                return 0;
        }
    } else {
        if ((unsigned int)(mMushroomType - 0xb) <= 1) {
            if (mModel.SetFile(data_ov002_0210d9b8.file, 1, 1) == 0)
                return 0;
        } else {
            f = (BMD_File*)Model::LoadFile(data_ov002_0210da30);
            if (mModel.SetFile(f, 1, 1) == 0)
                return 0;
        }
    }

    if (mShadowModel.InitCylinder() == 0)
        return 0;

    if (mMushroomType == 6 || mMushroomType == 8 || (unsigned int)(mMushroomType - 0xb) <= 1) {
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this) + 0x110, (dActor_c*)((char*)this), 0x64000, 0x40000, 0x100002, 0);
        if ((unsigned int)(mMushroomType - 0xb) <= 1) {
            mdCcAc_c.vulnFlags |= 0x8000;
        }
    } else {
        isKind115 = (actorID == 0x115);
        if (isKind115) {
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this) + 0x110, (dActor_c*)((char*)this), 0x41000, 0x41000, 0x100002, 0);
        } else {
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this) + 0x110, (dActor_c*)((char*)this), 0x32000, 0x32000, 0x100002, 0);
        }
    }

    unk_388 = 0;
    if (data_ov002_020ff040[mMushroomType] == 0) {
        mdCcAc_c.flags |= 1;
    }
    if (data_ov002_020ff050[mMushroomType] == 0) {
        mFlags &= ~1;
    }
    if ((unsigned int)(mMushroomType - 0xb) <= 1) {
        unk_38e = 1;
    } else {
        unk_38e = 0;
    }
    unk_38f = 1;
    unk_390 = ((unsigned int)param1 >> 4) & 0xf;
    unk_378 = mPosX;
    unk_37c = mPosY;
    unk_380 = mPosZ;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x32000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x144, (dActor_c*)((char*)this), 0x32000, 0x32000, 0, 0);
    mWithMeshClsn.SetLimMovFlag();
    unk_394 = 0;

    if (data_0209f2f8 == 7 && mPosY == 0xdac000 && mPosZ == 0
        && (data_0209f220 == 1 || IsStarCollectedInCurLevel(1) == 0)) {
        MarkForDestruction();
        return 0;
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* MEGA_MUSHROOM (277 / SCALEUP_KINOKO). Leaf operator new routes to
   fBase_c::operator new; the implicit constructor inlines the dEnemyBase_c
   base step, vptr store, and the four member constructors. */
// @symbol da1up_c_classInit_SCALEUP_KINOKO
extern "C" da1up_c *da1up_c_classInit_SCALEUP_KINOKO()
{
    return new da1up_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* ONE_UP_MUSHROOM (276 / ONEUPKINOKO). Same class, second profile. */
// @symbol da1up_c_classInit_ONEUPKINOKO
extern "C" da1up_c *da1up_c_classInit_ONEUPKINOKO()
{
    return new da1up_c();
}
