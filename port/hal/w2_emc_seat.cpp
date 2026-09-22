// THE ExtendingMeshCollider SEAT -- storage, 13 slots, and its own carry shim.
//
// This is the class port/slice_w1l4.txt opened by refusing to seat. The refusal
// was right at the time and both of its reasons are now gone:
//
//   "THE PORT HAS NO STORAGE FOR ITS VTABLE" -- include/decl_common.h:2971
//   declares _ZTV14dBgW_KcMbgSclY extern and nothing defines it. This
//   file is that definition.
//
//   "NOTHING DISPATCHES IT, AND NOTHING CAN, on the overlay set the port
//   mounts" -- config/arm9/relocs.txt loads 0x02099490 from exactly three sites
//   (0x0203ab64 D0, 0x0203ab88 D1, 0x0203abac C1, re-read here, still three),
//   and the ONE constructor caller in the whole ROM is ov045 0x02111b00, inside
//   daObjKm2_Nobiru_c_classInit. Wave 3 mounted ov045 and this wave registers
//   EXTENDING_PLATFORM (id 148), so that call site is now in the build and level
//   37 places one of them. The class can be constructed, so its table is
//   dispatched, so it has to hold real bodies.
//
// ---- THE ROM TABLE, re-read from config/arm9/relocs.txt --------------------
//
// 0x02099490, thirteen slots; 0x020994c4 carries no relocation because it is
// the next class's offset-to-top word.
//
//   [0]  0x0203ab68  _ZN14dBgW_KcMbgSclYD1Ev                  OWN
//   [1]  0x0203ab3c  _ZN14dBgW_KcMbgSclYD0Ev                  OWN
//   [2]  0x0203aa0c  _ZN14dBgW_KcMbgSclY9Virtual08Ev          OWN
//   [3]  0x01ffd920  MeshCollider::GetSurfaceInfo         ITCM, inherited
//   [4]  0x0203a4dc  _ZN14dBgW_KcMbgSclY9GetNormalEsR7Vector3 OWN
//   [5]  0x02039e60  MovingMeshCollider::GetTriangleOrigin   inherited
//   [6]  0x0203a8fc  EMC::DetectClsn(RaycastGround &)                OWN
//   [7]  0x0203a594  EMC::DetectClsn(RaycastLine &)                  OWN
//   [8]  0x0203a660  EMC::DetectClsn(SphereClsn &)                   OWN
//   [9]  0x02039438  MeshColliderBase::BeforeClsn            inherited
//   [10] 0x02039930  MovingMeshCollider::TransformPos        inherited
//   [11] 0x02039924  MovingMeshCollider::GetAngularVelY      inherited
//   [12] 0x02039908  MovingMeshCollider::GetVelocity         inherited
//
// SEVEN slots overridden (0, 1, 2, 4, 6, 7, 8), six inherited. Laid beside
// _ZTV10dBgW_KcMbg (0x02099434) word for word, the shape is identical:
// same width, same inherited words at 3/5/9/10/11/12, different bodies at the
// seven. The earlier reading of "four overridden" in slice_w1l4.txt counted the
// D1/D0 pair as inherited; they are not.
//
// ---- THE NUMBERING is hal/mmc_vtable.cpp's, because the shape and the
//      callers are the same ------------------------------------------------
//
// Slots 0..5 are MSVC's (MeshColliderBase.h declares ONE destructor, so MSVC
// spends one slot where the ROM's Itanium layout spends two, and everything
// after it shifts down by one); slots 6..8 are the ROM's, because every
// DetectClsn caller in src is a transcribed shadow class that counts two
// destructor slots. hal/clsn_vtable.cpp pins the same mixture for the seed
// table this one is modelled on.
//
//   host [0]   the destructor, D1/D0 folded
//   host [1]   Virtual08 at the MSVC index
//   host [2]   Virtual08 again, at the ROM index (see below)
//   host [3]   GetNormal      (ROM slot 4, shifted down one by the fold)
//   host [4]   GetTriangleOrigin (ROM slot 5, shifted)
//   host [5]   GetTriangleOrigin again, at the ROM index -- the fill-it-twice
//              hal/mmc_vtable.cpp and hal/clsn_vtable.cpp already use
//   host [6..8]  the three DetectClsn at their ROM indices
//   host [9]   BeforeClsn, with its OWN park-and-restore shim
//   host [10]  GetAngularVelY  [11] GetVelocity  [12] GetVelocity again
//
// WHY [1] AND [2] BOTH TAKE Virtual08. The MMC table gets away with leaving [1]
// alone because it is a COPY of MeshCollider's seeded table and slot 1 already
// held MeshCollider::Virtual08 (hal/clsn_vtable.cpp's slot_v08). This array is
// FRESH AND ZEROED -- nothing seeds it -- so every slot has to be written or it
// is a call through null, not a base-class body. [1] is the MSVC index for
// Virtual08 and [2] is the ROM shadow's; the class's own Virtual08 is the right
// answer at both. What [2] costs is MSVC's GetSurfaceInfo index, and that is
// free here: the ROM's slot 3 is the ITCM body 0x01ffd920 and the port hosts no
// TU for it, so the index was never going to hold anything anyway -- the same
// trade hal/mmc_vtable.cpp makes.
//
// ---- THE CARRY-SLOT TRAP, and why this file gets its OWN shim -------------
//
// hal/mmc_vtable.cpp explains the index skew in full: the four MATCHED
// MeshColliderBase carry callbacks are MSVC-compiled against MeshColliderBase.h
// and emit their virtual calls one index LOW (UpdatePosWithTransform reaches
// TransformPos through index 9), while the ROM shadows read the ROM index. The
// one genuine collision is index 9, where the ROM shadow wants BeforeClsn and
// the MSVC carry wants TransformPos. mmc_beforeclsn resolves it by parking
// TransformPos in index 9 for the duration of the callback.
//
// mmc_beforeclsn writes _ZTV10dBgW_KcMbg[9] BY NAME. An
// ExtendingMeshCollider instance dispatches through THIS array, so reusing that
// shim would park slot 9 of the wrong table and leave BeforeClsn sitting in the
// index UpdatePosWithTransform is about to call here -- the recursion that
// produced the lift-ride crash, and on this class it would be a rider standing
// on the stretching platform, which is the entire point of EXTENDING_PLATFORM.
// emc_beforeclsn below is the same shim against this file's own storage.
//
// ---- WHAT ARRIVES WITH IT -------------------------------------------------
//
// func_0203aa74, func_0203aa10 and func_0203abb0 were unlinked and every call
// site of all three lies inside EMC's own GetNormal and DetectClsn bodies, so
// they come in with the bodies and needed no separate argument. func_020374b8,
// func_020375ec and func_02035394 were already in walk_window.map (the
// MovingMeshCollider DetectClsn triple names them). func_01ffb0fc is ITCM and
// hal/method_faces.cpp hosts it.
#include <cstdio>
#include "dsstate_seg.h"

#include "dBgW_KcMbgSclY.h"

extern "C" {
/* THE STORAGE. include/decl_common.h:2971 declares this extern and no object
   anywhere defined it; src/_ZN14dBgW_KcMbgSclYC1Ev.cpp, D1 and D0 all
   store it into the object, so it has to exist before the class can be built.
   Thirteen slots, the ROM width. It is mutable game state a save state rolls
   back like every other hosted table, so it goes in .dsstate. */
DSSTATE_BEGIN
void *_ZTV14dBgW_KcMbgSclY[13];
DSSTATE_END

/* The seven own bodies. Virtual08 and GetNormal are real C++ methods against
   ExtendingMeshCollider.h (faced at the bottom of this file); the rest are
   extern-C free definitions in src. */
void _ZN14dBgW_KcMbgSclY9Virtual08Ev(void *self);
void _ZN14dBgW_KcMbgSclY9GetNormalEsR7Vector3(void *self, s16 tri,
                                                     Vector3 *res);
void *_ZN14dBgW_KcMbgSclYD1Ev(void *self);            /* ROM slot 0 */
void *_ZN14dBgW_KcMbgSclYD0Ev(void *self);            /* ROM slot 1 */
int _ZN14dBgW_KcMbgSclY10DetectClsnER9dBgCh_Gnd(void *self,
                                                            void *ground);
int _ZN14dBgW_KcMbgSclY10DetectClsnER9dBgCh_Lin(void *self,
                                                          void *line);
int _ZN14dBgW_KcMbgSclY10DetectClsnER12dBgCh_SphCrr(void *self,
                                                         void *sphere);
}
/* The four inherited MovingMeshCollider words (ROM slots 5, 10, 11, 12) are the
   same MSVC methods hal/mmc_vtable.cpp seats, so the veneers below call them
   QUALIFIED through MovingMeshCollider.h rather than by an Itanium C name that
   nothing defines.

   EMC's own DetectClsn(RaycastGround &) spells the base line-walk it delegates
   to by its Itanium name; the port hosts that ITCM body (0x01ffb0fc) under the
   ROM-address spelling in hal/method_faces.cpp, which is the same function, so
   the two names are bound rather than a second face written. */
#pragma comment(linker, "/alternatename:__ZN7dBgW_Kc10DetectClsnER9dBgCh_Lin=_func_01ffb0fc")

typedef dBgW_KcMbg MMC;

/* ---- the __fastcall veneers ---------------------------------------------- */
/* Slot 0 is the MSVC scalar deleting destructor: one slot for the ROM's D1/D0
   pair. The ROM's own deleting body (D0, 0x0203ab3c) is the one that frees, and
   it is what a `delete` through this table has to reach -- the same choice
   hal/mmc_vtable.cpp makes with _ZN10dBgW_KcMbgD0Ev. Both bodies take
   `this` as an ordinary argument (flat C), so the veneer moves ecx across. */
static void __fastcall emc_dtor(void *s, void *)
{ _ZN14dBgW_KcMbgSclYD0Ev(s); }
static void __fastcall emc_v08(void *s, void *)
{ _ZN14dBgW_KcMbgSclY9Virtual08Ev(s); }
static void __fastcall emc_norm(void *s, void *, s16 tri, Vector3 *res)
{ _ZN14dBgW_KcMbgSclY9GetNormalEsR7Vector3(s, tri, res); }
static void __fastcall emc_orig(void *s, void *, s16 tri, Vector3 *res)
{ ((MMC *)s)->MMC::GetTriangleOrigin(tri, *res); }
static int __fastcall emc_ground(void *s, void *, void *g)
{ return _ZN14dBgW_KcMbgSclY10DetectClsnER9dBgCh_Gnd(s, g); }
static int __fastcall emc_line(void *s, void *, void *r)
{ return _ZN14dBgW_KcMbgSclY10DetectClsnER9dBgCh_Lin(s, r); }
static int __fastcall emc_sphere(void *s, void *, void *sp)
{ return _ZN14dBgW_KcMbgSclY10DetectClsnER12dBgCh_SphCrr(s, sp); }
static int __fastcall emc_tpos(void *s, void *, const Vector3 *p, Vector3 *r)
{ return ((MMC *)s)->MMC::TransformPos(*p, *r); }
static s16 __fastcall emc_angvel(void *s, void *)
{ return ((MMC *)s)->MMC::GetAngularVelY(); }
static void __fastcall emc_vel(void *s, void *, Vector3 *r)
{ ((MMC *)s)->MMC::GetVelocity(*r); }

/* Slot 9, BeforeClsn -- inherited from MeshColliderBase (ROM 0x02039438), with
   this table's OWN park-and-restore. Identical in shape to mmc_beforeclsn and
   deliberately NOT a call into it: the array named on the two lines below is
   what makes the two shims different functions. */
static void __fastcall emc_beforeclsn(void *s, void *, dBgPi *res,
                                      dActor_c *actor, Vector3 *pos,
                                      Vector3_16 *motionAng, Vector3_16 *ang)
{
    dBgW *base = (dBgW *)s;
    void *saved9 = _ZTV14dBgW_KcMbgSclY[9];
    _ZTV14dBgW_KcMbgSclY[9] = (void *)emc_tpos;
    base->beforeClsnCallback(base, actor, res, pos, motionAng, ang);
    _ZTV14dBgW_KcMbgSclY[9] = saved9;
}

/* EVERY SLOT IS WRITTEN. This array is fresh zeroed storage, not a seeded copy,
   so an unwritten slot is a call through null rather than a base-class body --
   the one real difference from hal/mmc_vtable.cpp's fill, which only writes what
   it changes. */
extern "C" void hal_seat_extending_mesh_collider_vtable(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    void **vt = _ZTV14dBgW_KcMbgSclY;
    vt[0]  = (void *)emc_dtor;
    vt[1]  = (void *)emc_v08;      /* MSVC index for Virtual08 */
    vt[2]  = (void *)emc_v08;      /* ROM index for Virtual08 */
    vt[3]  = (void *)emc_norm;     /* ROM slot 4, MSVC index 3 */
    vt[4]  = (void *)emc_orig;     /* ROM slot 5, MSVC index 4 */
    vt[5]  = (void *)emc_orig;     /* ...and again at the ROM index */
    vt[6]  = (void *)emc_ground;
    vt[7]  = (void *)emc_line;
    vt[8]  = (void *)emc_sphere;
    vt[9]  = (void *)emc_beforeclsn;
    vt[10] = (void *)emc_angvel;
    vt[11] = (void *)emc_vel;
    vt[12] = (void *)emc_vel;
}

/* ---- method faces --------------------------------------------------------
   Virtual08 and GetNormal are the two EMC bodies src defines as real C++
   methods (ExtendingMeshCollider::Virtual08 / ::GetNormal), so they mangle as
   MSVC methods and the C names above need a bridge. The other five own bodies
   are extern-C free definitions and need none -- the header says so in as many
   words ("SetFile and the DetectClsn triple are launder-heavy, so those
   definitions stay extern-C free functions"). */
extern "C" {
void _ZN14dBgW_KcMbgSclY9Virtual08Ev(void *self)
{ ((dBgW_KcMbgSclY *)self)->dBgW_KcMbgSclY::Virtual08(); }
void _ZN14dBgW_KcMbgSclY9GetNormalEsR7Vector3(void *self, s16 tri,
                                                     Vector3 *res)
{ ((dBgW_KcMbgSclY *)self)->dBgW_KcMbgSclY::GetNormal(tri, *res); }
}
