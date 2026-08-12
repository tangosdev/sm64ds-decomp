// LANE-LK1 LINKAGE SEAT: ExpandingHeap's VMax pair, seated for the
// walk_window family only.
//
// Slots 10 and 11 of _ZTV13ExpandingHeap are VMaxAllocationUnitSize (arm9
// 0x0203c5f8) and VMaxAllocatableSize (arm9 0x0203c5d0), both 2004/b56
// byte-matches in src/. Each is the same one-line forwarder: it reads the
// allocator at this+0x14 and calls the allocator's flat-C MaxAllocatableSize
// (arm9 0x0204e0f8, also a byte-match) with align 4, which walks the free
// list for the largest usable run.
//
// WHY A SEPARATE FILE, NOT gate 3a. The table lives in hal/heap_vtable.cpp,
// which EVERY target links, and gate 3a is the slice every target carries --
// that is where the zero-closure V-methods went. This pair cannot follow
// them: MaxAllocatableSize calls cstd::abs, whose matched TU rides gate 4b,
// and the minimal root-heap target (smoke_roots) links gate 3a without gate
// 4b. Enrolling abs in 3a as well would define it twice in every target that
// links both. So the three TUs ride slice_gate16 (walk_window family, which
// carries gate 4b), the static slots 10/11 keep their traps everywhere else
// (no caller evidence anywhere, so nothing dispatches them), and this file --
// added only to the walk_window family in CMakeLists, next to
// meshcollider_dtor_seat.cpp -- seats the real bodies at boot.
//
// NAME SPACES. Both V-method TUs define real MSVC methods on struct
// ExpandingHeap (include/ExpandingHeap.h), so the shadow struct here resolves
// to them directly, no alias machinery; the flat-C MaxAllocatableSize is
// pulled in by the V-bodies' own references. The slot thunks are the same
// ecx->this __fastcall shims as the rest of the table in heap_vtable.cpp.

typedef unsigned int u32;

struct ExpandingHeap {
    u32 VMaxAllocationUnitSize();
    u32 VMaxAllocatableSize();
};

extern "C" void *_ZTV13ExpandingHeap[16];  /* storage in hal/heap_vtable.cpp */

static u32 __fastcall slot_eh_vmaxunit(void *self, void *)
{ return ((ExpandingHeap *)self)->VMaxAllocationUnitSize(); }
static u32 __fastcall slot_eh_vmaxsize(void *self, void *)
{ return ((ExpandingHeap *)self)->VMaxAllocatableSize(); }

/* Called from level_boot's boot fills. Naming the two matched bodies here is
   the reference edge that pulls them, and the allocator's MaxAllocatableSize
   they forward to, into the link. */
extern "C" void hal_seat_expandingheap_vmax(void)
{
    _ZTV13ExpandingHeap[10] = (void *)slot_eh_vmaxunit;
    _ZTV13ExpandingHeap[11] = (void *)slot_eh_vmaxsize;
}
