/* HOST COPY of src/_ZN14CutsceneObject16CleanupResourcesEv.cpp -- ONE TOKEN
 * CHANGED, and it is the SAME defect as unmatched/CutsceneObject_Render.cpp,
 * found by asking whether the class had any other shadow-dispatch TU.
 *
 * THE DIVERGENCE. The matched source destroys its model through a local shadow
 * counted in ROM/Itanium numbering:
 *
 *     struct Obj { virtual void v00(); virtual void m04(); };
 *     Obj *a = *(Obj **)&mModel;   if (a) a->m04();
 *     Obj *b = *(Obj **)&unk_0e0;  if (b) b->m04();
 *
 * `m04` is index 1. On the ROM the model tables are Itanium-shaped and carry
 * the destructor TWICE -- slot 0 the complete D1, slot 1 the DELETING D0 --
 * which hal/actor_classes_intro.cpp's own seat records off the ROM relocations:
 *
 *     data_ov002_0210bae4  +0x00 0x020f6a00 D1   +0x04 0x020f69a8 D0
 *     data_ov002_0210bcc4  +0x00 0x020f6870 D1   +0x04 0x020f6778 D0
 *
 * so index 1 is the deleting destructor, which is exactly what a cleanup wants.
 *
 * MSVC folds that pair into a single slot 0, so index 1 on the host tables is
 * _ZTV5Model[1] / _ZTV9ModelAnim[1] = Model::DoSetFile. Cleaning up the cast
 * would have called DoSetFile with the destructor's argument list: a load, not
 * a free, on a model that is being thrown away. It never crashed because
 * nothing reached cleanup -- the run faulted in Render first, on the SAME
 * one-slot shift one file over.
 *
 * THE CHANGE: the shadow loses v00, so `m04` sits at index 0 -- the folded
 * deleting destructor in MSVC numbering, which hal/actor_classes_intro.cpp
 * seats with each class's own ROM D0 body.
 *
 * AND THE CALLING CONVENTION WENT WITH IT. Those two ROM D0 bodies are plain C
 * functions taking `this` as an ordinary first argument, and the seat used to
 * store their raw addresses under a comment saying it never calls them. This
 * file is what starts calling them, so the seat now stores __fastcall faces
 * (co_model_d0 / co_modelanim_d0) instead -- the same shape every other slot in
 * those two arrays already had. Both halves of that change are required
 * together: index 0 without the face would enter a cdecl body thiscall.
 *
 * Everything else is the matched source line for line, the doubled `if (a)`
 * included. Src line commented out in port/slice_intro.txt.
 */
#include "CutsceneObject.h"
extern "C" {
int func_ov002_020f63a0(void* thiz);
int func_ov002_020f23d0(void* c);
}
/* MSVC numbering: the folded deleting destructor is index 0. See the header --
   this single dropped declaration IS the fix. */
struct Obj { virtual void m04(); };

int CutsceneObject::CleanupResources()
{
  int r1 = unk_008;
  if (r1 == 0x2e) return func_ov002_020f63a0(((char*)this));
  if (r1 == 0x2f) return func_ov002_020f23d0(((char*)this));
  Obj* a = *(Obj**)((char*)&mModel);
  if (a) if (a) a->m04();
  Obj* b = *(Obj**)((char*)&unk_0e0);
  if (b) if (b) b->m04();
  return 1;
}
