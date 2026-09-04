//cpp
/* ov047/daObjKm3_Dorifu_c -- the Bowser-in-the-Sky drifting platform.
 *
 * ENROLLED AND CANONICAL. One genuine translation unit built from five
 * one-function sources, linked as a single .text run 0x02111510..0x021116dc.
 * It began as a tools/tubuild.py shadow; the shadow's unreconciled local
 * declarations have since been replaced by the real project headers, which is
 * the reconciliation step plan sec 7.3 leaves to review.
 *
 * IT IS ALSO THE CLASS'S KEY-FUNCTION TU. With ~daObjKm3_Dorifu_c() inline in
 * include/daObjKm3_Dorifu_c.h the key function is the first DECLARED
 * non-inline virtual -- CleanupResources -- so mwccarm emits this class's
 * _ZTV/_ZTI/_ZTS group here as vague linkage, along with the whole inherited
 * RTTI chain. All thirteen records are licensed in the manifest as
 * deadstrip-data with a canonical module and address, and romdata_check
 * word-compares each against the cartridge with relocations applied:
 * 9 VERIFIED, 4 PARTIAL, 0 DIFFERS. The 4 PARTIAL are shared base _ZTS
 * strings only, the known benign pattern.
 *
 * _ZTV17daObjKm3_Dorifu_c is VERIFIED at 128 bytes, and 128 is the full span
 * of the table's SLOTS rather than a comfortable prefix: the address point is
 * ov047 0x0211254c and the next symbol outside the table,
 * data_ov047_021125cc, begins at exactly 0x0211254c + 128. (The two symbols
 * listed between them, at 0x02112570 and 0x02112590, fall inside the table --
 * cross-overlay phantoms, not a boundary.) What this does NOT cover is the
 * 8-byte {offset-to-top, _ZTI pointer} header at 0x02112544, which is emitted
 * but which no word comparison in the tree reaches.
 *
 * None of the above was reachable while the class carried the coined name
 * "RickshawPlatformBs": _ZTS is a length-prefixed mangled string, so it could
 * not have matched the cartridge at any address, and a class whose RTTI
 * cannot be compared cannot license its own vtable.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02111510  src/_ZN17daObjKm3_Dorifu_cD1Ev.cpp
 *   [1] 0x02111590  src/_ZN17daObjKm3_Dorifu_cD0Ev.cpp
 *   [2] 0x02111624  src/_ZN17daObjKm3_Dorifu_c16CleanupResourcesEv.cpp
 *   [3] 0x02111638  src/_ZN17daObjKm3_Dorifu_c13InitResourcesEv.cpp
 *   [4] 0x0211164c  src/StairsBs_Spawn.cpp
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "daObjKm3_Dorifu_c.h"
#include "types.h"

/* The resource table this platform hands to its base, five 0xc-byte entries
 * at ov047 0x02112508. daObjDorifuResources and the two base methods that
 * consume it are declared by include/daObjDorifu_c.h, which daObjKm3_Dorifu_c.h
 * includes -- so no shadow declaration is needed for either.
 *
 * tubuild's generated shadow block spelled this differently and WRONGLY: a
 * local `struct Arg { void *m[3]; }` plus raw `func_ov002_020b4b6c` /
 * `func_ov002_020b4d58` externs. Those two symbols are not exported by any
 * module, so the TU compiled but the ROM link failed with `Undefined :
 * "func_ov002_020b4b6c"`. The legacy one-function sources this TU replaces
 * had already recovered the real base calls; the generator regressed them,
 * and what follows is the legacy recovery restored, not a new guess. */
extern daObjDorifuResources data_ov047_02112508[5];

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- StairsBs_Spawn, 0x0211164c, size 0x90 */
/* -------------------------------------------------------------------------- */
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void*);
extern void func_020733a8(void*,int,int,void*,void*);
extern int _ZTV13daObjDorifu_c[];
extern int _ZTV17daObjKm3_Dorifu_c[];
extern void _ZN5ModelD1Ev(void*);
extern void _ZN5ModelC1Ev(void*);
extern void _ZN10dBgW_KcMbgD1Ev(void*);
extern void _ZN10dBgW_KcMbgC1Ev(void*);
void* StairsBs_Spawn(void){
  char* c = (char*)_ZN7fBase_cnwEj(0xdcc);
  if(c){
    _ZN10dBgActor_cC2Ev(c);
    *(int*)c = (int)_ZTV13daObjDorifu_c;
    func_020733a8(c+0x320, 5, 0x50, (void*)_ZN5ModelC1Ev, (void*)_ZN5ModelD1Ev);
    func_020733a8(c+0x4b0, 5, 0x1c8, (void*)_ZN10dBgW_KcMbgC1Ev, (void*)_ZN10dBgW_KcMbgD1Ev);
    /* This TU DEFINES _ZTV17daObjKm3_Dorifu_c (it anchors the vtable via the
     * out-of-line destructor above), so the vptr store must skip the two
     * hidden ABI slots with +2 -- see notes/key-function-tu-vptr-store-blocker.md.
     * _ZTV13daObjDorifu_c above stays addend-0: that base's vtable is still
     * imported (UNDEF) from elsewhere, not defined in this TU. */
    *(int*)c = (int)(_ZTV17daObjKm3_Dorifu_c + 2);
  }
  return c;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN17daObjKm3_Dorifu_c13InitResourcesEv, 0x02111638, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjKm3_Dorifu_c13InitResourcesEv
/* The marker above is not decoration: without it tools/tiers.py folds this
 * body into the neighbouring fragment and scores that fragment's mangled
 * vtable externs against InitResources. */
/* recovered: named members + shared header, real C++ method */
s32 daObjKm3_Dorifu_c::InitResources()
{
    return daObjDorifu_c::InitResources(data_ov047_02112508);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN17daObjKm3_Dorifu_c16CleanupResourcesEv, 0x02111624, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjKm3_Dorifu_c16CleanupResourcesEv
/* Same reason for the marker as InitResources above. */
/* recovered: named members + shared header, real C++ method */
s32 daObjKm3_Dorifu_c::CleanupResources()
{
    return daObjDorifu_c::CleanupResources(data_ov047_02112508);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- one definition, two ROM-visible variants:          */
/*   _ZN17daObjKm3_Dorifu_cD1Ev  0x02111510  size 0x80  (complete-object)    */
/*   _ZN17daObjKm3_Dorifu_cD0Ev  0x02111590  size 0x94  (deleting)          */
/* -------------------------------------------------------------------------- */
/* recovered: real C++ destructor -- the compiler emits the whole body.
 *
 * Three vtable stores and five destructor calls, every one a consequence of
 * `struct daObjKm3_Dorifu_c : daObjDorifu_c : dBgActor_c`: its own vptr, then
 * daObjDorifu_c's, then that class's dBgW_KcMbg[5] at 0x4b0 and Model[5]
 * at 0x320 through __destroy_arr, then dBgActor_c's vptr and dBgActor_c's own
 * dBgW_KcMbg and Model, then dActor_c. The two arrays are declared in
 * include/daObjDorifu_c.h, so the empty body reproduces them. D0 additionally
 * returns the object to its heap through the inline operator delete it
 * inherits, which is why nothing here mentions a heap.
 *
 * In the one-function tree these were two files, each carrying this same
 * empty body and each having objisolate keep the variant its filename named.
 * In TU context one definition produces both -- and, if it is written out of
 * line HERE, a third: _ZN17daObjKm3_Dorifu_cD2Ev, byte-identical to D1, which
 * the ROM does not contain (see decomp-cpp-class-form and the matching
 * PoleLift/Squasher precedent in src_tu/actors/).
 *
 * So there is deliberately no definition at this point. `~daObjKm3_Dorifu_c()
 * {}` is INLINE in include/daObjKm3_Dorifu_c.h, which is what emits D1 and D0
 * in the ROM's order and suppresses the homeless D2. See the header for why
 * that is required rather than merely preferred.
 */
