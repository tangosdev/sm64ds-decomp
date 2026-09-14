// Lane FACES4, run link100 wave 9c: thirty rows of walk_window's wall that are
// NAME rows, not body rows.
//
// WHY A LANE-OWNED FILE. Every row below belongs in a file this lane does not
// own: the twenty-three adapters belong in hal/dtor_faces_cpp.h and the call
// sites in fourteen hal/actor_classes_*.cpp, the five IRQ/CP15 rows belong in
// hal/cxx_aliases.cpp, and the two faces belong in hal/reverse_bridges.cpp and
// hal/method_faces.cpp. No lane owns those this wave and the house law is that
// a lane stays inside its own files, so the rows are written here, ADDITIVELY,
// through port/slice_faces4.txt and one CMake block. Each says what the tidy
// version of itself is, so whoever next owns those files can move it in one
// pass. Nothing here adds a body the ROM does not have.
//
// =========================================================================
// 1. THE TWENTY-THREE RENAMED DESTRUCTOR ADAPTERS
// =========================================================================
//
// hal/dtor_faces_cpp.cpp defines one slot-16 adapter per seated class through
// its CPPD1 macro, and the macro spells the adapter's name out of the CLASS
// name: CPPD1(Coin) defines hal_cppd1_Coin. The main to port sync RENAMED
// twenty-three of those classes (the decomp's own names landed: Flag became
// daMcFlag_c, InvisiblePole became daCamTag_c, and so on), and the sync
// rewrote the macro invocations because they have to name a class that exists.
// It did NOT rewrite hal/dtor_faces_cpp.h, which still declares the adapters
// under their old names, nor the fourteen hal/actor_classes_*.cpp fillers that
// write `vt[16] = (void *)hal_cppd1_<old>`. So twenty-three adapters are
// defined under one name and asked for under another: twenty-three rows of the
// wall, every one a C++-linkage __fastcall free function on both sides.
//
// AN /alternatename IS ADMISSIBLE HERE AND IS NOT A METRIC TRICK. The standing
// test (out/HALROWS/settled.txt section B) is that an alias is a NAME bridge
// and never an ABI bridge, so the two sides have to agree about the call
// already. They do, exactly: both spellings are `int __fastcall (void *,
// void *)`, YIHPAX0 on both sides of every row, because they are the SAME
// MACRO EXPANSION under two names. There is no receiver to misplace and no
// stack to unbalance.
//
// THE MAPPING IS NOT GUESSED, and it was derived twice.
//   * The sync commit 4462caf68 rewrote the CPPD1 list in place, so its own
//     diff of hal/dtor_faces_cpp.cpp pairs each removed line with the added
//     one. SnowmanBody went dormant in that commit and came back as
//     daBgSnmBdy_c in 4fd18fd27, which is the one pair that takes two commits
//     to read. The list is 65 entries long before and after and exactly 23
//     differ.
//   * Independently, thirteen of the twenty-three are confirmed BY ADDRESS by
//     an /alternatename row this tree already carries on the two classes'
//     VTABLE symbols: __ZTV10daMcFlag_c=__ZTV4Flag,
//     __ZTV11daObjFire_c=__ZTV9BlueFlame,
//     __ZTV12daObjClock_c=__ZTV22ClockPaintingHandShort and ten more. A shared
//     vtable object is one class. All thirteen agree with the diff.
//
// THE TIDY VERSION: rename the declaration in hal/dtor_faces_cpp.h and the one
// `vt[16] =` line at each call site, and delete this section. Until then
// port/tools/alternatename_guard.py is the net: the day anything DEFINES one
// of these old names, the guard fails the build on that row by design.
#pragma comment(linker, "/alternatename:?hal_cppd1_Flag@@YIHPAX0@Z=?hal_cppd1_daMcFlag_c@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_VirtualDoor@@YIHPAX0@Z=?hal_cppd1_daChScene_c@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_PoppingLavaBubbles@@YIHPAX0@Z=?hal_cppd1_daObjWaterfall_c@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_AmbientSoundEffects@@YIHPAX0@Z=?hal_cppd1_daSetSE_c@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_Amp@@YIHPAX0@Z=?hal_cppd1_daBrq_c@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_FlameChomp@@YIHPAX0@Z=?hal_cppd1_daKrpa_c@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_FlameChompFire@@YIHPAX0@Z=?hal_cppd1_daKpFr_c@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_MrI_Projectile@@YIHPAX0@Z=?hal_cppd1_daEyBm_c@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_SnowmanBody@@YIHPAX0@Z=?hal_cppd1_daBgSnmBdy_c@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_InvisibleSecret@@YIHPAX0@Z=?hal_cppd1_Number@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_IceSlideManager@@YIHPAX0@Z=?hal_cppd1_daSldMng_c@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_MegaMushroomCreateTag@@YIHPAX0@Z=?hal_cppd1_daObjKinokoTag_c@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_ClockPaintingHandShort@@YIHPAX0@Z=?hal_cppd1_daObjClock_c@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_SlideDecorationSilverStar@@YIHPAX0@Z=?hal_cppd1_daObjHsBillboard_c@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_RotatingPlatformRr@@YIHPAX0@Z=?hal_cppd1_daObjRc_Hane_c@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_OrangeBallBillboard@@YIHPAX0@Z=?hal_cppd1_daObjKb1Billboard_c@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_Pokey@@YIHPAX0@Z=?hal_cppd1_daSanbo_c@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_Moneybag@@YIHPAX0@Z=?hal_cppd1_daGmch_c@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_PushBlock@@YIHPAX0@Z=?hal_cppd1_PowerFlower@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_BlueFlame@@YIHPAX0@Z=?hal_cppd1_daObjFire_c@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_Seaweed@@YIHPAX0@Z=?hal_cppd1_HealingHeart@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_ArrowLift@@YIHPAX0@Z=?hal_cppd1_daObjWc_Obj03_c@@YIHPAX0@Z")
#pragma comment(linker, "/alternatename:?hal_cppd1_InvisiblePole@@YIHPAX0@Z=?hal_cppd1_daCamTag_c@@YIHPAX0@Z")

// =========================================================================
// 2. FIVE ROWS WHERE A CALLER SPELLS A ROM FREE FUNCTION AT C++ LINKAGE
// =========================================================================
//
// Each is __cdecl with no receiver on BOTH sides, which is the whole
// admissibility test for an alias. The right hand side is the ROM's own flat
// name and every one of them is DEFINED in this link, measured with dumpbin
// over all 8662 of walk_window's link inputs. The left hand side is how a
// caller TU spelled it: either as a namespace free function (Y) or as a static
// member of a class of the same name (S). Both are __cdecl, the A, so no
// receiver moves and no stack changes.
//
//   ?DisableIRQs@IRQ@@YAII@Z        namespace IRQ, free  -> _ZN3IRQ11DisableIRQsEj
//   ?DisableIRQs@IRQ@@SAII@Z        class IRQ, static    -> the same body
//   ?EnableIRQs@IRQ@@YAII@Z                              -> _ZN3IRQ10EnableIRQsEj
//   ?GetIRQHandler@IRQ@@YAP6AXXZI@Z                      -> _ZN3IRQ13GetIRQHandlerEj
//   ?WaitForInterrupt@CP15@@SAXXZ                        -> _ZN4CP1516WaitForInterruptEv
//
// THREE SIBLINGS ARE REFUSED HERE AND SAY WHY. ?SetIRQs@IRQ@@SAII@Z has the
// same shape, but _ZN3IRQ7SetIRQsEj (arm9 0x02056e18) is NOT in this link, so
// the alias would resolve to nothing. ?Spawn@Actor@@SAPAU1@IIABUVector3@@
// PBUVector3_16@@HH@Z and ?_ZN8dActor_c15FindWithActorIDEjPS_@dActor_c@@
// SAPAU1@IPAU1@@Z are the same case: static, admissible in shape, and the ROM
// body is not in the link. All three are seat rows, not alias rows.
#pragma comment(linker, "/alternatename:?DisableIRQs@IRQ@@YAII@Z=__ZN3IRQ11DisableIRQsEj")
#pragma comment(linker, "/alternatename:?DisableIRQs@IRQ@@SAII@Z=__ZN3IRQ11DisableIRQsEj")
#pragma comment(linker, "/alternatename:?EnableIRQs@IRQ@@YAII@Z=__ZN3IRQ10EnableIRQsEj")
#pragma comment(linker, "/alternatename:?GetIRQHandler@IRQ@@YAP6AXXZI@Z=__ZN3IRQ13GetIRQHandlerEj")
#pragma comment(linker, "/alternatename:?WaitForInterrupt@CP15@@SAXXZ=__ZN4CP1516WaitForInterruptEv")

// =========================================================================
// 3. TWO REAL FACES, because an alias here WOULD be an ABI bridge
// =========================================================================
//
// Both left hand sides are __thiscall (QAE): the receiver rides ECX and the
// callee pops. The bodies they want are flat __cdecl with the receiver as the
// first stack argument. That is failure mode 3 in hal/method_faces.cpp's own
// header and the 2026-08-07 door-open crash, so each is a real forwarder.

// ?DetectClsn@RaycastLine@@QAEHXZ. hal/reverse_bridges.cpp DECLARES this member
// on its RaycastLine shadow, which is the host name for dBgCh_Lin, and never
// defined it, so the reference has been sitting on the wall unanswered and the
// only object that asks for it is that same file. The body is dBgCh_Lin's own,
// arm9 0x02038638, defined in this link under its flat name. The shadow is
// spelled here exactly as reverse_bridges.cpp spells it, which is what makes
// this the missing definition of that declaration rather than a second one.
extern "C" int _ZN9dBgCh_Lin10DetectClsnEv(void *self);
struct RaycastLine {
    int DetectClsn();
};
/* PORT_HOST_ABI: __thiscall receiver in ECX forwarded to the cdecl flat name. */
int RaycastLine::DetectClsn() { return _ZN9dBgCh_Lin10DetectClsnEv(this); }

// ?_ZN5Model12SetPolygonIDEi@Model@@QAEXH@Z. src/game/actors/d_a_obj_kumo.cpp
// declares a Model shadow whose MEMBER IS LITERALLY NAMED with the ROM's
// Itanium string and calls it as a method, so MSVC emits a __thiscall
// reference to a member of Model called _ZN5Model12SetPolygonIDEi. The body is
// the flat _ZN5Model12SetPolygonIDEi, arm9 0x02016abc, in this link, and it
// takes the receiver first: src/func_ov006_020c092c.cpp:18 spells it
// `void _ZN5Model12SetPolygonIDEi(Model *, int)` and three more src TUs agree.
// The tidy version is for that TU to declare the flat name extern "C" the way
// its siblings do; until then this is the bridge.
extern "C" void _ZN5Model12SetPolygonIDEi(void *self, int id);
struct Model {
    void _ZN5Model12SetPolygonIDEi(int id);
};
/* PORT_HOST_ABI: the same receiver move as above. */
void Model::_ZN5Model12SetPolygonIDEi(int id)
{ ::_ZN5Model12SetPolygonIDEi(this, id); }

// =========================================================================
// 4. EIGHT PARTICLE CALLBACKS THAT DO NOT OVERRIDE SpawnParticles
// =========================================================================
//
// src/_ZN8Particle10SysTrackerC1Ev.cpp builds the thirteen particle callback
// vtables and is the only object on this wall that asks for any of the eight
// names below. Each is a DERIVED callback class's SpawnParticles, and
// config/**/symbols.txt has no such function at any address: the ROM names
// SpawnParticles on five callback classes only (the base callback_c,
// simpleCallback_c, scaleCallback_c, checkYoganCallback_c and
// edStarKiraCallback_c). So facegen refuses all eight under rule 1, correctly,
// and the answer is not a ROM address for the derived name. It is that THESE
// CLASSES DO NOT OVERRIDE IT.
//
// THE ROM'S OWN VTABLES SAY SO, word for word. The thirteen tables are four
// words apart from _ZTVN5dPa_c7level_c16bubbleCallback_cE at arm9 0x0208f3a4;
// word 0 is SpawnParticles and word 1 is OnUpdate. Read out of
// config/arm9/relocs.txt, every one of the eight has its OWN OnUpdate in word 1
// and a BASE CLASS's body in word 0:
//
//   table                        word 0 -> body            word 1
//   bubbleCallback_c   0x0208f3a4  0x02022640 simpleCallback_c  its own
//   splashCallback_c   0x0208f3e4  0x02022640 simpleCallback_c  its own
//   fitWaterSimpleCallback_c 0x0208f444 0x02022640 simple...     its own
//   checkWaterCallback_c 0x0208f3f4 0x020226d0 callback_c        its own
//   checkWaterRippleCallback_c 0x0208f424 0x020226d0 callback_c  its own
//   clipCallback_c     0x0208f434  0x020226d0 callback_c         its own
//   fitWaterCallback_c 0x0208f454  0x020226d0 callback_c         its own
//   cleanParticleCallback_c 0x0208f464 0x020226d0 callback_c     its own
//
// That is exactly what a C++ class that inherits a virtual without overriding
// it produces, so a face that forwards the derived name to the base body
// reproduces the ROM's own dispatch rather than inventing one. Both base
// bodies are defined in this link under their flat names
// (port/unmatched/Particle_RideThroughs.cpp hosts simpleCallback_c's, which
// carries an ARM r1 ride-through, and the base callback_c's is a four-byte
// stub). Nothing here changes a dispatch that exists: all eight names are
// undefined at this tip, so nothing can currently reach them at all.
//
// THE TIDY VERSION is for src to stop declaring an override these classes do
// not have, which is a matching question and is written down rather than tried.
namespace Particle { struct System; }
struct dPa_c { struct level_c; };
struct dPa_c::level_c {
    struct bubbleCallback_c;
    struct splashCallback_c;
    struct fitWaterSimpleCallback_c;
    struct checkWaterCallback_c;
    struct checkWaterRippleCallback_c;
    struct clipCallback_c;
    struct fitWaterCallback_c;
    struct cleanParticleCallback_c;
};
extern "C" {
void _ZN5dPa_c7level_c16simpleCallback_c14SpawnParticlesERN8Particle6SystemE(
    void *self, void *sys);
void _ZN5dPa_c7level_c10callback_c14SpawnParticlesERN8Particle6SystemE(
    void *self, void *sys);
}

struct dPa_c::level_c::bubbleCallback_c
{ virtual void SpawnParticles(Particle::System &sys); };
struct dPa_c::level_c::splashCallback_c
{ virtual void SpawnParticles(Particle::System &sys); };
struct dPa_c::level_c::fitWaterSimpleCallback_c
{ virtual void SpawnParticles(Particle::System &sys); };
struct dPa_c::level_c::checkWaterCallback_c
{ virtual void SpawnParticles(Particle::System &sys); };
struct dPa_c::level_c::checkWaterRippleCallback_c
{ virtual void SpawnParticles(Particle::System &sys); };
struct dPa_c::level_c::clipCallback_c
{ virtual void SpawnParticles(Particle::System &sys); };
struct dPa_c::level_c::fitWaterCallback_c
{ virtual void SpawnParticles(Particle::System &sys); };
struct dPa_c::level_c::cleanParticleCallback_c
{ virtual void SpawnParticles(Particle::System &sys); };

/* the three that inherit simpleCallback_c's body, ROM word 0 = 0x02022640 */
void dPa_c::level_c::bubbleCallback_c::SpawnParticles(Particle::System &sys)
{ _ZN5dPa_c7level_c16simpleCallback_c14SpawnParticlesERN8Particle6SystemE(this, &sys); }
void dPa_c::level_c::splashCallback_c::SpawnParticles(Particle::System &sys)
{ _ZN5dPa_c7level_c16simpleCallback_c14SpawnParticlesERN8Particle6SystemE(this, &sys); }
void dPa_c::level_c::fitWaterSimpleCallback_c::SpawnParticles(Particle::System &sys)
{ _ZN5dPa_c7level_c16simpleCallback_c14SpawnParticlesERN8Particle6SystemE(this, &sys); }

/* the five that inherit the base callback_c's, ROM word 0 = 0x020226d0 */
void dPa_c::level_c::checkWaterCallback_c::SpawnParticles(Particle::System &sys)
{ _ZN5dPa_c7level_c10callback_c14SpawnParticlesERN8Particle6SystemE(this, &sys); }
void dPa_c::level_c::checkWaterRippleCallback_c::SpawnParticles(Particle::System &sys)
{ _ZN5dPa_c7level_c10callback_c14SpawnParticlesERN8Particle6SystemE(this, &sys); }
void dPa_c::level_c::clipCallback_c::SpawnParticles(Particle::System &sys)
{ _ZN5dPa_c7level_c10callback_c14SpawnParticlesERN8Particle6SystemE(this, &sys); }
void dPa_c::level_c::fitWaterCallback_c::SpawnParticles(Particle::System &sys)
{ _ZN5dPa_c7level_c10callback_c14SpawnParticlesERN8Particle6SystemE(this, &sys); }
void dPa_c::level_c::cleanParticleCallback_c::SpawnParticles(Particle::System &sys)
{ _ZN5dPa_c7level_c10callback_c14SpawnParticlesERN8Particle6SystemE(this, &sys); }
