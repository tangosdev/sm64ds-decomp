// BATCH-3 LINKAGE SEAT: MeshCollider's own ROM destructor pair, seated for the
// walk_window family only.
//
// The ROM has the Itanium destructor pair: D1 (complete-object, arm9 0x02039864)
// and D0 (deleting, arm9 0x0203982c), both 2004/b56 byte-matches
// (src/_ZN12MeshColliderD1Ev.c and _ZN12MeshColliderD0Ev.c). D0 tears down the
// KCL octree object, calls func_02039658 to restore the base vptr, then frees
// via Memory::operator_delete2; D1 is the same without the free, the body every
// derived collider dtor calls for its base subobject.
//
// WHY A SEPARATE FILE, NOT slice_gate8. The concrete MeshCollider table lives in
// hal/clsn_vtable.cpp, which EVERY collision target links -- including the
// gate-8/9 smoke runners (smoke_clsn, smoke_actor) that carry no level teardown.
// D0's body drags func_02038224, func_02039658 and Memory::operator_delete2 (in
// turn _ZdlPv), none of which those minimal targets link, and none of which they
// need: they construct the level collider and exit, they never delete it. So the
// static slot 0 stays slot_trap0 there, and this file -- added only to the
// walk_window family in CMakeLists, next to meshcolliderbase_vtable.cpp -- seats
// the real deleting body at boot.
//
// ABI. The flat-C D0 is `int *D0(int *this)` (cdecl, this on the stack). The
// MSVC vtable slot is __thiscall (this in ecx), so it cannot point at the flat
// body directly; slot_mc_dtor is the ecx->arg adapter, the same convention every
// slot in clsn_vtable.cpp uses. Naming BOTH flat bodies here is the reference
// edge: D0 for the slot, D1 kept so the complete-object body links alongside it.

extern "C" {

extern void *_ZTV12MeshCollider[13];      /* storage in hal/clsn_vtable.cpp */

int *_ZN12MeshColliderD0Ev(void *self);   /* deleting, arm9 0x0203982c */
int *_ZN12MeshColliderD1Ev(void *self);   /* complete, arm9 0x02039864 */

}

static void __fastcall slot_mc_dtor(void *self, void *)
{
    _ZN12MeshColliderD0Ev(self);
}

/* Kept referenced so the complete-object body (called by derived colliders'
   base-subobject teardown) is pulled into the link with the deleting body. */
static void *const g_keep_mc_d1 = (void *)&_ZN12MeshColliderD1Ev;

extern "C" void hal_seat_meshcollider_dtor(void)
{
    (void)g_keep_mc_d1;
    _ZTV12MeshCollider[0] = (void *)slot_mc_dtor;
}
