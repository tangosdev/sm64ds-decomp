/* PORT_HOST_ABI. CHEEP_CHEEP (227, 12daPukupuku_c), ov090. Run rel0215 wave 2,
 * lane cast-ov090. Three sites; see unmatched/Skeeter_HostSites.cpp for the
 * full derivation of the ModelAnim and PMF shapes.
 *
 * (1) src/_ZN10CheepCheep6RenderEv.cpp -- the ModelAnim slot-5 collision (T1),
 *     guarded by unk_0b0 & 0x40000 the way Skeeter's is, null scale.
 * (2) src/func_ov090_021332e8.cpp -- the state setter, record 0 (ENTER).
 * (3) src/_ZN10CheepCheep8BehaviorEv.cpp -- the record-1 (TICK)
 *     pointer-to-member call. This one is spelled as a REAL C++ METHOD in the
 *     source (`int CheepCheep::Behavior()`, out of include/CheepCheep.h) with
 *     the PMF taken through a forward-declared `struct Enemy;` -- incomplete
 *     at the typedef, so MSVC's four-word unknown-inheritance form again.
 *     No PathPtr in this one.
 */
#include "ModelAnim.h"

extern "C" {

/* _ZN10CheepCheep6RenderEv RETIRED (run link100, lane EXCEPT). Its stated reason -- the
   ROM-order model slot-5 dispatch -- died with lane SLOT5F's
   respelling of include/ModelBase.h: hal/cxxname_bridge.cpp:522/578
   put Render back on index 5 of _ZTV5Model and _ZTV9ModelAnim, so the
   matched source's local six-virtual shadow reaches the body it means.
   The C name is defined in hal/except_faces.cpp onto the matched
   __thiscall method; the ROM vtable word and the kind:function record
   are in port/slice_except2.txt. */


/* ---- (2) and (3) ------------------------------------------------------- */
struct PortOv090Pmf { unsigned int fn; int delta; };
typedef int (*PortOv090StateFn)(void *);

/* func_ov090_021332e8 IS NOT A HOST COPY ANY MORE. Run link100 lane PMF2 put
   src/func_ov090_021332e8.cpp back on port/slice_pmf2.txt: with /vmg /vmm global (the
   R8 block in port/CMakeLists.txt) MSVC's pointer-to-member IS the ROM's
   8-byte {function, delta} pair, and the matched TU compiles to the same
   tail jump this body was -- measured, listing in that slice's header.
   The reading above is kept because it is the derivation. */
int  _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(void *self, void *wm);
void _ZN12CylinderClsn5ClearEv(void *c);
void _ZN12CylinderClsn6UpdateEv(void *c);
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *thiz, void *clsn);
void _ZN9Animation7AdvanceEv(void *thiz);
char *_ZN5Actor13ClosestPlayerEv(void *thiz);
void func_ov090_02133338(void *c);
void func_ov090_021330c8(void *c);

/* HOST COPY RETIRED, run link100 lane PMFB7 gate 1. src/_ZN10CheepCheep8BehaviorEv.cpp
   dispatches its own field now: with /vmg /vmm (block R8) MSVC's pointer to
   member IS the ROM's eight-byte {code, adjust} pair, so the widening this
   banner was written for does not happen. The per-frame half of every state
   cell holds a zero-argument __fastcall face; the enter half does not change,
   because the helper that dispatches it tail-jumps. Measurements in
   port/slice_pmfb7.txt and runs/link100/out/PMFB7/. */

}  /* extern "C" */
