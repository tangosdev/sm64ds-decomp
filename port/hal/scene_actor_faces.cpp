// The four ActorBase/ActorDerived lifecycle halves a SCENE vtable dispatches,
// as plain C-named faces hal/scene_boot.cpp can call.
//
// WHY THIS IS A SEPARATE FILE FROM scene_boot.cpp. Two mutually exclusive
// declarations of `struct ActorBase` are needed and one TU cannot hold both.
// The bodies here are real C++ methods against include/ActorBase.h (virtual,
// so ?AfterBehavior@fBase_c@@UAEXI@Z), while ActorBase::Virtual34 and
// Virtual38 are defined by src TUs that declare them NON-virtual in their own
// local struct (?Virtual34@fBase_c@@QAEHII@Z, which is what
// hal/lk4_solidheap_seat.cpp links against and what scene_boot.cpp has to
// reproduce). Same class name, different mangling, one file each.
//
// ---- THE ARGUMENT RIDE-THROUGH, which is the real reason these exist -------
//
// Three of the eight Scene:: halves the scene vtables hold are 0xc-byte
// TAIL-CALL VENEERS in the ROM:
//
//   Scene::AfterInitResources     0x0202e62c -> ActorDerived::  0x02013ef4
//   Scene::AfterBehavior          0x0202e3c8 -> ActorBase::     0x02043af8
//   Scene::AfterRender            0x0202e398 -> ActorBase::     0x02043ac4
//
// each `ldr ip, [pc]; bx ip; .word <target>`. On ARM the arguments ride
// through in r0/r1 untouched, so the decomp writes all three as `void f(void)`
// calling `void g(void)` -- which is a faithful transcription of three
// instructions and is exactly right for the ROM.
//
// It cannot work on the host. MSVC's __cdecl passes arguments on the stack, so
// a (void) caller pushes nothing and the (void) callee reads whatever was
// there: the scene object and the vfSuccess code both arrive as garbage, and
// AfterInitResources' garbage decides whether the actor is marked for
// destruction on its first frame. This is the same class of defect the port's
// other ride-through host copies exist for (port/unmatched/
// SetNumPlayers_hostcopy.c, func_02073244_hostcopy.c, func_0203cbc0).
//
// So the scene fill dispatches STRAIGHT TO THE VENEER'S TARGET with both
// arguments, which is what the three ROM instructions mean. The three
// src/_ZN5Scene*.cpp veneer TUs are therefore NOT in port/slice_scene1.txt:
// there is no host form of them that carries the arguments, and linking a body
// nothing can call would be a count, not a port. Scene::AfterCleanupResources
// (slot 5) is NOT a veneer -- it has a real body with declared arguments -- and
// it is dispatched directly, from scene_boot.cpp.
#include "dBase_c.h"

extern "C" {

/* slot 2. ActorDerived's is the ONE functional override in that class: mark
   the actor for destruction when init reported VS_FAIL, then chain to
   ActorBase's. The body is the hostgen copy of src/
   _ZN7dBase_c18AfterInitResourcesEj.c (GATE13_SYMS in CMakeLists). */
void port_scene_after_init(void *self, unsigned vfSuccess)
{ ((dBase_c *)self)->dBase_c::AfterInitResources(vfSuccess); }

/* slots 8 and 11 */
void port_scene_after_behavior(void *self, unsigned vfSuccess)
{ ((fBase_c *)self)->fBase_c::AfterBehavior(vfSuccess); }
void port_scene_after_render(void *self, unsigned vfSuccess)
{ ((fBase_c *)self)->fBase_c::AfterRender(vfSuccess); }

/* slot 15. Not a veneer -- ActorBase::OnHeapCreated is an 8-byte `return 1`
   and the three scene classes do not override it -- but it is reached through
   the same header, so it lives here with the other three rather than forcing
   scene_boot.cpp to include ActorBase.h. */
int port_scene_on_heap_created(void *self)
{ return ((fBase_c *)self)->fBase_c::OnHeapCreated(); }

/* slots 0 and 3, for the MINIGAME classes only, and they are here for the same
   include reason the other four are rather than because anything about them is
   special. Neither is a veneer.
   ov003's and ov007's scene classes override both slots, so no fill before this
   one ever needed the base bodies; dScMgBase_c does NOT override either --
   data_ov004_020bc0c0 slot 0 is 0x02043c80 and slot 3 is 0x02043bf0, the arm9
   ActorBase addresses, read out of extracted/overlays/overlay_0004.bin. A
   derived minigame may still override slot 0 (MgShuffleShell does, with
   _ZN14dScMgCurling_c13InitResourcesEv), which is why the fill keys on the ROM word rather than
   on the slot index. */
/* SLOT 0 IS CALLED AT C LINKAGE AND SLOT 3 AS A METHOD, and the asymmetry is
   the ROM's rather than a slip. src/_ZN7fBase_c13InitResourcesEv.cpp is
   "deliberately defined WITHOUT including ActorBase.h" -- its own header block
   says so at length: InitResources is the first virtual declared in the class,
   which makes it CW 1.2's KEY FUNCTION, and a real method definition there
   emits a second vtable for ActorBase on top of the one the module's gap
   object already supplies as ROM data, so the ROM link fails with
   "Multiply-defined: virtual table for ActorBase". The TU therefore defines
   the C name only, and a `((ActorBase *)self)->ActorBase::InitResources()`
   here does not resolve -- measured, as an unresolved
   ?InitResources@fBase_c@@UAEHXZ at link. CleanupResources is not the key
   function and IS a real method, which is why the two lines below differ. */
extern int _ZN7fBase_c13InitResourcesEv(void *self);
int port_scene_base_init(void *self)
{ return _ZN7fBase_c13InitResourcesEv(self); }
int port_scene_base_cleanup(void *self)
{ return ((fBase_c *)self)->fBase_c::CleanupResources(); }

}  /* extern "C" */
