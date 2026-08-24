// PORT_HOST_ABI. dScMgCard_c's vtable face. Run mg11, lane PKR.
// actor id 0x17b = scene 379, "Picture Poker".
//
// ---- TWO ALIASES, AND BOTH ARE THE ORDINARY KIND -------------------------
//
// src/MgPicturePoker_Spawn.cpp is a //cpp TU and declares the class's vtable
// OUTSIDE its extern "C" block:
//
//     extern int data_ov006_0213bdb4;
//
// so MSVC mangles the reference as ?data_ov006_0213bdb4@@3HA -- a C++-linkage
// int -- while the ov006 mount (port/tools/ovdata.py's emission) defines the
// plain C name _data_ov006_0213bdb4.  That is the "C-named symbols declared at
// C++ linkage" case port/mg_fanout_costs.txt section 4 counts twenty-two of,
// and hal/scene_boot.cpp section 1 already carries twenty-three of for the star
// select.  An alias is the whole fix: both sides are the same object at the
// same address and nothing about the type or the calling convention differs.
//
// AN ALIAS CANNOT CHANGE A CALLING CONVENTION, which is why neither of these
// two is one of those.  Both are DATA.  The one row in this class's closure
// that would have needed a re-landing face is vtable slot 18, and that is
// handled where it belongs -- hal/scene_mg_card.cpp's card_reset takes the
// stack argument the ROM's callers push -- not by a name.
#pragma comment(linker, "/alternatename:?data_ov006_0213bdb4@@3HA=_data_ov006_0213bdb4")

// The second is the SAME SHAPE in a different TU and it is not this class's
// vtable: src/func_ov006_020da9c4.cpp (vtable slot 9, Render) declares
//
//     extern int data_ov006_02134028;
//
// outside its own extern "C" block, while src/func_ov006_020dbaf0.cpp (slot 0)
// declares the same object INSIDE one, as `extern u32 *data_ov006_02134028`.
// Two TUs of one class disagreeing about the linkage of one symbol is what
// leaves a single unresolved C++ name against a defined C one.  The mount owns
// the storage; both readers reach the same 0x02134028.
//
// IT IS HERE BECAUSE THIS SEAT IS THE FIRST TO REACH THAT BODY.  A later lane
// that reaches the same OAM attribute list will find the alias already
// standing; an /alternatename with the same left and right sides declared twice
// is deduplicated by the linker, so a duplicate is harmless.
#pragma comment(linker, "/alternatename:?data_ov006_02134028@@3HA=_data_ov006_02134028")

// BOTH ROWS ARE CHECKED THE OTHER WAY ROUND RATHER THAN TAKEN ON TRUST.
// port/tools/alternatename_guard.py runs POST-LINK over the real map on every
// build and fails if an /alternatename left-hand side is also a DEFINED symbol
// -- the failure mode that would make either line silently useless.
//
// facegen was not used to generate these and that is the tool behaving:
// port/mg_fanout_costs.txt section 10's third tool finding is that a failed
// link TRUNCATES walk_window.map to zero bytes, which is the universe facegen
// classifies against, so it refuses after exactly the probe link that names
// these symbols.  Both were found instead by port/tools/closure.py over
// port/slice_pkr.txt against the BASELINE map, before any of this lane's code
// existed, and both were confirmed by the first real link.

/* MSVC emits nothing for a TU that is only pragmas, and an empty object still
   carries the linker directive.  This symbol exists so a reader grepping for
   the file's contribution finds something, and so the object is never mistaken
   for a build accident. */
extern "C" const char port_mg_card_faces_marker[] = "MgCard_Faces";
