"""Generate hal/dtor_forwarders_gen*.cpp: the INLINE-DESTRUCTOR FORWARDERS.

Run link100 wave 9b, lane DTORS-A.  tools/dtor_store_guard.py is the authority
for the PORT_HOST_ABI ruling that makes these admissible and states the evidence;
this file only builds what that ruling allows, and asks it about every row.

THE PROBLEM.  192 rows on walk_window's link wall are flat Itanium destructor
names whose owning src TU IS compiled into the binary and does not define them.
main's header declares the destructor inline (`virtual ~daCamTag_c() {}`).
mwccarm emits the body out of line because the class's vtable odr-uses it; MSVC
emits nothing, because the port's vtables are hand-written arrays in
hal/actor_classes_*.cpp and nothing on the host odr-uses the destructor.  So
there is no definition for a face to bind to and no body for it to call.

THE ANSWER.  One generated TU that INCLUDES THE REAL HEADER and defines the flat
ROM name as a forwarder making the qualified call:

    extern "C" void _ZN10daCamTag_cD1Ev(void *self)
    { ((daCamTag_c *)self)->daCamTag_c::~daCamTag_c(); }

The call odr-uses the inline destructor, so MSVC emits it, and the forwarder is
the ROM's own flat name so the port's callers resolve.  QUALIFIED on purpose:
`self->~daCamTag_c()` on a class with a virtual destructor is a VIRTUAL call,
which would dispatch through the port's ROM-shaped table at slot 16 straight back
into this same symbol.  hal/dtor_faces_cpp.cpp's adapters use the qualified
spelling for exactly that reason and this file follows it.

The D0 half is the same call plus the one deallocation the cartridge's own D0
body makes, READ OFF THAT BODY, never defaulted -- getting the heap wrong is heap
corruption, which is why facegen's D0 rule reads it too:

    extern "C" void _ZN10daCamTag_cD0Ev(void *self)
    { ((daCamTag_c *)self)->daCamTag_c::~daCamTag_c();
      _ZN6Memory10DeallocateEPvP4Heap(self, GAME_HEAP_PTR); }

NOTHING IS DERIVED FROM A NAME.  Per row, before anything is emitted:
  * the flat name must have ONE ROM address with a size (facegen._span_of);
  * dtor_store_guard.check_name must return ok, which re-disassembles the
    cartridge body and applies the ruling;
  * the class must be declared in EXACTLY ONE header under include/, and that
    header must spell the destructor inline -- the whole premise of the row;
  * a D0 row's body must end in exactly ONE call after the base restore, that
    call must be one of the two deallocations a D0 may add, and the heap word
    must come from its own pooled load.  Where config has both the D0 and the
    D1, facegen.d0_reloc_proof re-proves "D0 is D1 plus one deallocation" from
    config/**/relocs.txt as a second, independent net.
Any row that fails any of these is REFUSED with the reason and left on the wall.

    python dtorfwd.py <root> --emit          write hal/dtor_forwarders_gen*.cpp
    python dtorfwd.py <root> --verify        regenerate and diff, nonzero on drift
    python dtorfwd.py <root> --conflicts     the static duplicate-type scan
    python dtorfwd.py --selftest             the fixture battery
"""
import os
import re
import sys

# ---------------------------------------------------------------------------
# THE ROWS.  The flat ROM names this file defines, one per line.
#
# The set is lane FACES3's inline_in_header.txt measurement -- walk_window's own
# unresolved list, restricted to flat Itanium destructor names whose owning src
# TU is on a live slice row -- taken in batches so each build gate carries a
# batch and a batch that breaks a link can be dropped without the others.  A
# name is only ever REMOVED from here; adding one needs the guard's ok and a
# build.
#
# STRUCK OFF BATCH 3 after gate 1 measured them, and this is the one failure
# mode the mechanism has.  Emitting a class's inline destructor makes MSVC emit
# that class's vftable too, and the vftable references EVERY virtual slot.  Two
# classes have a slot whose body exists in this link only under the ROM's flat
# name, so the decorated one is undefined and the emission adds a wall row:
#
#   _ZN4ToadD0Ev       ?Behavior@Toad@@UAEHXZ      the body is _ZN4Toad8BehaviorEv
#                                                  (ov085 0x02122cf8, 0x204),
#                                                  defined in hal/Ov085_Behaviors.cpp
#   _ZN8daTree_cD0Ev   ?Behavior@daTree_c@@UAEHXZ  the body is _ZN8daTree_c8BehaviorEv
#   _ZN8daTree_cD1Ev                               (ov002 0x020b1898, 0x54),
#                                                  defined in src/game/actors/d_a_tree.cpp
#
# Gate 1's own line for each, from build_dtorsa_1.log:
#   dtor_forwarders_gen.cpp.obj : error LNK2001: unresolved external symbol
#   "public: virtual int __thiscall Toad::Behavior(void)" (?Behavior@Toad@@UAEHXZ)
#
# Both are one facegen REVERSE FACE away (the flat name is defined, the
# decorated one is wanted: faces_sync.txt already carries 1,842 rows of exactly
# that shape, and these two were simply never asked for before).  faces_sync.txt
# is frozen for this lane, so the three rows come out rather than being answered
# here, and they come back the moment those two faces exist.

# PUT BACK INTO BATCH 3 at the wave-9c fold (run link100, lane INT4), on the
# coordinator's word. The three rows the note above struck -- _ZN4ToadD0Ev,
# _ZN8daTree_cD0Ev and _ZN8daTree_cD1Ev -- came out only because
# ?Behavior@Toad@@UAEHXZ and ?Behavior@daTree_c@@UAEHXZ had no definition, and
# that note names the condition for their return in so many words: one facegen
# reverse face per slot. Lane FACES4 wrote both this wave, in port/faces_sync.txt,
# so the condition is met and the rows go back.

BATCHES = {}

BATCHES[1] = """
_ZN10dScMgBSC_cD0Ev
_ZN10dScMgBSC_cD1Ev
_ZN10daCamTag_cD0Ev
_ZN10daChRoom_cD0Ev
_ZN10daMcFlag_cD0Ev
_ZN10daPgDfdr_cD0Ev
_ZN10daPgDfdr_cD1Ev
_ZN10daPgMthr_cD0Ev
_ZN10daSldMng_cD0Ev
_ZN11PyramidLiftD0Ev
_ZN11PyramidLiftD1Ev
_ZN11daChScene_cD0Ev
_ZN11daObjFire_cD0Ev
_ZN11daObjKumo_cD0Ev
_ZN11daObjLava_cD0Ev
_ZN11daObjLava_cD1Ev
_ZN11daWarpkun_cD0Ev
_ZN11daWarpkun_cD1Ev
_ZN11dScMgCard_cD0Ev
_ZN11dScMgCard_cD1Ev
_ZN12daDossyCap_cD0Ev
_ZN12daDossyCap_cD1Ev
_ZN12daIDonketu_cD0Ev
_ZN12daIDonketu_cD1Ev
_ZN12daObjAbuku_cD0Ev
_ZN12daObjAbuku_cD1Ev
_ZN12daObjClock_cD0Ev
_ZN12daSoundObj_cD0Ev
_ZN12daSoundObj_cD1Ev
_ZN12dScMgJump2_cD0Ev
_ZN12dScMgJump2_cD1Ev
_ZN12dScMgSound_cD0Ev
_ZN12dScMgSound_cD1Ev
_ZN13PeachPaintingD0Ev
_ZN13PrincessPeachD0Ev
_ZN13daObjDorifu_cD0Ev
_ZN13daObjDorifu_cD1Ev
_ZN13daObjEmmLog_cD0Ev
_ZN13daObjEmmLog_cD1Ev
_ZN13daObjSwdoor_cD0Ev
_ZN13daObjSwdoor_cD1Ev
_ZN13daObjTdFuta_cD0Ev
_ZN13daObjTdFuta_cD1Ev
_ZN13daObjWakame_cD0Ev
_ZN13daObjWakame_cD1Ev
_ZN13dScGameOver_cD0Ev
_ZN13dScGameOver_cD1Ev
_ZN13dScMgMCarlo_cD0Ev
_ZN13dScMgMCarlo_cD1Ev
_ZN14daObjBSwdoor_cD0Ev
_ZN14daObjBSwdoor_cD1Ev
_ZN14daObjC1_Trap_cD0Ev
_ZN14daObjC1_Trap_cD1Ev
_ZN14daObjKsWater_cD0Ev
_ZN14daObjKsWater_cD1Ev
_ZN14daObjMcWater_cD0Ev
_ZN14daObjMcWater_cD1Ev
_ZN14daObjRc_Hane_cD0Ev
_ZN14daObjTdWater_cD0Ev
_ZN14daObjTdWater_cD1Ev
_ZN14daObjWcObj01_cD0Ev
_ZN14daObjWcObj01_cD1Ev
_ZN14daObjWcObj06_cD0Ev
_ZN14daObjWcObj06_cD1Ev
_ZN14dScMgMCarlo2_cD0Ev
_ZN14dScMgMCarlo2_cD1Ev
"""

BATCHES[2] = """
_ZN15daObjIceBoard_cD0Ev
_ZN15daObjIceBoard_cD1Ev
_ZN15daObjKm2_Gura_cD0Ev
_ZN15daObjKm2_Gura_cD1Ev
_ZN15daObjMarioCap_cD0Ev
_ZN15daObjMarioCap_cD1Ev
_ZN15daObjPathLift_cD0Ev
_ZN15daObjPathLift_cD1Ev
_ZN15daObjRcCarpet_cD0Ev
_ZN15daObjRcCarpet_cD1Ev
_ZN15daObjWc_Obj02_cD0Ev
_ZN15daObjWc_Obj02_cD1Ev
_ZN15daObjWc_Obj05_cD0Ev
_ZN15daObjWc_Obj05_cD1Ev
_ZN15daObjWc_Obj07_cD0Ev
_ZN15daObjWc_Obj07_cD1Ev
_ZN15daYurei_Mucho_cD0Ev
_ZN15daYurei_Mucho_cD1Ev
_ZN15dScMgRoulette_cD0Ev
_ZN15dScMgRoulette_cD1Ev
_ZN16dPathLiftActor_cD0Ev
_ZN16dPathLiftActor_cD1Ev
_ZN16daObjBC_Switch_cD0Ev
_ZN16daObjBC_Switch_cD1Ev
_ZN16daObjC0_Switch_cD0Ev
_ZN16daObjC0_Switch_cD1Ev
_ZN16daObjCtMecha03_cD0Ev
_ZN16daObjCtMecha03_cD1Ev
_ZN16daObjCtMecha04_cD0Ev
_ZN16daObjCtMecha04_cD1Ev
_ZN16daObjCtMecha05_cD0Ev
_ZN16daObjCtMecha05_cD1Ev
_ZN16daObjCvShutter_cD0Ev
_ZN16daObjCvShutter_cD1Ev
_ZN16daObjFl_London_cD0Ev
_ZN16daObjFl_London_cD1Ev
_ZN16daObjFm_Battan_cD0Ev
_ZN16daObjFm_Battan_cD1Ev
_ZN16daObjKinokoTag_cD0Ev
_ZN16daObjPushblock_cD0Ev
_ZN16daObjPushblock_cD1Ev
_ZN16daObjRcBuranko_cD0Ev
_ZN16daObjRcBuranko_cD1Ev
_ZN16daObjRc_Dorifu_cD0Ev
_ZN16daObjRc_Dorifu_cD1Ev
_ZN16daObjWaterfall_cD0Ev
_ZN17BigMovingIceBlockD0Ev
_ZN17BigMovingIceBlockD1Ev
_ZN17BowserPuzzlePieceD0Ev
_ZN17BowserPuzzlePieceD1Ev
_ZN17daObjBk_Rotebar_cD0Ev
_ZN17daObjBk_Rotebar_cD1Ev
_ZN17daObjBk_Ukisima_cD1Ev
_ZN17daObjKm1_Dorifu_cD0Ev
_ZN17daObjKm1_Dorifu_cD1Ev
_ZN17daObjKm3_Dorifu_cD0Ev
_ZN17daObjKm3_Dorifu_cD1Ev
_ZN17daObjKm3_Kuruma_cD0Ev
_ZN17daObjKm3_Kuruma_cD1Ev
_ZN17dScMgTrampoline_cD0Ev
_ZN17dScMgTrampoline_cD1Ev
_ZN18daObjBkBillboard_cD1Ev
_ZN18daObjClockHuriko_cD0Ev
_ZN18daObjClockHuriko_cD1Ev
_ZN18daObjHsBillboard_cD0Ev
"""

BATCHES[3] = """
_ZN18daObjMc_Metalnet_cD0Ev
_ZN18daObjMc_Metalnet_cD1Ev
_ZN18daObjRc_Guruguru_cD0Ev
_ZN18daObjRc_Guruguru_cD1Ev
_ZN18dScMgTrampoline2_cD0Ev
_ZN18dScMgTrampoline2_cD1Ev
_ZN19daObjBk_Dossunbar_cD0Ev
_ZN19daObjBk_Dossunbar_cD1Ev
_ZN19daObjHatenaSwitch_cD0Ev
_ZN19daObjHatenaSwitch_cD1Ev
_ZN19daObjKb1Billboard_cD0Ev
_ZN19daObjKm1_Ukishima_cD0Ev
_ZN19daObjKm1_Ukishima_cD1Ev
_ZN19daObjRc_Kaitendai_cD0Ev
_ZN19daObjRc_Kaitendai_cD1Ev
_ZN19daPropeller_Heyho_cD0Ev
_ZN19daPropeller_Heyho_cD1Ev
_ZN20daObjBk_Fall_Block_cD0Ev
_ZN20daObjBk_Fall_Block_cD1Ev
_ZN20daObjCannonShutter_cD0Ev
_ZN20daObjCannonShutter_cD1Ev
_ZN20daObjFl_Fall_Block_cD1Ev
_ZN20daObjKm3_Kaitendai_cD0Ev
_ZN20daObjKm3_Kaitendai_cD1Ev
_ZN20daObjWanwanShutter_cD0Ev
_ZN20daObjWanwanShutter_cD1Ev
_ZN21daObjKm2_Fall_Block_cD0Ev
_ZN21daObjKm2_Fall_Block_cD1Ev
_ZN21daObjKm3_Kurumajiku_cD0Ev
_ZN21daObjKm3_Kurumajiku_cD1Ev
_ZN21daObj_volcanoCannon_cD0Ev
_ZN6CoffinD0Ev
_ZN6CoffinD1Ev
_ZN6DorrieD0Ev
_ZN6DorrieD1Ev
_ZN7daBar_cD0Ev
_ZN7daBar_cD1Ev
_ZN7daBmb_cD0Ev
_ZN7daBmb_cD1Ev
_ZN7daBrq_cD0Ev
_ZN7daDgr_cD0Ev
_ZN7daDgr_cD1Ev
_ZN7daDkk_cD0Ev
_ZN7daDkk_cD1Ev
_ZN8PoleLiftD0Ev
_ZN8PoleLiftD1Ev
_ZN8SignPostD0Ev
_ZN8SignPostD1Ev
_ZN8daEyBm_cD0Ev
_ZN8daKpFr_cD0Ev
_ZN8daKrpa_cD0Ev
_ZN8daSCre_cD0Ev
_ZN8daSCre_cD1Ev
_ZN9KoopaFlagD0Ev
_ZN9LightBeamD0Ev
_ZN9daSCoin_cD0Ev
_ZN9daSCoin_cD1Ev
_ZN9daSetSE_cD0Ev
_ZN4ToadD0Ev
_ZN8daTree_cD0Ev
_ZN8daTree_cD1Ev
"""

# ---------------------------------------------------------------------------
# RUN link100 WAVE 9c, LANE DTORS2.  Two more batches, and a SECOND PREMISE.
#
# Batch 4 is the same premise as batches 1 to 3 and needs no argument of its own:
# these seven rows are classes whose header spells the destructor inline and
# whose owning src TU was NOT compiled at 8dc06444f, so lane DTORS-A could not
# take them.  Lanes SEATS2 and HOSTGEN4 have since seated every one of those TUs
# (d_a_wanwan.cpp, daPkn_c.cpp and the three ov029 water TUs are all on a live
# slice row at 319f0f191, and their objects define the classes' decorated
# virtuals), so the rows are now ordinary inline-in-header rows.
#
# Batch 5 is the SECOND PREMISE, and it is the narrower one.  These classes do
# NOT spell the destructor inline: the header declares it and the owning
# per-function structor TU (src/_ZN<n><Cls>D1Ev.cpp) defines it, and that TU is
# on a live slice row, so `??1<Cls>@@UAE@XZ` is ALREADY IN THE LINK.  Read with
# dumpbin at 319f0f191, one object per class:
#
#   src/_ZN9ModelAnimD1Ev.cpp.obj           ??1ModelAnim@@UAE@XZ
#   src/_ZN15TextureSequenceD1Ev.cpp.obj    ??1TextureSequence@@UAE@XZ
#   src/_ZN18TextureTransformerD1Ev.cpp.obj ??1TextureTransformer@@UAE@XZ
#   src/_ZN11dCapEnemy_cD1Ev.cpp.obj        ??1dCapEnemy_c@@UAE@XZ
#   src/_ZN15dScMgSnowball_cD1Ev.cpp.obj    ??1dScMgSnowball_c@@UAE@XZ
#
# So the forwarder here does not make MSVC EMIT a destructor, it only CALLS one
# that the ROM's own recovered body already provides.  The ROM-side question is
# identical and is asked identically: dtor_store_guard.check_name re-disassembles
# the cartridge body and applies the same ruling, because the port's callers now
# reach MSVC's destructor body and that body still stores its own ??_7Cls@@6B@.
# The host-side premise is what differs, and it is checked by defining_tu()
# below rather than by the header's inline spelling.
#
# WHY THESE FIVE CLASSES AND NOT A SEAT.  out/SEATS2/seat_table.md settles it:
# a per-function structor TU does not define its own flat ROM name under MSVC,
# so compiling one never closed the row it was seated for and 209 of them
# collided.  The TU is already compiled; what is missing is the flat name, which
# is this file's business.
#
# THE D1/D2 ROWS.  MSVC has no D2: for a class with no virtual base its single
# destructor is both the complete-object and the base-object destructor.  That
# is a claim about the cartridge, so it was measured rather than assumed, with
# out/DTORS2/bodydiff.py (out/FACES3/d0diff.py's resolved streams):
#
#   _ZN9ModelAnimD1Ev   0x0201691c  vs _ZN9ModelAnimD2Ev   0x0201689c
#       IDENTICAL, 15 instructions
#   _ZN11dCapEnemy_cD1Ev 0x0200651c vs _ZN11dCapEnemy_cD2Ev 0x020aedbc
#       IDENTICAL, 14 instructions   (the D1 is in arm9, the D2 in ov002)
#
# Only ModelAnim's D2 is on the wall; dCapEnemy_c's D2 is measured because its
# D1 and D2 live in different modules and that is worth proving before its D1 is
# bound to one MSVC symbol.
# STRUCK OFF BATCH 4 after this lane's gate 2 measured it, and it is the same
# failure mode DTORS-A's note above describes, on a third class.  Emitting
# Tornado's inline destructor makes MSVC emit ??_7Tornado@@6B@, which references
# every virtual slot, and two of Tornado's slots have a body in this link only
# under the ROM's FLAT name:
#
#   _ZN7Tornado8BehaviorEv        ov096 0x02137448, 0x11c bytes
#   _ZN7Tornado13InitResourcesEv  ov096 0x02137564, 0x158 bytes
#
# both defined in port/unmatched/Tornado_HostSites.cpp and reached from
# port/hal/actor_classes_ov096.cpp's ROM vtable array through a __fastcall face.
# Gate 2's own lines, from tmp/build_dtors2_2.log:
#
#   dtor_forwarders_gen_w9c.cpp.obj : error LNK2001: unresolved external symbol
#   "public: virtual int __thiscall Tornado::InitResources(void)" (?InitResources@Tornado@@UAEHXZ)
#   dtor_forwarders_gen_w9c.cpp.obj : error LNK2001: unresolved external symbol
#   "public: virtual int __thiscall Tornado::Behavior(void)" (?Behavior@Tornado@@UAEHXZ)
#
# So the emission adds two rows and closes one, and the row comes out.  Like
# Toad and daTree_c it is one facegen REVERSE FACE per slot from coming back:
# the flat name is defined and the decorated one is wanted, which is the shape
# of every row already in faces_sync.txt.  That file is lane FACES4's this wave.
# ??1Tornado@@UAE@XZ therefore stays on the wall, and it is the only row this
# lane hands back.
#
# STILL STRUCK AFTER THE REVERSE FACES LANDED (wave-9c fold, lane INT4), and
# this is the half the note above could not know. Lane FACES4's commit dadf12c2c
# wrote both Tornado reverse faces, so the two LNK2001 rows are answered now, and
# Tornado still cannot go back in, because that pair was never the whole failure.
# out/DTORS2/build_dtors2_2.log lines 2207 and 2208 carry the other half:
#
#   dtor_forwarders_gen_w9c.cpp.obj : error LNK2005: __ZN7TornadoD0Ev already
#       defined in faces_sync_gen.cpp.obj
#   dtor_forwarders_gen_w9c.cpp.obj : error LNK2005: __ZN7TornadoD1Ev already
#       defined in Tornado_HostSites.cpp.obj
#
# Both flat names are ALREADY DEFINED in this link, which is why neither is on
# the wall: the D0 by a live ledger D row and the D1 by the host copy. A
# forwarder for either is a duplicate no reverse face can fix, so the rows stay
# out. The seat route does not reach it either: src/_ZN7TornadoD1Ev.cpp is on
# port/slice_dtorfaces.txt and compiled today, and dumpbin over its object shows
# exactly one external, ?Tornado_EmitDestructor@@YAXPAUTornado@@@Z -- MSVC inlined
# the whole class-body destructor and emitted no out-of-line copy to name.
# ??1Tornado@@UAE@XZ is therefore still on the wall, referenced by
# hal_cppd1_Tornado in hal/dtor_faces_cpp.cpp, and it is handed on.
BATCHES[4] = """
_ZN10daWanwan_cD0Ev
_ZN20daObjFl_Fall_Block_cD0Ev
_ZN10daWanwan_cD1Ev
_ZN14daObjWc_Mizu_cD0Ev
_ZN15daObjWc_Obj03_cD0Ev
_ZN15daObjWc_Obj04_cD0Ev
_ZN7daPkn_cD0Ev
_ZN7daPkn_cD1Ev
"""

# FOUR ROWS ADDED TO BATCH 5 at the wave-9c fold (lane INT4), on the
# coordinator's word and on this batch's own out-of-line premise. Nobody claimed
# KnockDownPlank or TowerStep: include/KnockDownPlank.h:46 and
# include/TowerStep.h:37 declare the destructor OUT OF LINE, and the
# per-function structor TUs that define it sat on no slice row at all, which is
# the whole of why the premise could not reach them. port/slice_int4.txt puts
# ONE TU of each pair on a slice row. One and not both: the D0 file of each pair
# is the same four lines of C++ as its D1 file, because MSVC folds the complete
# and deleting destructors into one symbol, so compiling both halves of one class
# is an LNK2005 -- out/SEATS2/seat_table.md's stated reason nobody seated these.

BATCHES[5] = """
_ZN11dCapEnemy_cD0Ev
_ZN11dCapEnemy_cD1Ev
_ZN15TextureSequenceD0Ev
_ZN15TextureSequenceD1Ev
_ZN15dScMgSnowball_cD0Ev
_ZN18TextureTransformerD0Ev
_ZN18TextureTransformerD1Ev
_ZN9ModelAnimD0Ev
_ZN9ModelAnimD1Ev
_ZN9ModelAnimD2Ev
_ZN14KnockDownPlankD0Ev
_ZN14KnockDownPlankD1Ev
_ZN9TowerStepD0Ev
_ZN9TowerStepD1Ev
"""

# Which premise each batch stands on.  "inline" is DTORS-A's: the header spells
# the destructor inline and the forwarder's odr-use makes MSVC emit it.
# "outofline" is DTORS2's: the header declares it, a compiled per-function
# structor TU defines it, and the forwarder only calls it.
PREMISE = {1: "inline", 2: "inline", 3: "inline", 4: "inline", 5: "outofline"}

# Which generated file a batch's rows go into.  DTORS-A's two files are left
# exactly as they are, so a rebase carries this lane as an additive file.
GROUP = {1: None, 2: None, 3: None, 4: "w9c", 5: "w9c"}

# Batches that are ACTUALLY EMITTED.  A batch is added here only after its own
# build gate; a class the link refuses is struck from its batch above with the
# reason, never left in and skipped here.
LIVE = [1, 2, 3, 4, 5]


# ---------------------------------------------------------------------------
# THE WRAPPER STRUCTORS (run link100 wave 9c, lane DTORS2).
#
# A DIFFERENT ROW SHAPE, and the wall asks for it in the MSVC spelling rather
# than the ROM's: `??0BattanModelFilePtr@@QAE@I@Z` and
# `??1BattanModelFilePtr@@QAE@XZ`, not a flat _ZN...Ev name.
#
# WHAT THESE CLASSES ARE.  Four src TUs declare a small SharedFilePtr wrapper
# INSIDE THE .cpp, with a constructor and a destructor DECLARED and neither
# DEFINED, and then define a file-scope object of that type:
#
#     struct BattanModelFilePtr : SharedFilePtr { u32 words[2];
#         BattanModelFilePtr(u32 fileID); ~BattanModelFilePtr(); };
#     BattanModelFilePtr data_ov023_02112088(1558);
#
# mwccarm resolved those two undefined imports to ROM addresses, and
# config/tu_manifest.d records which, with the evidence, per class:
#
#   ov023/daObjFm_Battan_c.json      _ZN18BattanModelFilePtrC1Ej -> func_02017acc
#                                    _ZN18BattanModelFilePtrD1Ev -> func_02017ab4
#                                    _ZN22BattanCollisionFilePtrC1Ej -> func_02017b4c
#                                    _ZN22BattanCollisionFilePtrD1Ev ->
#                                        SharedFilePtr_Destruct_Clsn (0x02017b34)
#   ov044/daObjKb1Billboard_c.json   C1 -> func_02017acc, D1 -> func_02017ab4
#   ov100/daStarGate_c.json          C1 -> func_02017acc, D1 -> func_02017ab4
#
# So the port is not missing a body, it is missing the MEMBER DEFINITION that
# carries MSVC's own name onto it.  This file writes that definition, declaring
# the class EXACTLY as the owning TU declares it -- an MSVC mangle depends on
# the name, the class, the calling convention and the signature and on nothing
# else -- and forwarding to the canonical destination.  The calling convention
# conversion (__thiscall member to __cdecl C body) is the compiler's, not a
# hand-written thunk, so the __fastcall dead-edx face trap does not arise.
#
# NO ROM-SIDE VTABLE QUESTION ARISES.  include/SharedFilePtr.h declares no data
# members and no virtuals, and none of the four wrappers declares one either, so
# there is no vftable to store and the PORT_HOST_ABI ruling in
# tools/dtor_store_guard.py has nothing to say about these rows.  What it does
# say about them is nothing: they carry no flat _ZN...D[012]Ev name, so the
# guard reads none out of this file, which is correct rather than a gap.
#
# EVERY CANONICAL DESTINATION IS ALREADY IN THIS LINK, checked before a row was
# written: func_02017acc, func_02017ab4 and func_02017b4c are host veneers in
# hal/cxx_aliases.cpp (lines 384, 437, 439, each with its own PORT_HOST_ABI note
# about the r1 fileID ride-through), and SharedFilePtr_Destruct_Clsn is
# src/SharedFilePtr_Destruct_Clsn.c, on a live slice row.  So no row here needs
# a seat and none can add one.
STRUCTORS = [
    {
        "cls": "BattanModelFilePtr",
        "base": "SharedFilePtr",
        "body": "u32 words[2];",
        "src": "src/game/actors/d_a_obj_fm_battan.cpp:90",
        "manifest": "config/tu_manifest.d/ov023/daObjFm_Battan_c.json",
        "ctor": ("func_02017acc", "0x02017acc"),
        "dtor": ("func_02017ab4", "0x02017ab4"),
    },
    {
        "cls": "BattanCollisionFilePtr",
        "base": "SharedFilePtr",
        "body": "u32 words[2];",
        "src": "src/game/actors/d_a_obj_fm_battan.cpp:97",
        "manifest": "config/tu_manifest.d/ov023/daObjFm_Battan_c.json",
        "ctor": ("func_02017b4c", "0x02017b4c"),
        "dtor": ("SharedFilePtr_Destruct_Clsn", "0x02017b34"),
    },
    {
        "cls": "Kb1BillboardModelFilePtr",
        "base": "SharedFilePtr",
        "body": "u32 words[2];",
        "src": "src/game/actors/d_a_obj_kb1_billboard.cpp:46",
        "manifest": "config/tu_manifest.d/ov044/daObjKb1Billboard_c.json",
        "ctor": ("func_02017acc", "0x02017acc"),
        "dtor": ("func_02017ab4", "0x02017ab4"),
    },
    {
        "cls": "StarGateModelFilePtr",
        "base": "SharedFilePtr",
        "body": "u32 words[2];",
        "src": "src/game/actors/d_a_star_gate.cpp:72",
        "manifest": "config/tu_manifest.d/ov100/daStarGate_c.json",
        "ctor": ("func_02017acc", "0x02017acc"),
        "dtor": ("func_02017ab4", "0x02017ab4"),
    },
]

STRUCTOR_SUFFIX = "w9c_structors"


# ---------------------------------------------------------------------------
# ODR-USE ONLY (run link100 wave 9c, lane DTORS2).
#
# A class here is on the wall under MSVC's OWN destructor name,
# ??1<Cls>@@UAE@XZ, because a host __fastcall face calls it
# (hal/dtor_faces_cpp.cpp's hal_cppd1_<Cls>) off a ROM vtable slot.  Its header
# spells the destructor inline, so the answer is the same as every other row in
# this file -- make MSVC emit it -- but its flat ROM names are ALREADY DEFINED
# somewhere else in this link, so a forwarder under the flat name is a duplicate
# symbol rather than a row.  Measured, not guessed: this lane's gate 2 wrote the
# flat pair for daSanbo_c and the link answered
#
#   dtor_forwarders_gen_w9c.cpp.obj : error LNK2005: __ZN9daSanbo_cD0Ev
#       already defined in faces_sync_gen.cpp.obj
#   dtor_forwarders_gen_w9c.cpp.obj : error LNK2005: __ZN9daSanbo_cD1Ev
#       already defined in Pokey_HostSites.cpp.obj
#
# So the row becomes an odr-use and nothing else: one extern "C" function whose
# body makes the qualified call, which is what forces MSVC to emit the inline
# destructor and the class's vftable.  It is deliberately NOT under a ROM name:
# nothing calls it, it raises no count by itself, and the row that leaves the
# wall is the decorated destructor a ROM vtable slot genuinely reaches.
ODRUSE = [
    {
        "cls": "daSanbo_c",
        "hdr": "daSanbo_c.h",
        "wants": "??1daSanbo_c@@UAE@XZ",
        "caller": "hal/dtor_faces_cpp.cpp's hal_cppd1_daSanbo_c",
        "why": ("_ZN9daSanbo_cD0Ev is already defined in faces_sync_gen.cpp "
                "and _ZN9daSanbo_cD1Ev in port/unmatched/Pokey_HostSites.cpp, "
                "so a flat forwarder is an LNK2005 rather than a row"),
    },
]

# The deallocations a D0 body may add, in the spelling the port resolves.  Same
# table as facegen.DEALLOCATORS and for the same reason: Memory::Deallocate
# reaches its C++ definition through the /alternatename in hal/cxx_aliases.cpp
# and operator_delete2 is defined under its flat name in hal/cxxname_bridge.cpp,
# so the flat spellings are the ones that link.
DEALLOCATORS = {
    "_ZN6Memory10DeallocateEPvP4Heap": {"heap": True},
    "_ZN6Memory16operator_delete2EPv": {"heap": False},
}


def rows(which=None):
    out = []
    for b in (which if which is not None else LIVE):
        for ln in BATCHES[b].split():
            out.append((b, ln.strip()))
    return out


# ---------------------------------------------------------------------------
# derivation


def class_of(name):
    m = re.match(r"^_ZN(\d+)(.+)D([012])Ev$", name)
    if not m:
        return None, None
    return m.group(2)[:int(m.group(1))], m.group(3)


def headers(root):
    """{class: [(header file, spells the destructor inline)]} over include/.

    ONE ENTRY PER FILE, not per declaration.  These headers declare each class
    TWICE -- the real `struct daCamTag_c : dActor_c` under `#ifdef __cplusplus`
    and a flat `struct daCamTag_c { u8 pad[..]; }` under `#else` for the C
    translation units, which can express neither the base class nor the
    virtuals -- and counting both reads as an ambiguity that is not there.
    """
    inc = os.path.join(root, "include")
    out = {}
    for dp, _dn, fn in os.walk(inc):
        for f in fn:
            if not f.endswith(".h"):
                continue
            p = os.path.join(dp, f)
            text = open(p, encoding="utf-8", errors="replace").read()
            rel = os.path.relpath(p, inc).replace("\\", "/")
            seen = set()
            for m in re.finditer(r"^\s*(?:struct|class)\s+(\w+)\b\s*[:{]",
                                 text, re.M):
                cls = m.group(1)
                if cls in seen:
                    continue
                seen.add(cls)
                inline = re.search(
                    r"virtual\s+~%s\s*\(\s*\)\s*\{" % re.escape(cls),
                    text) is not None
                out.setdefault(cls, []).append((rel, inline))
    return out


def d0_shape(guard, rom, name):
    """((deallocation, heap word or None), None) for a D0 row, or (None, why)."""
    ident = name
    span, why = rom.facegen._span_of(rom.root, ident)
    if span is None:
        return None, why
    addr, mod, size = span
    lines, why = guard.disasm(rom, mod, addr, size)
    if lines is None:
        return None, why
    cls, _k = class_of(ident)
    info = guard.analyse(lines, cls)
    if info["base_at"] is None:
        return None, "no base restore, so there is no tail to read"
    tail = [(o, c) for o, c, _s in info["calls"] if o > info["base_at"]]
    if len(tail) != 1:
        return None, ("a D0 body must add exactly one call after the base "
                      "restore and this one adds %d (%s)"
                      % (len(tail), ", ".join(c for _o, c in tail) or "none"))
    dealloc = tail[0][1]
    spec = DEALLOCATORS.get(dealloc)
    if spec is None:
        return None, ("%s is not one of the two deallocations a D0 body may "
                      "add (%s)" % (dealloc, ", ".join(sorted(DEALLOCATORS))))
    if not spec["heap"]:
        return (dealloc, None), None
    # the heap pointer is Deallocate's second argument, out of the body's own
    # literal pool; the pooled word is relocated, so the cartridge names it.
    at = [i for i, (_a, mn, _o, p) in enumerate(lines)
          if mn == "bl" and p == dealloc][0]
    for j in range(at - 1, -1, -1):
        _a, mn, ops, pooled = lines[j]
        m = re.match(r"^r1, =(.+)$", ops)
        if mn.startswith("ldr") and m:
            return (dealloc, m.group(1)), None
    return None, ("%s takes a heap pointer and the body has no pooled load "
                  "into r1 before the call" % dealloc)


def slice_files(root):
    """Every port/slice_*.txt in the tree."""
    import glob
    return sorted(glob.glob(os.path.join(root, "port", "slice_*.txt")))


_LIVE_SLICE = {}


def live_slice_rows(root):
    """The set of src paths any port/slice_*.txt lists UNCOMMENTED.

    A commented row is a retirement with a reason written beside it, so it must
    not read as compiled; that is what the leading `#` means in every slice file
    in this tree.
    """
    if root in _LIVE_SLICE:
        return _LIVE_SLICE[root]
    out = set()
    for f in slice_files(root):
        for ln in open(f, encoding="utf-8", errors="replace"):
            ln = ln.strip()
            if ln and not ln.startswith("#"):
                out.add(ln.replace("\\", "/"))
    _LIVE_SLICE[root] = out
    return out


def defining_tu(root, cls, name):
    """(path, None) for the compiled per-function structor TU, or (None, why).

    THE OUT-OF-LINE PREMISE.  A batch-5 class does not spell its destructor
    inline, so the forwarder cannot make MSVC emit one; it can only call one
    that is already in the link.  What puts it there is the class's own
    per-function structor TU, src/_ZN<n><Cls>D1Ev.cpp (or D2), compiled because
    a slice row names it.  Both halves are checked here: the file must exist in
    src/ and a slice file must list it uncommented.
    """
    live = live_slice_rows(root)
    tried = []
    for k in ("1", "2"):
        rel = "src/_ZN%d%sD%sEv.cpp" % (len(cls), cls, k)
        tried.append(rel)
        if not os.path.exists(os.path.join(root, rel.replace("/", os.sep))):
            continue
        if rel in live:
            return rel, None
        return None, ("%s exists but no port/slice_*.txt lists it uncommented, "
                      "so nothing defines ~%s in this link" % (rel, cls))
    return None, ("no per-function structor TU for %s under src/ (tried %s), "
                  "so the out-of-line premise has nothing to stand on"
                  % (cls, ", ".join(tried)))


def derive(root, which=None):
    """(emitted rows, refusals). Every check is stated in the module header."""
    sys.path.insert(0, os.path.join(root, "port", "tools"))
    import dtor_store_guard as guard
    rom = guard.Rom(root)
    hdrs = headers(root)
    out, refused = [], []
    for batch, name in rows(which):
        cls, kind = class_of(name)
        if cls is None:
            refused.append((name, "not a flat Itanium destructor name"))
            continue
        v, why = guard.check_name(rom, name)
        if v != "ok":
            refused.append((name, "dtor_store_guard: %s" % why))
            continue
        hs = hdrs.get(cls, [])
        if len(hs) != 1:
            refused.append((name, "class %s is declared in %d headers under "
                            "include/ (%s), so this file cannot name one"
                            % (cls, len(hs),
                               ", ".join(h for h, _i in hs) or "none")))
            continue
        hdr, inline = hs[0]
        premise = PREMISE.get(batch, "inline")
        tu = None
        if premise == "inline":
            if not inline:
                refused.append((name, "include/%s does not spell ~%s inline, "
                                "so this row is not an inline-in-header row "
                                "and needs a different answer" % (hdr, cls)))
                continue
        else:
            if inline:
                refused.append((name, "include/%s DOES spell ~%s inline, so "
                                "this row belongs to the inline premise and "
                                "not to the out-of-line one" % (hdr, cls)))
                continue
            tu, why = defining_tu(root, cls, name)
            if tu is None:
                refused.append((name, why))
                continue
        span, _why = rom.facegen._span_of(rom.root, name)
        addr = span[0]
        row = {"batch": batch, "name": name, "cls": cls, "kind": kind,
               "hdr": hdr, "addr": addr, "dealloc": None, "heap": None,
               "premise": premise, "tu": tu}
        if kind == "0":
            got, why = d0_shape(guard, rom, name)
            if got is None:
                refused.append((name, why))
                continue
            row["dealloc"], row["heap"] = got
            d1 = name[:-4] + "D1Ev"
            ents = rom.facegen.rom_index(root).get(d1)
            if ents and row["heap"]:
                hidx = rom.facegen.rom_index(root).get(row["heap"]) or []
                didx = rom.facegen.rom_index(root).get(row["dealloc"]) or []
                if len(set(a for a, _m in hidx)) == 1 \
                        and len(set(a for a, _m in didx)) == 1:
                    _info, pwhy = rom.facegen.d0_reloc_proof(
                        root, name, d1, didx[0][0], hidx[0][0])
                    row["d0proof"] = pwhy or "relocation proof: D0 is D1 plus " \
                                             "%s" % row["dealloc"]
        out.append(row)
    return out, refused


# ---------------------------------------------------------------------------
# emission

BANNER = '''\
// GENERATED by port/tools/dtorfwd.py -- do not edit by hand.  Read that file's
// header for what is derived and how, and port/tools/dtor_store_guard.py for
// the PORT_HOST_ABI ruling this file exists under.  Run link100 wave 9b, lane
// DTORS-A.
//
// THE ROWS.  Each is a flat Itanium destructor name walk_window's link asked for
// and nothing defined.  The owning src TU IS compiled into the binary; main's
// header declares the destructor INLINE (`virtual ~daCamTag_c() {}`), mwccarm
// emits the body out of line because the class's vtable odr-uses it, and MSVC
// emits nothing because the port's vtables are hand-written arrays in
// hal/actor_classes_*.cpp and nothing here odr-uses the destructor.  Each line
// below is a forwarder under the ROM's own flat name whose QUALIFIED call
// odr-uses the inline body, so MSVC emits it.
//
// QUALIFIED ON PURPOSE.  `self->~Cls()` on a class with a virtual destructor is
// a VIRTUAL call; it would dispatch through the port's ROM-shaped table at slot
// 16 and re-enter this same symbol.  `((Cls *)self)->Cls::~Cls()` is a direct
// call.  hal/dtor_faces_cpp.cpp's adapters carry the same spelling and the same
// reason.
//
// WHAT CHANGES AT RUN TIME, honestly.  MSVC's destructor stores ITS OWN vftable
// (??_7Cls@@6B@) into word 0 of the object before running the body, where the
// cartridge's D1 stores _ZTV<Cls>; MSVC folds the Itanium D1/D0 pair into one
// scalar-deleting slot, so that table is NOT ROM-shaped.  That store is inert
// because nothing reads the object's vptr between it and the base destructor
// call, and the base destructor -- the ROM's own C body in this port -- stores a
// ROM-shaped table again as its own first act.  Measured over every row from
// extracted/, per class, and re-measured by tools/dtor_store_guard.py in the
// pre-configure guard wave on every build.  A class whose cartridge body stops
// satisfying that refuses the build.
//
// THE D0 HALF adds the one deallocation the cartridge's own D0 body makes, with
// the heap pointer read out of that body's pooled load rather than defaulted.
'''


# ---------------------------------------------------------------------------
# THE SPLIT.  Two of these headers cannot share a translation unit.
#
# Measured, not guessed: the whole 192-row set emitted into one file and
# syntax-checked with the build's own flags (cl /Zs, walk_window's DEFINES,
# FLAGS and INCLUDES straight out of build/port/build.ninja) reports
#
#   include\\Stage.h(26): error C2011: 'Particle::SysTracker': 'struct' type
#                         redefinition
#   include\\dScMgSingle3DBase_c.h(18): note: see declaration of
#                         'Particle::SysTracker'
#
# Both headers DEFINE the struct, each behind its own include guard, so no
# ordering fixes it and the TU that includes both cannot compile.  That is a
# tree defect (out/DTORS-A/bugs.md), and this lane does not fix decomp headers,
# so the emission splits instead: the rows whose header closure carries the
# SECOND header of the pair get their own file.
#
# Over the full 192 rows that is the ONLY conflict.  Statically, three type
# names are defined in more than one header in the 168-header closure --
# SysTracker, plus `State` in four actor headers and `Vector3_16` in common.h
# and dBgW.h -- and the compiler accepts the other two, because they are nested
# in different enclosing classes.  So the split is keyed on the MEASURED pair,
# and --conflicts prints the static scan for a human to re-check when the set of
# rows changes.
SPLITS = [
    {
        "suffix": "stage",
        "header": "Stage.h",
        "against": "dScMgSingle3DBase_c.h",
        "why": ("include/Stage.h:26 and include/dScMgSingle3DBase_c.h:18 each "
                "define struct Particle::SysTracker, so one TU cannot include "
                "both (C2011)"),
    },
]


def _closure(root, hdr, memo):
    """The transitive `#include \"...\"` closure of one header under include/."""
    if hdr in memo:
        return memo[hdr]
    inc = os.path.join(root, "include")
    seen, stack = set(), [hdr]
    while stack:
        h = stack.pop()
        p = os.path.join(inc, h)
        if h in seen or not os.path.exists(p):
            continue
        seen.add(h)
        text = open(p, encoding="utf-8", errors="replace").read()
        stack += re.findall(r'^\s*#\s*include\s+"([^"]+)"', text, re.M)
    memo[hdr] = seen
    return seen


def _suffix(group, split):
    """The file suffix for one (GROUP, SPLITS) pair. None is DTORS-A's file."""
    parts = [x for x in (group, split) if x]
    return "_".join(parts) or None


def split_rows(root, rowlist):
    """([(path suffix, rows)], refusals). The main file first.

    TWO axes, and they are independent.  GROUP keeps lane DTORS2's wave-9c rows
    in their own file so DTORS-A's two files stay byte-identical through a
    rebase; SPLITS is the measured header conflict, and it is applied inside
    each group because the conflicting header is reachable from either lane's
    rows.
    """
    memo = {}
    groups = {}
    refused = []
    for r in rowlist:
        c = _closure(root, r["hdr"], memo)
        split, bad = None, False
        for sp in SPLITS:
            if sp["header"] in c and sp["against"] in c:
                refused.append((r["name"], "include/%s pulls in BOTH %s and "
                                "%s, which cannot share a translation unit: %s"
                                % (r["hdr"], sp["header"], sp["against"],
                                   sp["why"])))
                bad = True
                break
            if sp["header"] in c:
                split = sp["suffix"]
                break
        if bad:
            continue
        groups.setdefault(_suffix(GROUP.get(r["batch"]), split), []).append(r)
    order = [None] + [_suffix(None, sp["suffix"]) for sp in SPLITS]
    for g in sorted(x for x in set(GROUP.values()) if x):
        order.append(_suffix(g, None))
        order += [_suffix(g, sp["suffix"]) for sp in SPLITS]
    out = []
    for sfx in order:
        if sfx is None:
            out.append((None, groups.get(None, [])))
        elif groups.get(sfx):
            out.append((sfx, groups[sfx]))
    return out, refused


STRUCTOR_BANNER = """// GENERATED by port/tools/dtorfwd.py -- do not edit by hand.  Run link100 wave
// 9c, lane DTORS2.
//
// THE ROWS.  Each pair below is a small SharedFilePtr wrapper class that a src
// translation unit DECLARES inside itself, with a constructor and a destructor
// declared and neither defined, and then instantiates at file scope:
//
//     struct BattanModelFilePtr : SharedFilePtr { u32 words[2];
//         BattanModelFilePtr(u32 fileID); ~BattanModelFilePtr(); };
//     BattanModelFilePtr data_ov023_02112088(1558);
//
// So walk_window asks for ??0BattanModelFilePtr@@QAE@I@Z and
// ??1BattanModelFilePtr@@QAE@XZ and nothing defines them.  What the cartridge
// did with those same two undefined imports is recorded per class in
// config/tu_manifest.d, with the relocation that proves it, and this file
// writes the member definition that carries MSVC's name onto that destination.
//
// THE CLASS IS DECLARED EXACTLY AS THE OWNING TU DECLARES IT, which is what
// makes the mangles meet: an MSVC mangle depends on the name, the class, the
// calling convention and the signature and on nothing else.  The source line
// each declaration was copied from is quoted above it.
//
// THE CALLING CONVENTION IS THE COMPILER'S.  These are real member definitions,
// so MSVC emits __thiscall and converts to the __cdecl C body itself.  Nothing
// here is a hand-written thunk and no dead-edx __fastcall face is involved.
//
// NO VTABLE QUESTION ARISES.  include/SharedFilePtr.h declares no data members
// and no virtuals and neither does any of these four wrappers, so nothing
// stores a vftable and tools/dtor_store_guard.py's PORT_HOST_ABI ruling has
// nothing to say about these rows.  It reads no name out of this file because
// there is no flat _ZN...D[012]Ev name in it, which is right rather than a gap.
"""


def emit_structors(root, path):
    lines = [STRUCTOR_BANNER, "", '#include "types.h"',
             '#include "SharedFilePtr.h"', ""]
    fns = {}
    for r in STRUCTORS:
        fns[r["ctor"][0]] = ("ctor", r["ctor"][1])
        fns[r["dtor"][0]] = ("dtor", r["dtor"][1])
    lines.append("/* The canonical destinations, every one already defined in")
    lines.append("   this link: the three veneers are hal/cxx_aliases.cpp and")
    lines.append("   SharedFilePtr_Destruct_Clsn is its own src TU. */")
    lines.append('extern "C" {')
    for n in sorted(fns):
        kind, addr = fns[n]
        if kind == "ctor":
            lines.append("int %s(void *self, unsigned int fileID);  /* ROM %s */"
                         % (n, addr))
        else:
            lines.append("int %s(int self);                          /* ROM %s */"
                         % (n, addr))
    lines.append("}")
    lines.append("")
    for r in STRUCTORS:
        lines.append("/* %s, declared as %s declares it." % (r["cls"], r["src"]))
        lines.append("   %s names both destinations. */" % r["manifest"])
        lines.append("struct %s : %s {" % (r["cls"], r["base"]))
        lines.append("    %s" % r["body"])
        lines.append("")
        lines.append("    %s(u32 fileID);" % r["cls"])
        lines.append("    ~%s();" % r["cls"])
        lines.append("};")
        lines.append("")
        lines.append("%s::%s(u32 fileID) { %s(this, fileID); }"
                     % (r["cls"], r["cls"], r["ctor"][0]))
        lines.append("%s::~%s() { %s((int)this); }"
                     % (r["cls"], r["cls"], r["dtor"][0]))
        lines.append("")
    text = "\n".join(lines).rstrip() + "\n"
    old = open(path, encoding="utf-8").read() if os.path.exists(path) else None
    if old != text:
        open(path, "w", encoding="utf-8", newline="\n").write(text)
    return text, old


def genpath(root, suffix):
    name = "dtor_forwarders_gen.cpp" if suffix is None \
        else "dtor_forwarders_gen_%s.cpp" % suffix
    return os.path.join(root, "port", "hal", name)


def emit(root, rowlist, path, suffix=None):
    lines = [BANNER]
    if suffix is not None and suffix.startswith("w9c"):
        lines.append("//")
        lines.append("// THIS FILE IS RUN link100 WAVE 9c, LANE DTORS2's half")
        lines.append("// of the same mechanism.  Its batch-4 rows stand on the")
        lines.append("// SAME premise as the file above (the header spells the")
        lines.append("// destructor inline); their owning TUs were simply not")
        lines.append("// compiled when DTORS-A ran, and lanes SEATS2 and")
        lines.append("// HOSTGEN4 have seated them since.  Its batch-5 rows")
        lines.append("// stand on the OUT-OF-LINE premise: the header only")
        lines.append("// declares the destructor and the class's own")
        lines.append("// per-function structor TU defines it, so the forwarder")
        lines.append("// CALLS a body that is already in the link instead of")
        lines.append("// making MSVC emit one.  Each row below says which.")
        suffix = suffix[3:].lstrip("_") or None
        odruse = ODRUSE if suffix is None else []
    else:
        odruse = []
    if suffix is not None:
        s = [x for x in SPLITS if x["suffix"] == suffix][0]
        lines.append("//")
        lines.append("// THIS FILE IS A SPLIT of the one above it, and the")
        lines.append("// reason is a tree defect rather than a choice: %s."
                     % s["why"])
        lines.append("// Every row here is a class whose header closure pulls")
        lines.append("// in include/%s. See tools/dtorfwd.py's SPLITS block."
                     % s["header"])
    hdrs = sorted(set([r["hdr"] for r in rowlist]
                      + [o["hdr"] for o in odruse]))
    lines.append("")
    lines.append('#include "types.h"')
    for h in hdrs:
        lines.append('#include "%s"' % h)
    lines.append("")
    deallocs = sorted(set(r["dealloc"] for r in rowlist if r["dealloc"]))
    heaps = sorted(set(r["heap"] for r in rowlist if r["heap"]))
    if deallocs or heaps:
        lines.append("// The deallocation each D0 body makes, and the heap")
        lines.append("// pointer word it reads, both under the ROM's own flat")
        lines.append("// names: those are what the cartridge's relocations")
        lines.append("// name and what this port already resolves.")
        lines.append('extern "C" {')
        for d in deallocs:
            if DEALLOCATORS[d]["heap"]:
                lines.append("void %s(void *ptr, void *heap);" % d)
            else:
                lines.append("void %s(void *ptr);" % d)
        for h in heaps:
            lines.append("extern void *%s;" % h)
        lines.append("}")
        lines.append("")
    for r in sorted(rowlist, key=lambda x: (x["batch"], x["name"])):
        call = "((%s *)self)->%s::~%s();" % (r["cls"], r["cls"], r["cls"])
        how = "the inline ~%s()" % r["cls"]             if r.get("premise", "inline") == "inline"             else "~%s(), defined out of line by %s" % (r["cls"], r.get("tu"))
        if r["kind"] == "0":
            if r["heap"]:
                free = " %s(self, %s);" % (r["dealloc"], r["heap"])
            else:
                free = " %s(self);" % r["dealloc"]
            lines.append("/* ROM 0x%08x %s -- batch %d, %s plus %s */"
                         % (r["addr"], r["name"], r["batch"], how,
                            r["dealloc"]))
        else:
            free = ""
            lines.append("/* ROM 0x%08x %s -- batch %d, %s */"
                         % (r["addr"], r["name"], r["batch"], how))
        lines.append('extern "C" void %s(void *self)' % r["name"])
        lines.append("{ %s%s }" % (call, free))
        lines.append("")
    for o in odruse:
        lines.append("/* ODR-USE ONLY, no ROM name. The wall wants %s,"
                     % o["wants"])
        lines.append("   which %s calls off a ROM vtable slot. %s's"
                     % (o["caller"], o["cls"]))
        lines.append("   header spells the destructor inline, so MSVC"
                     )
        lines.append("   emits it as soon as something odr-uses it, and"
                     )
        lines.append("   this call is that and nothing else. It is NOT"
                     )
        lines.append("   under the flat ROM name, because %s. */" % o["why"])
        lines.append('extern "C" void *dtorfwd_odruse_%s(void *self)'
                     % o["cls"])
        lines.append("{ ((%s *)self)->%s::~%s(); return self; }"
                     % (o["cls"], o["cls"], o["cls"]))
        lines.append("")
    text = "\n".join(lines).rstrip() + "\n"
    old = open(path, encoding="utf-8").read() if os.path.exists(path) else None
    if old != text:
        open(path, "w", encoding="utf-8", newline="\n").write(text)
    return text, old


# ---------------------------------------------------------------------------


def selftest():
    fails = []
    if class_of("_ZN10daCamTag_cD1Ev") != ("daCamTag_c", "1"):
        fails.append("class_of D1")
    if class_of("_ZN21daObj_volcanoCannon_cD0Ev") != \
            ("daObj_volcanoCannon_c", "0"):
        fails.append("class_of long name with an underscore")
    if class_of("?InitResources@daCamTag_c@@UAEHXZ") != (None, None):
        fails.append("class_of refuses a decorated name")
    names = [n for _b, n in rows()]
    if len(names) != len(set(names)):
        dup = sorted(n for n in set(names) if names.count(n) > 1)
        fails.append("duplicate row(s): %s" % ", ".join(dup))
    bad = [n for n in names if class_of(n)[0] is None]
    if bad:
        fails.append("unparsable row(s): %s" % ", ".join(bad[:5]))
    if not LIVE or [b for b in LIVE if b not in BATCHES]:
        fails.append("LIVE names a batch BATCHES does not carry: %r vs %r"
                     % (sorted(LIVE), sorted(BATCHES)))
    # the emitted shape, on a fixture row rather than the tree
    row = [{"batch": 1, "name": "_ZN10daCamTag_cD0Ev", "cls": "daCamTag_c",
            "kind": "0", "hdr": "daCamTag_c.h", "addr": 0x020b076c,
            "dealloc": "_ZN6Memory10DeallocateEPvP4Heap",
            "heap": "GAME_HEAP_PTR"}]
    import tempfile
    fd, p = tempfile.mkstemp(suffix=".cpp")
    os.close(fd)
    text, _old = emit(".", row, p)
    os.unlink(p)
    want = ('extern "C" void _ZN10daCamTag_cD0Ev(void *self)\n'
            '{ ((daCamTag_c *)self)->daCamTag_c::~daCamTag_c();'
            ' _ZN6Memory10DeallocateEPvP4Heap(self, GAME_HEAP_PTR); }')
    if want not in text:
        fails.append("emitted D0 body is not the expected shape")
    if '#include "daCamTag_c.h"' not in text:
        fails.append("the real header is not included")
    bodies = [ln for ln in text.splitlines() if ln.startswith("{ ")]
    if any("self->~" in ln for ln in bodies):
        fails.append("an UNQUALIFIED destructor call was emitted, which is a "
                     "virtual dispatch back into this symbol")
    if not bodies:
        fails.append("no forwarder body was emitted at all")
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    import dtor_store_guard as guard
    got = guard.names_in.__doc__ and guard.DEF_RE.findall(text)
    if sorted(got) != ["_ZN10daCamTag_cD0Ev"]:
        fails.append("the guard cannot read this file's definitions back: %r"
                     % (got,))
    if fails:
        for f in fails:
            print("dtorfwd SELFTEST FAIL: %s" % f)
        return 1
    print("dtorfwd selftest OK -- %d rows over %d batch(es), %d live"
          % (len(names), len(BATCHES), len(LIVE)))
    return 0


def conflict_scan(root, rowlist):
    """Type names DEFINED in more than one header of the rows' closure.

    Advisory, not a gate: the name is taken bare, so a nested type in two
    different enclosing classes reads as a clash the compiler accepts. The
    measured pairs are in SPLITS; this is the net for noticing a NEW one when
    the row set changes, before a build slot is spent on it.
    """
    memo, owner = {}, {}
    union = set()
    for r in rowlist:
        union |= _closure(root, r["hdr"], memo)
    inc = os.path.join(root, "include")
    for h in sorted(union):
        p = os.path.join(inc, h)
        if not os.path.exists(p):
            continue
        text = open(p, encoding="utf-8", errors="replace").read()
        for m in re.finditer(r"^[ \t]*(?:struct|class)\s+(\w+)\b[^;\n]*\{",
                             text, re.M):
            owner.setdefault(m.group(1), set()).add(h)
    return {t: sorted(v) for t, v in owner.items() if len(v) > 1}, len(union)


# The slice lists that carry the GENERATED FILES themselves (not src rows).
# DTORS-A's is left alone; wave 9c's file is listed in lane DTORS2's own list
# with its own CMake block, so a rebase carries each lane's step separately.
SLICES = ["port/slice_dtors_a.txt", "port/slice_dtors2.txt"]
SLICE = SLICES[0]


def slice_rows(root):
    out = []
    for rel in SLICES:
        p = os.path.join(root, rel)
        if not os.path.exists(p):
            continue
        out += [ln.strip() for ln in open(p, encoding="utf-8")
                if ln.strip() and not ln.strip().startswith("#")]
    return out


def main():
    if "--selftest" in sys.argv:
        return selftest()
    root = os.path.abspath(sys.argv[1])
    rowlist, refused = derive(root)
    groups, split_refused = split_rows(root, rowlist)
    refused += split_refused
    for n, why in refused:
        print("REFUSED %s: %s" % (n, why))
    if "--conflicts" in sys.argv:
        dup, n = conflict_scan(root, rowlist)
        print("%d header(s) in the transitive closure, %d type name(s) defined "
              "in more than one of them" % (n, len(dup)))
        for t in sorted(dup):
            print("  %-20s %s" % (t, ", ".join(dup[t])))
        return 0
    want_files = [os.path.relpath(genpath(root, s), root).replace("\\", "/")
                  for s, _rs in groups]
    structor_path = genpath(root, STRUCTOR_SUFFIX)
    if STRUCTORS:
        want_files.append(os.path.relpath(structor_path, root)
                          .replace("\\", "/"))
    bad = 0
    if "--verify" in sys.argv:
        import tempfile
        scratch = tempfile.mkdtemp(prefix="dtorfwd-verify-")
        for suffix, rs in groups:
            path = genpath(root, suffix)
            # into a temp directory, never beside the file: --verify runs in
            # the pre-configure guard wave and must not write into the tree it
            # is checking.
            probe = os.path.join(scratch, os.path.basename(path))
            text, _old = emit(root, rs, probe, suffix)
            cur = open(path, encoding="utf-8").read() \
                if os.path.exists(path) else None
            os.unlink(probe)
            if cur != text:
                print("dtorfwd --verify: %s is NOT what this tool generates "
                      "from the tree. Re-run with --emit and read the diff "
                      "before committing."
                      % os.path.relpath(path, root).replace("\\", "/"))
                bad += 1
        if STRUCTORS:
            probe = os.path.join(scratch,
                                 os.path.basename(structor_path))
            text, _old = emit_structors(root, probe)
            cur = open(structor_path, encoding="utf-8").read() \
                if os.path.exists(structor_path) else None
            os.unlink(probe)
            if cur != text:
                print("dtorfwd --verify: %s is NOT what this tool "
                      "generates from the tree. Re-run with --emit "
                      "and read the diff before committing."
                      % os.path.relpath(structor_path, root)
                      .replace("\\", "/"))
                bad += 1
        os.rmdir(scratch)
        have = slice_rows(root)
        missing = [f for f in want_files if f not in have]
        extra = [f for f in have if f.startswith("port/hal/dtor_forwarders_")
                 and f not in want_files]
        if missing or extra:
            print("dtorfwd --verify: %s do not list exactly the generated "
                  "files. missing %s; stale %s"
                  % (" + ".join(SLICES), missing or "none", extra or "none"))
            bad += 1
        if bad:
            return 1
        print("dtorfwd --verify OK -- %d forwarder(s) over %d file(s), "
              "%d refused" % (len(rowlist), len(groups), len(refused)))
        return 0
    for suffix, rs in groups:
        emit(root, rs, genpath(root, suffix), suffix)
    if STRUCTORS:
        emit_structors(root, structor_path)
    print("dtorfwd --emit OK -- %d forwarder(s) over %d class(es) in %d "
          "file(s), %d refused"
          % (sum(len(rs) for _s, rs in groups),
             len(set(r["cls"] for _s, rs in groups for r in rs)),
             len(groups), len(refused)))
    for suffix, rs in groups:
        print("    %-44s %d row(s)"
              % (os.path.relpath(genpath(root, suffix), root)
                 .replace("\\", "/"), len(rs)))
    if STRUCTORS:
        print("    %-44s %d wrapper structor pair(s)"
              % (os.path.relpath(structor_path, root).replace("\\", "/"),
                 len(STRUCTORS)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
