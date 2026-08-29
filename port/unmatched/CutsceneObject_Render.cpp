/* HOST COPY of src/_ZN14CutsceneObject6RenderEv.cpp -- ONE TOKEN CHANGED.
 *
 * THE DIVERGENCE, NAMED. The matched source dispatches its model through a
 * LOCAL SHADOW class, and that shadow counts in ROM/Itanium numbering:
 *
 *     struct ModelBase {
 *       virtual void v0(); virtual void v1(); virtual void v2();
 *       virtual void v3(); virtual void v4(); virtual void m(int arg);
 *     };                                          // m == index 5
 *     ((ModelBase *)*(void **)&mModel)->m((int)&unk_080);
 *
 * ROM slot 5 is what mwcc reaches there, and include/ModelAnim.h reads the ROM
 * table out verbatim:
 *
 *     slot 5  0x020167f8  ModelAnim::Render(Vector3 const *)
 *     slot 6  0x020167c4  ModelAnim::Virtual18(u32, Vector3 const *)
 *
 * so on the ROM this is Render(scale) -- one argument. MSVC folds the D1/D0
 * pair into one slot, so every host table in this port is one slot low, and
 * index 5 there is _ZTV9ModelAnim[5] = ModelAnim::Virtual18(u32, Vector3 const*)
 * -- a DIFFERENT function with TWO parameters, taking &unk_080 as a matrix id
 * and reading `scale` off whatever follows it. The renderer then walks an Entry
 * chosen by a garbage index, which is the `part->cmd == 9` fault (an odd
 * address, and *cmd is the only odd-addressable read in src/func_0204488c.c).
 *
 * THE TREE PREDICTED THIS TU BY NAME. hal/cxxname_bridge.cpp says at the
 * ModelAnim fill: "No dual-fill here: Render's ROM slot (5) is Virtual18's MSVC
 * slot, so shadow-TU Render dispatch cannot be served by the same array --
 * trap-by-Virtual18 will name it if such a TU ever appears." It appeared, and
 * the trap fired exactly as advertised: ModelAnim::Virtual18 turned up in a
 * CutsceneObject::Render stack, where it has no business being.
 *
 * WHY THE FIX IS HERE AND NOT IN THE TABLE. data_ov002_0210bcc4 has MIXED
 * consumers -- ModelBase::SetFile is MSVC-numbered and wants [1] = DoSetFile,
 * this TU is ROM-numbered and wants [5] = Render. Renumbering the array serves
 * one and breaks the other, and EVERY ModelAnim in the port dispatches through
 * _ZTV9ModelAnim. [[sm64ds-port-msvc-dtor-slot-shift]] prescribes copying the
 * CONSUMER for precisely this shape (port/unmatched/CylinderClsn_Process.cpp is
 * the same pattern), so hal/cxxname_bridge.cpp stays at 0 changes.
 *
 * WHY ONLY THE ANIM PAIR WAS EVER BITTEN. _ZTV5Model IS dual-filled -- [4] and
 * [5] both hold Render -- so the five plain-Model cast members (three pipes,
 * the letter, the cloud) reached Render through slot 5 by luck. Only the two
 * ModelAnim members, the Lakitu (0x12) and the Peach (0x13), could fault, and
 * those are exactly the two the load trace proved healthy end to end.
 *
 * THE CHANGE: the shadow loses v4, so `m` sits at index 4. That is Render in
 * MSVC numbering on BOTH tables (_ZTV5Model[4] = mv_render,
 * _ZTV9ModelAnim[4] = ma2_render), so the plain-Model path keeps landing on the
 * same function it already landed on and the ModelAnim path is corrected.
 * Everything else is the matched source line for line, declarations included --
 * `data_0209b41c` in particular keeps its exact declaration form so the
 * /alternatename bridge in hal/actor_classes_intro.cpp still spells it.
 *
 * Src line commented out in port/slice_intro.txt.
 */
#include <cstdio>
#include <cstdlib>

#include "decl_common.h"
#include "CutsceneObject.h"

extern "C" {
void _ZN9ModelBase12ApplyOpacityEj(void* m, unsigned int op, int z);
void func_ov002_020f65b8(void* o);
}
struct M48 { int w[12]; };
extern M48 data_0209b41c;

/* MSVC numbering: the folded destructor is slot 0, so ROM slot 5 (Render) is
   index 4 here. See the header -- this single dropped declaration IS the fix. */
struct ModelBase {
  virtual void v0(); virtual void v1(); virtual void v2();
  virtual void v3(); virtual void m(int arg);
};

/* Per-object render trace. Inert unless SM64DS_INTRO_WATCH. Prints which member
   is live (+0xdc plain Model vs +0xe0 ModelAnim), the vtable word actually
   about to be called, and the Entry table the renderer will walk -- so a future
   dispatch fault names its object and its slot instead of being inferred. */
static void co_render_watch(const CutsceneObject* self, const void* obj,
                            const char* which)
{
    static int on = -1;
    if (on < 0)
        on = std::getenv("SM64DS_INTRO_WATCH") ? 1 : 0;
    if (!on)
        return;
    void* const* vt = obj ? *(void* const* const*)obj : 0;
    const char* mc = obj ? (const char*)obj + 8 : 0;
    void* info = mc ? *(void* const*)mc : 0;
    void* entries = info ? *(void* const*)((const char*)info + 0x10) : 0;
    std::fprintf(stderr,
                 "  [render] obj %p unk8 0x%02x via %s | model %p | vt %p"
                 " | slot4 %p | info %p | entries %p\n",
                 (const void*)self, (unsigned)self->unk_008, which, obj,
                 (const void*)vt, vt ? vt[4] : 0, info, entries);
}

int CutsceneObject::Render()
{
  if (unk_008 == 0x19){
    struct { char* p; char* cur; M48* src; } s;
    s.src = &data_0209b41c;
    s.p = *(char**)((char*)&mModel) + 0x1c;
    s.cur = s.p;
    *(M48*)s.cur = *s.src;
    int* tbl = data_ov002_0210bb7c;
    int i = 0;
    int zero = 0;
    co_render_watch(this, *(void**)((char*)&mModel), "mModel/0x19");
    do {
      *(int*)(s.p + 0x24) = tbl[0];
      *(int*)(s.p + 0x28) = tbl[1];
      *(int*)(s.p + 0x2c) = tbl[2];
      ((ModelBase*)*(void**)((char*)&mModel))->m(zero);
      tbl += 3;
      i++;
    } while ((unsigned)i < 3u);
    return 1;
  }
  unsigned char op = unk_102;
  if (op == 0) return 1;
  {
    void* a = *(void**)((char*)&mModel);
    if (a != 0){
      a = (void*)((int)a);
      co_render_watch(this, a, "mModel");
      _ZN9ModelBase12ApplyOpacityEj(a, op, 0);
      ((ModelBase*)*(void**)((char*)&mModel))->m((int)((char*)&unk_080));
    } else {
      void* b = *(void**)((char*)&unk_0e0);
      if (b != 0){
        b = (void*)((int)b);
        co_render_watch(this, b, "unk_0e0");
        func_ov002_020f65b8(b);
        _ZN9ModelBase12ApplyOpacityEj(*(void**)((char*)&unk_0e0), unk_102, 0);
        ((ModelBase*)*(void**)((char*)&unk_0e0))->m((int)((char*)&unk_080));
      }
    }
  }
  return 1;
}
