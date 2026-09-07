/* HOST COPIES of src/_ZN6Rabbit6RenderEv.cpp and
 * src/_ZN9LakituBro6RenderEv.cpp -- the one slot the ROM's and MSVC's vtable
 * numbering cannot both be served by one array.
 *
 * Gate 17 hit this with Bird::Render and FLAG's and wrote it down:
 * hal/cxxname_bridge.cpp fills _ZTV9ModelAnim in MSVC order (dtor 0,
 * DoSetFile 1, UpdateVerts 2, Virtual10 3, Render 4, Virtual18 5) because
 * MSVC spends ONE slot on the destructor where Itanium spends two. A TU that
 * dispatches through a LOCAL SHADOW CLASS counts in the ROM's numbering, so
 * its "slot 5" is Render -- and slot 5 of the host array is Virtual18. Both
 * of these files do exactly that:
 *
 *   Rabbit::Render     ((VObj *)&mModelAnim)->m14(&mScale)      six virtuals
 *   LakituBro::Render  ((Sub *)&mModelAnim1)->g5(0)             six virtuals
 *
 * Gate 17's two were one line each and became faces in hal/method_faces.cpp.
 * These have bodies, so they are host copies instead, and the bodies are the
 * matched sources' control flow line for line -- only the final dispatch is
 * spelled as the qualified ModelAnim::Render the ROM means.
 *
 * Measured before the fix: LakituBro drew nothing and his model matrix read
 * back (0,0,0) while the actor sat at (0,1088,2200); Virtual18 takes TWO
 * arguments where the shadow's slot-5 declaration passes one, so it was also
 * reading its scale off the stack.
 */
#include "ModelAnim.h"

extern "C" {

int _ZN15TextureSequence6UpdateER15ModelComponents(void *self, void *mc);
void func_ov085_0212c150(void *self);
extern signed char data_0209f2f8;   /* the level id */
extern signed char data_02092120;   /* the area currently shown */

/* _ZN6Rabbit6RenderEv RETIRED (run link100, lane EXCEPT). Its stated reason -- the
   ROM-order model slot-5 dispatch -- died with lane SLOT5F's
   respelling of include/ModelBase.h: hal/cxxname_bridge.cpp:522/578
   put Render back on index 5 of _ZTV5Model and _ZTV9ModelAnim, so the
   matched source's local six-virtual shadow reaches the body it means.
   The C name is defined in hal/except_faces.cpp onto the matched
   __thiscall method; the ROM vtable word and the kind:function record
   are in port/slice_except2.txt. */


/* _ZN9LakituBro6RenderEv RETIRED (run link100, lane EXCEPT). Its stated reason -- the
   ROM-order model slot-5 dispatch -- died with lane SLOT5F's
   respelling of include/ModelBase.h: hal/cxxname_bridge.cpp:522/578
   put Render back on index 5 of _ZTV5Model and _ZTV9ModelAnim, so the
   matched source's local six-virtual shadow reaches the body it means.
   The C name is defined in hal/except_faces.cpp onto the matched
   __thiscall method; the ROM vtable word and the kind:function record
   are in port/slice_except2.txt. */


/* ---- RABBIT_KEY (actor 229) ---------------------------------------------
   The third ov085 render host copy, and the same final-dispatch reason with
   a MEASURED twist: the matched TU dispatches slot +0x14 of the plain Model
   at +0x110 through a local shadow struct whose fn field is a PLAIN (cdecl)
   function pointer. _ZTV5Model[5] does dual-fill mv_render for the shadow
   slot NUMBERING -- but mv_render is __fastcall(self, dummy, s), so the
   cdecl shadow call leaves ECX garbage and mv_render renders a Model at
   whatever ECX held (measured: the spawn-assisted key crashed frame 1,
   ecx=13811ba3, DEP-exec fault). The numbering dual-fill cannot fix a
   convention mismatch; the qualified call below is the draw the ROM means.
   The angle bump is the caught-state spin (state == &data_ov085_0213072c,
   the State object __sinit_ov085_0212f9bc builds). */
extern char data_ov085_0213072c[];
void func_ov085_0212d2b8(void *self);
/* PORT_HOST_ABI: ROM-order model slot-5 dispatch AND a convention
 * mismatch: the shadow's fn field is cdecl where mv_render is __fastcall.
 * See the note above. */
int _ZN9RabbitKey6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if (*(void **)(c + 0x188) == (void *)data_ov085_0213072c)
        *(short *)(c + 0x8e) += 0x500;
    func_ov085_0212d2b8(c);
    /* ((Sub *)&mModel)->fn14(this+0x110, 0): Model at +0x110, ROM slot 5. */
    ((Model *)(c + 0x110))->Model::Render(0);
    return 1;
}

}  /* extern "C" */
