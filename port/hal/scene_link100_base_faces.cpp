/* ---- link100 SCENE: the two ActorBase halves an ARM9 scene table holds ------
 *
 * hal/scene_actor_faces.cpp already carries six of the ActorBase/ActorDerived
 * lifecycle halves a Scene vtable can dispatch. The two arm9-resident scene
 * classes this run seats -- dScBoot_c (id 0, vtable 0x02091528) and dScMB_c
 * (id 360, vtable 0x020943c4) -- are the first tables in the port that leave
 * TWO MORE of them unoverridden, so they are the first that need faces for
 * them:
 *
 *   slot  9  Render             0x02043af0  _ZN9ActorBase6RenderEv
 *   slot 12  OnPendingDestroy   0x02043ac0  _ZN9ActorBase16OnPendingDestroyEv
 *
 * Both addresses are read out of config/arm9/relocs.txt at the table word, not
 * inferred from a slot number:
 *
 *   from:0x0209154c kind:load to:0x02043af0 module:main   (Boot slot 9)
 *   from:0x02091558 kind:load to:0x02043ac0 module:main   (Boot slot 12)
 *   from:0x020943f4 kind:load to:0x02043ac0 module:main   (MB   slot 12)
 *
 * and config/arm9/symbols.txt names both:
 *   _ZN9ActorBase6RenderEv           kind:function(arm,size=0x8)  addr:0x02043af0
 *   _ZN9ActorBase16OnPendingDestroyEv kind:function(arm,size=0x4) addr:0x02043ac0
 *
 * NEITHER IS A VENEER. Render is an 8-byte `return 1` (VS_FAIL) and
 * OnPendingDestroy is a bare `bx lr`, so there are no arguments to ride
 * through and nothing here has the shape of the three tail-call veneers the
 * file above this one exists for.
 *
 * SEPARATE FILE for the reason hal/scene_actor_faces.cpp gives for being one:
 * these bodies are real C++ methods against include/ActorBase.h, and the seat
 * files that call them declare their own flat structs. One include per file.
 */
#include "ActorBase.h"

extern "C" {

/* slot 9. src/_ZN9ActorBase6RenderEv.cpp is a //cpp TU that defines the real
   method, so the call below resolves to ?Render@ActorBase@@UAEHXZ -- the same
   shape port_scene_base_cleanup uses for CleanupResources one file over. */
int port_scene_link100_base_render(void *self)
{ return ((ActorBase *)self)->ActorBase::Render(); }

/* slot 12. Same shape; src/_ZN9ActorBase16OnPendingDestroyEv.cpp is the body
   and it is already in the link through port/slice_gate9.txt. */
void port_scene_link100_base_pending_destroy(void *self)
{ ((ActorBase *)self)->ActorBase::OnPendingDestroy(); }

}  /* extern "C" */
