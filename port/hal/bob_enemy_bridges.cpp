// GATE 32: the linkage seam for Bob-omb Battlefield's cast.
//
// Same two jobs hal/cxx_aliases.cpp and hal/actor_class_faces.cpp already do,
// kept in a file of this gate's own so the merge with the other actor stream
// is a file add rather than a diff through two shared files.
//
//   * a C++-MANGLED REFERENCE onto a C-named definition gets a linker alias.
//     Both sides are cdecl and describe the same ROM function; the mangling
//     differs only because the referencing TU declared the symbol inside a
//     namespace or a shadow class instead of extern "C".
//   * a __thiscall METHOD reference gets a real definition against a shadow
//     class of the same name, because an alias cannot bridge ecx-vs-stack.
//
// Nothing here invents behaviour. The two exceptions are called out where they
// are, and both are the ROM's own answer rather than a guess.
#include <cstdio>
#include <cstdlib>

extern "C" {
/* the C-named definitions the aliases below land on */
void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);      /* cxxname_bridge */
char *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);  /* player_bridges */
void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned, unsigned,
                                                   const void *, const void *,
                                                   int, int);
unsigned _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned, unsigned, unsigned,
                                           const void *, unsigned);
void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned, unsigned, int, int, int, const void *, void *);
int _ZNK12WithMeshClsn13JustHitGroundEv(void *self);
int _ZNK12WithMeshClsn10IsOnGroundEv(const void *self);
void _ZN5Actor8PoofDustEv(void *self);
int func_ov002_020ada40(void *self, void *v, void *other, unsigned r);
extern int data_020ad560[];        /* the cylinder template BMD (stub bytes) */
}

/* ---- static/namespace functions: same ABI, different spelling -------------
   BobOmb::InitResources declares Animation::LoadFile returning void and
   Model::LoadFile returning BMD_File*; the definitions return char* and void*.
   One register either way. Enemy::SpawnCoin's Actor::Spawn and
   func_ov102_0214b248's Sound::PlayLong are the same shape, and
   Enemy::SpawnMegaCharParticles declares Particle::System::New by its ITANIUM
   name inside C++ without extern "C", so MSVC mangles the mangled name. */
#pragma comment(linker, "/alternatename:?LoadFile@Animation@@SAXAAUSharedFilePtr@@@Z=__ZN9Animation8LoadFileER13SharedFilePtr")
#pragma comment(linker, "/alternatename:?LoadFile@Model@@SAPAUBMD_File@@AAUSharedFilePtr@@@Z=__ZN5Model8LoadFileER13SharedFilePtr")
#pragma comment(linker, "/alternatename:?Spawn@Actor@@SAPAU1@IIABUVector3@@PBUVector3_16@@HH@Z=__ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii")
#pragma comment(linker, "/alternatename:?PlayLong@Sound@@YAIIIIABUVector3@@I@Z=__ZN5Sound8PlayLongEjjjRK7Vector3j")
#pragma comment(linker, "/alternatename:?_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE@@YAPAXIIHHHPBXPAX@Z=__ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE")

/* ---- the ov002 name without its overlay tag -------------------------------
   BobOmb::Behavior (and four other levels' classes) spell ov002 0x020ada40 as
   `func_020ada40`, with no overlay in the name. The reloc settles which one it
   is: ov102 0x0214c328 targets 0x020ada40 in module overlays(2,4), and ov002
   is the overlay this port mounts. */
#pragma comment(linker, "/alternatename:_func_020ada40=_func_ov002_020ada40")

/* ---- data: the same object under a different declared type ---------------- */
#pragma comment(linker, "/alternatename:?data_ov102_0214e9c0@@3USharedFilePtr@@A=_data_ov102_0214e9c0")
#pragma comment(linker, "/alternatename:?data_ov102_0214e9c8@@3USharedFilePtr@@A=_data_ov102_0214e9c8")
#pragma comment(linker, "/alternatename:?data_ov102_0214e9c8@@3UG2@@A=_data_ov102_0214e9c8")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9e0@@3USharedFilePtr@@A=_data_ov002_0210d9e0")
#pragma comment(linker, "/alternatename:?data_02082128@@3US48@@A=_data_02082128")
#pragma comment(linker, "/alternatename:?data_ov002_020ff014@@3GA=_data_ov002_020ff014")
#pragma comment(linker, "/alternatename:?data_ov002_0210dbc0@@3PAP8Enemy@@AEHAAUWithMeshClsn@@@ZA=_data_ov002_0210dbc0")

// ---- __thiscall method faces ------------------------------------------------
//
// Each is a real definition against a shadow class with the ROM's own name, so
// the decorated symbol comes out identical to the one the caller emitted. The
// bodies forward to the C-named definition the port already has.

/* WithMeshClsn2: the name func_ov102_0214aa18's own TU gives the collider at
   +0x144. Both methods are the plain WithMeshClsn ones. */
struct WithMeshClsn2 { int JustHitGround() const; int IsOnGround() const; };
int WithMeshClsn2::JustHitGround() const
{ return _ZNK12WithMeshClsn13JustHitGroundEv((void *)this); }
int WithMeshClsn2::IsOnGround() const
{ return _ZNK12WithMeshClsn10IsOnGroundEv((const void *)this); }

/* Actor::PoofDust, reached as a method by Enemy::SpawnCoin. */
struct Actor { void PoofDust(); };
void Actor::PoofDust() { _ZN5Actor8PoofDustEv(this); }

/* Enemy's two methods that the rest of the Enemy tier reaches by their Itanium
   C names. Their own TUs define them as real methods over locally-declared
   classes, so this is the ordinary face direction, not an alias. */
struct Enemy {
    void SpawnCoin();
    void SpawnMegaCharParticles(Actor &a, char *p);
};
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN5Enemy9SpawnCoinEv(void *self)
{ ((Enemy *)self)->Enemy::SpawnCoin(); }
#else
extern "C" void _ZN5Enemy9SpawnCoinEv(void *self);  /* Linux: real symbol from src/_ZN5Enemy9SpawnCoinEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN5Enemy22SpawnMegaCharParticlesER5ActorPc(void *self,
                                                            void *a, char *p)
{ ((Enemy *)self)->Enemy::SpawnMegaCharParticles(*(Actor *)a, p); }
#else
extern "C" void _ZN5Enemy22SpawnMegaCharParticlesER5ActorPc(void *self,
                                                            void *a, char *p);  /* Linux: real symbol from src/_ZN5Enemy22SpawnMegaCharParticlesER5ActorPc */
#endif /* _WIN32 */

/* Player::IncMegaKillCount, which the knockback at ov002 0x020ada40 runs when
   a mega-form Mario is the one that hit the enemy. Its own TU defines it
   against include/Player.h; the caller spells the C name. */
struct Player { void IncMegaKillCount(); };
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN6Player16IncMegaKillCountEv(void *self)
{ ((Player *)self)->Player::IncMegaKillCount(); }
#else
extern "C" void _ZN6Player16IncMegaKillCountEv(void *self);  /* Linux: real symbol from src/_ZN6Player16IncMegaKillCountEv */
#endif /* _WIN32 */

// ---- the two RIDE-THROUGH returns ------------------------------------------
//
// ModelBase::SetFile and ShadowModel::InitCylinder both end in a TAIL BRANCH on
// the ROM, so r0 carries the callee's result out even though the decomp
// declares both `void`. Every caller the port had before ignored the value;
// BobOmb::InitResources is the first that tests it, twice:
//
//     if (SetFile(bmd, 1, -1) == 0) return 0;
//     if (InitCylinder() == 0)      return 0;
//
// so a void definition and an eax full of whatever the last call left would
// decide whether the bomb initialises at all.
//
// The int-returning spellings the caller emitted are DEFINED here rather than
// aliased onto the void ones. Both are __thiscall against the ROM's own class
// name, so the decorated symbols match exactly.

/* SetFile is `DoSetFile(file, a, b)` and nothing else -- the ROM branches
   straight into it. DoSetFile is slot 1 of the host tables
   (hal/cxxname_bridge.cpp fills _ZTV5Model and _ZTV9ModelAnim in MSVC order:
   dtor 0, DoSetFile 1, UpdateVerts 2, Virtual10 3, Render 4), and every slot
   there is a __fastcall thunk. Dispatching it here is what returns the real
   value. */
struct BMD_File;
struct ModelBase { int SetFile(BMD_File *file, int a, int b); };

typedef int(__fastcall *PortDoSetFile)(void *, void *, char *, int, int);

int ModelBase::SetFile(BMD_File *file, int a, int b)
{
    void **vt = *(void ***)this;
    if (!vt || !vt[1]) {
        std::fprintf(stderr, "FATAL: ModelBase::SetFile on %p: vtable %p has "
                     "no DoSetFile\n", (void *)this, (void *)vt);
        std::abort();
    }
    return ((PortDoSetFile)vt[1])(this, 0, (char *)file, a, b);
}

/* InitCylinder is `SetFile(&data_020ad560, 1, -1)`, and THE SHADOW SYSTEM IS
   DEFERRED: hal/cxxname_bridge.cpp records the three reasons (ov001 is not
   mounted so the template BMD's bytes are absent, _ZTV11ShadowModel is eight
   nulls, and no per-frame RenderAll caller is hosted), and
   hal/player_bridges.cpp already stubs the C-named spelling of this same
   function to nothing. So this one does not call SetFile either -- it returns
   the ROM's success value, 1, which is what the callers test for. When the
   shadow system lands this becomes the same dispatch as SetFile above. */
struct ShadowModel { int InitCylinder(); };
int ShadowModel::InitCylinder() { return 1; }

// ---- gate 32's ov084 tier ---------------------------------------------------
//
// The GOOMBA is the first class the port carries that wears Mario's cap, plays
// a material animation and casts its own ground ray, so three more method
// faces come with it. All three are the ordinary direction: the definitions
// are real MSVC methods against include/ and the callers spell the C name.
#include "BgCh.h"
#include "MaterialChanger.h"

extern "C" {
/* MaterialChanger: two of its three are methods (SetFile already defines the
   C name in its own TU). */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File(void *self, void *bmd,
                                                       void *bma)
{ ((MaterialChanger *)self)->MaterialChanger::Prepare(*(BMD_File *)bmd,
                                                      *(BMA_File *)bma); }
#else
void _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File(void *self, void *bmd,
                                                       void *bma);  /* Linux: real symbol from src/_ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN15MaterialChanger6UpdateER15ModelComponents(void *self, void *model)
{ ((MaterialChanger *)self)->MaterialChanger::Update(*(ModelComponents *)model); }
#else
void _ZN15MaterialChanger6UpdateER15ModelComponents(void *self, void *model);  /* Linux: real symbol from src/_ZN15MaterialChanger6UpdateER15ModelComponents */
#endif /* _WIN32 */
}

/* func_ov084_02129238 builds a RaycastGround on its own stack and arms it
   through three flag setters it declares as RaycastGround methods. They are
   BgCh's -- RaycastGround carries a BgCh at its own offset 0, which is why the
   ROM's inlined stores are the same three words -- and BgCh's own definitions
   are matched src, in the build since gate 10. Shadow definitions rather than
   aliases, because both sides are __thiscall. */
struct RaycastGround {
    void StartDetectingWater();
    void StartDetectingToxic();
    void StopDetectingOrdinary();
};
void RaycastGround::StartDetectingWater()
{ ((BgCh *)this)->BgCh::StartDetectingWater(); }
void RaycastGround::StartDetectingToxic()
{ ((BgCh *)this)->BgCh::StartDetectingToxic(); }
void RaycastGround::StopDetectingOrdinary()
{ ((BgCh *)this)->BgCh::StopDetectingOrdinary(); }

/* ov084 data under a second declared type, the cxx_aliases direction. */
#pragma comment(linker, "/alternatename:?data_ov084_02130cf8@@3PADA=_data_ov084_02130cf8")
#pragma comment(linker, "/alternatename:?data_ov084_02130278@@3PAPAXA=_data_ov084_02130278")
#pragma comment(linker, "/alternatename:?data_ov002_02110304@@3UState@@A=_data_ov002_02110304")

/* ---- the CAP TIER's method faces ------------------------------------------
   Nine of CapEnemy's methods are real MSVC methods against include/CapEnemy.h
   while every caller in ov084 spells the Itanium C name. */
#include "CapEnemy.h"
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN8CapEnemy21DestroyIfCapNotNeededEv(void *self)
{ return ((CapEnemy *)self)->CapEnemy::DestroyIfCapNotNeeded(); }
#else
int _ZN8CapEnemy21DestroyIfCapNotNeededEv(void *self);  /* Linux: real symbol from src/_ZN8CapEnemy21DestroyIfCapNotNeededEv */
#endif /* _WIN32 */
void *_ZN8CapEnemy15RespawnIfHasCapEv(void *self)
{ return ((CapEnemy *)self)->CapEnemy::RespawnIfHasCap(); }
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN8CapEnemy12Unk_02005d94Ev(void *self)
{ ((CapEnemy *)self)->CapEnemy::Unk_02005d94(); }
#else
void _ZN8CapEnemy12Unk_02005d94Ev(void *self);  /* Linux: real symbol from src/_ZN8CapEnemy12Unk_02005d94Ev */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN8CapEnemy14RenderCapModelEPK7Vector3(void *self, const void *v)
{ ((CapEnemy *)self)->CapEnemy::RenderCapModel((const Vector3 *)v); }
#else
void _ZN8CapEnemy14RenderCapModelEPK7Vector3(void *self, const void *v);  /* Linux: real symbol from src/_ZN8CapEnemy14RenderCapModelEPK7Vector3 */
#endif /* _WIN32 */
}

/* WithMeshClsn's flag helper. Its sibling Unk_0203589c is NOT declared in
   include/WithMeshClsn.h -- its own TU invents a shadow for it -- so that one
   is in hal/bob_enemy_shadow_faces.cpp with the other two of its kind. */
#include "WithMeshClsn.h"
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN12WithMeshClsn22ClearJustHitGroundFlagEv(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::ClearJustHitGroundFlag(); }
#else
void _ZN12WithMeshClsn22ClearJustHitGroundFlagEv(void *self);  /* Linux: real symbol from src/_ZN12WithMeshClsn22ClearJustHitGroundFlagEv */
#endif /* _WIN32 */
}

/* ---- three more names spelled without their overlay, or with the wrong one -
   func_020ff028 is data_ov002_020ff028 (the six per-character cap
   SharedFilePtrs, read as an array of pointers); data_ov000_020ab3c4 is
   func_ov001_020ab3c4, and the reloc at arm9 0x02006574 says module:overlay(1)
   outright; func_020aea30 is ov002's, and the port hosts that one
   (port/unmatched/Enemy_UpdateDeath.cpp). */
#pragma comment(linker, "/alternatename:_func_020ff028=_data_ov002_020ff028")
#pragma comment(linker, "/alternatename:_data_ov000_020ab3c4=_func_ov001_020ab3c4")
#pragma comment(linker, "/alternatename:_func_020aea30=_func_ov002_020aea30")
#pragma comment(linker, "/alternatename:?data_0209fc68@@3HA=_data_0209fc68")
#pragma comment(linker, "/alternatename:?data_0209f2d8@@3HA=_data_0209f2d8_c")

extern "C" {
/* data_0208a0e0 (the live player-slot count func_02005e28 walks when the
   character is mega) is arm9 .data and comes from romdata.py, which emits
   the ROM's own byte. This gate defined it by hand as well and the two
   collided at link; the ROM copy is the one that cannot drift. */

/* data_0209f344 is the VS-mode star-order POINTER (u8*) in bss, not a goomba
   bookkeeping word. Stage::InitResources:427 seats it at runtime to
   &VS_STAR_SPAWN_ORDERS[func_0203dad4() % 6] -- one 12-byte row of the
   versus per-round star-id permutation table (arm9 data 0x02075720). The port
   hand-rolls its boot and never runs InitResources, so this stayed a zeroed
   host and read back as a NULL pointer; StarMarker::Behavior, and the ov002/
   ov084 star-progress checks, all index data_0209f344[data_0209f208] and
   faulted (SIG-2, WF star mission 2). Hosted as a real pointer here and seated
   in level_boot.cpp the way InitResources does. VS_STAR_SPAWN_ORDERS is the
   ROM's own bytes (0x02075720, 6 rows x 12; each row starts with 0, so the
   single-player index 0 yields star-id 0 and the "is this my star" compare is
   inert for every real marker -- identical observable behavior to hardware). */
unsigned char VS_STAR_SPAWN_ORDERS[6][0xC] = {
    { 0, 1, 2, 3, 4, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 3, 2, 4, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 2, 1, 3, 4, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 2, 3, 1, 4, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 3, 2, 1, 4, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 3, 1, 2, 4, 0, 0, 0, 0, 0, 0, 0 },
};
unsigned char *data_0209f344;

/* the red-coin counter NumRedCoins reads: DEFINED in hal/auto_bss.cpp, where
   this gate's 4-byte sizing was carried over. */
}
/* CapEnemy's own vtable is ov002 0x02108284, mounted with the rest of that
   overlay's data. The constructor installs it and the derived factory
   overwrites it two lines later, so nothing is ever dispatched through it --
   the same reading data_ov002_021081e4 (Enemy's) already has. Its D0 spells it
   by the RTTI name, so both spellings have to resolve to one object. */
#pragma comment(linker, "/alternatename:__ZTV11dCapEnemy_c=_data_ov002_02108284")
#pragma comment(linker, "/alternatename:?data_0209f344@@3PAEA=_data_0209f344")
#pragma comment(linker, "/alternatename:?data_0209f284@@3EA=_data_0209f284")
#pragma comment(linker, "/alternatename:?data_0209d6d4@@3GA=_data_0209d6d4")
#pragma comment(linker, "/alternatename:?data_ov084_02130d9c@@3HA=_data_ov084_02130d9c")

/* THE REST OF THIS GATE'S FACES ARE IN TWO OTHER FILES, and the split is
   forced rather than tidy. A face has to be compiled next to the class the
   caller named, and this TU has already spelled shadow Actor, Enemy, Player,
   ModelBase and ShadowModel above -- including include/Camera.h or Player.h
   here would redefine every one of them. So:
     * hal/bob_enemy_shadow_faces.cpp holds the faces whose class is a SHADOW
       in both TUs and includes nothing at all;
     * hal/bob_enemy_header_faces.cpp holds the ones whose class is in
       include/ and includes only those headers. */

/* ---- gate 32's last three overlays ---------------------------------------
   Three more kinds of the same two problems.

   THE OVERLAY TAG IS WRONG IN FOUR NAMES. ov014, ov021, ov022 and ov034 are
   all linked at the same DS base, so dsd's per-overlay naming can attach a
   reference to the wrong one: ChainChomp's two destructors spell their own
   vtable data_ov034_021147ec, and ChainChompFence's InitResources spells three
   ov014 symbols with ov021 and ov022 tags. Every one is settled by ADDRESS --
   0x021147ec is _ZTV10ChainChomp in ov014, 0x021149b8/0x021149c0 are ov014 bss
   and 0x02114558 is ov014 data -- and by the reloc, which names overlay(14).

   ChainChomp_Spawn's `func_020aed98` is NOT one of these and is NOT aliased.
   It is the same address as _ZN5EnemyC2Ev, but the source calls it with no
   argument and relies on the r0 ride-through, so an alias would hand the
   constructor stack garbage for `this`. That factory is a host copy instead --
   port/unmatched/ChainChomp_Spawn.cpp says what the measurement was. */
#pragma comment(linker, "/alternatename:_data_ov034_021147ec=__ZTV10ChainChomp")
#pragma comment(linker, "/alternatename:?data_ov021_021149b8@@3PAHA=_data_ov014_021149b8")
#pragma comment(linker, "/alternatename:_data_ov021_021149c0=_data_ov014_021149c0")
#pragma comment(linker, "/alternatename:?data_ov022_02114558@@3PAHA=_data_ov014_02114558")
/* The chomp's two animation SharedFilePtrs, ov014 0x02114970 and 0x02114980,
   are spelled with a different type in every TU that reaches them -- `char`,
   `int[]`, `void*[]` and a local two-word `struct S` -- so MSVC decorates the
   same object four ways. One definition, four names.
   func_ov019_02111f54 is the wrong-overlay-tag case again: chomp state 2's
   main half calls it and the reloc at 0x02111afc names overlay(14). */
#pragma comment(linker, "/alternatename:?data_ov014_02114970@@3DA=_data_ov014_02114970")
#pragma comment(linker, "/alternatename:?data_ov014_02114980@@3DA=_data_ov014_02114980")
#pragma comment(linker, "/alternatename:?data_ov014_02114970@@3US@@A=_data_ov014_02114970")
#pragma comment(linker, "/alternatename:?data_ov014_02114980@@3US@@A=_data_ov014_02114980")
#pragma comment(linker, "/alternatename:_func_ov019_02111f54=_func_ov014_02111f54")
#pragma comment(linker, "/alternatename:?data_ov062_0211e004@@3PADA=_data_ov062_0211e004")
#pragma comment(linker, "/alternatename:?data_ov062_0211e004@@3USharedFilePtr@@A=_data_ov062_0211e004")
#pragma comment(linker, "/alternatename:?data_ov062_0211e00c@@3PADA=_data_ov062_0211e00c")
#pragma comment(linker, "/alternatename:?data_ov062_0211e00c@@3USharedFilePtr@@A=_data_ov062_0211e00c")
#pragma comment(linker, "/alternatename:?data_ov062_0211e014@@3PADA=_data_ov062_0211e014")
#pragma comment(linker, "/alternatename:?data_ov062_0211e014@@3USharedFilePtr@@A=_data_ov062_0211e014")
#pragma comment(linker, "/alternatename:?data_ov062_0211e01c@@3PADA=_data_ov062_0211e01c")
#pragma comment(linker, "/alternatename:?data_ov062_0211e01c@@3USharedFilePtr@@A=_data_ov062_0211e01c")
#pragma comment(linker, "/alternatename:?data_ov062_0211e024@@3PADA=_data_ov062_0211e024")
#pragma comment(linker, "/alternatename:?data_ov062_0211e024@@3USharedFilePtr@@A=_data_ov062_0211e024")
#pragma comment(linker, "/alternatename:?data_ov062_0211e02c@@3PADA=_data_ov062_0211e02c")
#pragma comment(linker, "/alternatename:?data_ov062_0211e02c@@3USharedFilePtr@@A=_data_ov062_0211e02c")
#pragma comment(linker, "/alternatename:?data_ov062_0211e034@@3PADA=_data_ov062_0211e034")
#pragma comment(linker, "/alternatename:?data_ov062_0211e034@@3USharedFilePtr@@A=_data_ov062_0211e034")
#pragma comment(linker, "/alternatename:?data_ov062_0211e03c@@3PADA=_data_ov062_0211e03c")
/* ---- KOOPA_FLAG's three ---------------------------------------------------
   Its InitResources declares Animation::LoadFile returning void* where
   BobOmb's declares it returning void and the definition returns char*; one
   register in all three. Its two SharedFilePtrs and the level timer its
   Behavior stops are the declared-type case again -- data_0209d4c8 is arm9
   bss (hal/auto_bss.cpp) and the flag's TU spells it `extern char[]` in C++. */
#pragma comment(linker, "/alternatename:?LoadFile@Animation@@SAPAXAAUSharedFilePtr@@@Z=__ZN9Animation8LoadFileER13SharedFilePtr")
#pragma comment(linker, "/alternatename:?data_ov062_0211e0d4@@3USharedFilePtr@@A=_data_ov062_0211e0d4")
#pragma comment(linker, "/alternatename:?data_ov062_0211e0dc@@3USharedFilePtr@@A=_data_ov062_0211e0dc")
#pragma comment(linker, "/alternatename:?data_0209d4c8@@3PADA=_data_0209d4c8")
/* ---- KOOPA_THE_QUICK's own six states -------------------------------------
   Two more spellings of the same two SharedFilePtrs, and cstd::atan2 named
   without its Fix12 typedefs by the state that turns him toward the player. */
#pragma comment(linker, "/alternatename:?data_ov062_0211e034@@3PAHA=_data_ov062_0211e034")
#pragma comment(linker, "/alternatename:?data_ov062_0211e03c@@3PAHA=_data_ov062_0211e03c")
#pragma comment(linker, "/alternatename:?atan2@cstd@@YAHHH@Z=__ZN4cstd5atan2E5Fix12IiES1_")

/* ---- KING_BOB_OMB's own two problems --------------------------------------
   THE OVERLAY TAG IS WRONG ONCE MORE. Two of his states call ov078 0x02123804
   and dsd named that address for ov080 as well, so the C spells it
   _ZN13MontyMoleRockD0Ev. The reloc names overlay(78) and the body is his own
   throw, not a destructor.

   WithMeshClsn_IsOnGround is the same object under a different spelling: the
   ROM's method is _ZNK12WithMeshClsn10IsOnGroundEv and one of his TUs declares
   it as a plain C function under a hand-written name.

   The rest is his thirteen SharedFilePtrs and two state records, each spelled
   with whatever type its TU happened to declare. */
#pragma comment(linker, "/alternatename:__ZN13MontyMoleRockD0Ev=_func_ov078_02123804")
#pragma comment(linker, "/alternatename:_WithMeshClsn_IsOnGround=__ZNK12WithMeshClsn10IsOnGroundEv")
#pragma comment(linker, "/alternatename:?data_ov078_0212710c@@3UPMF@@A=_data_ov078_0212710c")
#pragma comment(linker, "/alternatename:?data_ov078_0212709c@@3PADA=_data_ov078_0212709c")
#pragma comment(linker, "/alternatename:?data_ov078_02126ee8@@3PAPAHA=_data_ov078_02126ee8")
#pragma comment(linker, "/alternatename:?data_ov078_02126ee0@@3USharedFilePtr@@A=_data_ov078_02126ee0")
#pragma comment(linker, "/alternatename:?data_ov078_02126ee8@@3USharedFilePtr@@A=_data_ov078_02126ee8")
#pragma comment(linker, "/alternatename:?data_ov078_02126ef0@@3USharedFilePtr@@A=_data_ov078_02126ef0")
#pragma comment(linker, "/alternatename:?data_ov078_02126ef8@@3USharedFilePtr@@A=_data_ov078_02126ef8")
#pragma comment(linker, "/alternatename:?data_ov078_02126f00@@3USharedFilePtr@@A=_data_ov078_02126f00")
#pragma comment(linker, "/alternatename:?data_ov078_02126f08@@3USharedFilePtr@@A=_data_ov078_02126f08")
#pragma comment(linker, "/alternatename:?data_ov078_02126f10@@3USharedFilePtr@@A=_data_ov078_02126f10")
#pragma comment(linker, "/alternatename:?data_ov078_02126f18@@3USharedFilePtr@@A=_data_ov078_02126f18")
#pragma comment(linker, "/alternatename:?data_ov078_02126f20@@3USharedFilePtr@@A=_data_ov078_02126f20")
#pragma comment(linker, "/alternatename:?data_ov078_02126f28@@3USharedFilePtr@@A=_data_ov078_02126f28")
#pragma comment(linker, "/alternatename:?data_ov078_02126f30@@3USharedFilePtr@@A=_data_ov078_02126f30")
#pragma comment(linker, "/alternatename:?data_ov078_02126f38@@3USharedFilePtr@@A=_data_ov078_02126f38")
#pragma comment(linker, "/alternatename:?data_ov078_02126f40@@3USharedFilePtr@@A=_data_ov078_02126f40")
#pragma comment(linker, "/alternatename:?data_ov078_0212703c@@3HA=_data_ov078_0212703c")
#pragma comment(linker, "/alternatename:?data_ov078_02126f30@@3PAHA=_data_ov078_02126f30")
#pragma comment(linker, "/alternatename:?data_02082128@@3UMatrix4x3@@A=_data_02082128")
#pragma comment(linker, "/alternatename:?data_ov062_0211e03c@@3USharedFilePtr@@A=_data_ov062_0211e03c")

/* BlendModelAnim::SetAnim takes THIS FILE'S SECOND RULE, not its first.
   func_ov078_02123bc4 (the king's throw state) declares a local shadow
   `struct BlendModelAnim { int SetAnim(BCA_File &, int, int, Fix12, unsigned
   short); }` and calls it as a METHOD, so the reference MSVC emits is
   __thiscall: the object goes in ecx and only the five declared arguments are
   pushed. The matched body, src/_ZN14BlendModelAnim7SetAnimER8BCA_Fileii
   5Fix12IiEt.cpp, is extern "C" and therefore cdecl: it reads SIX arguments
   off the stack and never looks at ecx.

   This pair used to be joined with an /alternatename, which the linker accepts
   and which is silently wrong -- an alias relabels a symbol, it cannot bridge
   ecx-vs-stack. Every call through the method spelling landed one stack slot
   out: the body took the caller's BCA_File* as its `this` and the caller's
   numBlendFrames literal as its BCA_File*, wrote that literal over the real
   animation file's +0x60, then dereferenced it. The king's throw passes 4, so
   the read came out at 0x00000006 and faulted -- level 6's most-reported
   crash, and the reason the king froze and vanished. ov062 and ov066 spell
   the same method the same way and were queued behind the same defect.

   A real definition against a shadow class of the same name is what actually
   converts the convention. */
struct BCA_File;
struct BlendModelAnim {
    int SetAnim(BCA_File &file, int numBlendFrames, int flags, int speed,
                unsigned short startFrame);
};
extern "C" void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
    void *thiz, void *file, int numBlendFrames, int flags, int speed,
    unsigned short startFrame);

int BlendModelAnim::SetAnim(BCA_File &file, int numBlendFrames, int flags,
                            int speed, unsigned short startFrame)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        this, &file, numBlendFrames, flags, speed, startFrame);
    /* the ROM body sets no return value and every caller of this spelling
       discards it; func_ov078_02123bc4 returns its own literal 1. */
    return 0;
}
