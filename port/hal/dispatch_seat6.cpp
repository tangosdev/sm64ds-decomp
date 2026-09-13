// ===========================================================================
// Run link100 wave 7, lane SEAT6, batch B6 (SLOT-DISPATCH): the one MEMBER
// DECORATION the batch needs and nothing in this link already answers.
//
// Seventeen of this batch's eighteen rows are respellings of a CALL -- they
// live in port/tools/hostgen.py's VIRTUAL_CALL / REG_RIDE_ARG / ARG_WIDTH /
// HEADER_SHADOW tables, where the generated host TU is the thing compiled and
// src/ is untouched. Two of them (UnchainedChomp::InitResources, ov100, and
// MantaRay::InitResources, ov090) need no respelling at all: they are already
// correct C++ and simply name a member MSVC decorates in a spelling nothing
// defines.
//
// WHAT THEY NAME. Both TUs declare the same local shadow
//
//     struct PathPtr {
//         char pad[8];
//         PathPtr();
//         void FromID(unsigned int id);
//         int  NumNodes() const;
//         void GetNode(Vector3& out, unsigned int idx) const;
//     };
//
// and construct one on the stack. Under MSVC those are __thiscall members and
// decorate to
//
//     ??0PathPtr@@QAE@XZ                      the default constructor
//     ?FromID@PathPtr@@QAEXI@Z                FromID
//     ?NumNodes@PathPtr@@QBEHXZ               NumNodes, returning int
//     ?GetNode@PathPtr@@QBEXAAUVector3@@I@Z   GetNode, const, void return
//
// while the matched bodies in this build carry the ROM's flat Itanium C names
// _ZN7PathPtrC1Ev / _ZN7PathPtr6FromIDEj / _ZNK7PathPtr8NumNodesEv /
// _ZNK7PathPtr7GetNodeER7Vector3j. When port/unmatched/MantaRay_HostSites.cpp
// was written "the link named all four decorations as unresolved". THREE OF
// THE FOUR HAVE SINCE BEEN ANSWERED by lanes that met the same wall from
// another class:
//
//     ??0PathPtr@@QAE@XZ                     port/unmatched/Klepto_PathPtrFaces.cpp
//     ?FromID@PathPtr@@QAEXI@Z               port/unmatched/Klepto_PathPtrFaces.cpp
//     ?GetNode@PathPtr@@QBEXAAUVector3@@I@Z  port/unmatched/RacingPenguin_ShadowFaces.cpp
//
// -- all three are publics of the current walk_window.map, measured, not
// assumed. THE FOURTH IS NOT, and the reason is a RETURN TYPE. include/PathPtr.h
// declares NumNodes returning `unsigned int`, so the face
// port/hal/method_faces.cpp:231 is built on top of decorates
// ?NumNodes@PathPtr@@QBEIXZ -- an `I` where both of these TUs' own shadow says
// `H`. MantaRay_HostSites.cpp's banner says exactly that: "include/PathPtr.h
// declares NumNodes returning `unsigned int` where this TU declares `int`, so
// even a face built on that header would decorate differently". One missing
// decoration is what kept two whole matched InitResources bodies out of the
// link.
//
// SO THIS FILE DEFINES THAT ONE MEMBER, and nothing else. It is the
// Klepto_PathPtrFaces.cpp recipe verbatim: a REAL __thiscall forwarder, never
// a `/alternatename` alias onto the flat body -- an alias points the member
// name at a cdecl body that reads its first argument off the stack while the
// member call left the receiver in ECX, which is the 2026-08-07 door-open
// crash and the level-16 Klepto c0000005. Nothing here changes what code runs;
// it only puts the receiver where __thiscall left it.
//
// include/PathPtr.h is deliberately NOT included: it declares the real class
// and having both in one TU is a redefinition (the ToxBox lesson). The shadow
// is spelled with the one member that is wanted, because the decoration of
// ?NumNodes@PathPtr@@QBEHXZ depends on the class name, the method name, the
// const, the calling convention and the signature -- not on the rest of the
// class. The `char pad[8]` keeps the object the size both TUs give it.
// ===========================================================================

// ---------------------------------------------------------------------------
// AND ONE C-NAME FACE, for the same reason lane FACEF needed four of them.
//
// src/_ZN9RabbitKey6RenderEv.cpp recovered as a REAL C++ METHOD against
// include/RabbitKey.h, so the only symbol it puts in the map is MSVC's own
// mangling ?Render@RabbitKey@@QAEHXZ. The seat that fills the ROM's vtable
// word -- hal/actor_classes.cpp:1679, `vt[9] = rk_render` -- calls the ROM's
// Itanium C name with an explicit self, which the retired host copy used to
// define. The face supplies that name and nothing else; the ROM vtable word,
// the seat and the call are all untouched. This is exactly the arrangement
// lane FACEF shipped in hal/except_faces.cpp for Goomboss::Render,
// ShipUp::Render, FloatOnWaterPlatformJrb::Render and Player::BlowAway, and
// the gate-1b LINK is what named it here rather than a reading.
//
// It is a FACE, not a shadow: it forwards with a qualified call, so the map
// carries both spellings and linkage.py's FACES bucket is the honest home for
// it. The receiver is passed explicitly and bridged into ECX by the qualified
// call, which is the method_faces.cpp checklist item (c).
// ---------------------------------------------------------------------------

#include "RabbitKey.h"

extern "C" int _ZN9RabbitKey6RenderEv(void *self)
{
    return ((RabbitKey *)self)->RabbitKey::Render();
}

struct PathPtr {
    char pad[8];
    int NumNodes() const;
};

extern "C" unsigned int _ZNK7PathPtr8NumNodesEv(const void *self);

/* PORT_HOST_ABI: __thiscall receiver in ECX forwarded to the cdecl flat name;
   the `int` return is this TU's shadow spelling of the ROM's own u32 count. */
int PathPtr::NumNodes() const
{
    return (int)_ZNK7PathPtr8NumNodesEv(this);
}
