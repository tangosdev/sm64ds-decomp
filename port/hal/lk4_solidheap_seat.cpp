// Seat the whole solid-heap face: ActorBase::Virtual34/Virtual38 into slots
// 13/14 of _ZTV5Actor (data_0208e3a4), and SolidHeap's OWN sixteen-slot
// vtable, hosted here and filled with the class's matched bodies. This is
// the audited batch slice_gate16.txt's header deferred by name ("pull the
// whole Heap:: allocator face in"): the audit found every member of the
// closure either already linked or a matched TU, no host-ABI floor, no
// hardware poke, no inferred body, so the face comes in whole.
//
// THE EDGE. hal_fill_actor_base_vtable (gate 90, STAR_CAMERA) fills
// _ZTV5Actor at registry install and traps 13/14; STAR_CAMERA leaves that
// vptr installed and dispatches all 31 slots, so the table is live. This
// seat runs at the tail of port_stage_a2_seat, after the registry install,
// and replaces the two traps with the real matched bodies. Virtual34/38 are
// the per-instance heap hooks: an actor that wants a dedicated solid heap
// creates it through them (InitializeSolidHeapAsDefault -> CreateSolidHeap
// -> SolidHeap C1 installs _ZTV9SolidHeap), runs its v15 resource init
// inside it, then restores the default heap. Nothing on the castle grounds
// dispatches them today; the ROM bodies replacing return-0 traps is strictly
// more faithful when a level does.
//
// _ZTV9SolidHeap: SIXTEEN slots, the Heap shape (the layout heap_vtable.cpp
// documents for _ZTV13ExpandingHeap; both classes derive Heap and override
// the same list, and every SolidHeap V-method TU names its slot in its
// header comment). The storage lives HERE, not in heap_vtable.cpp, because
// the only TU that installs it (SolidHeap C1, matched .c) rides
// slice_gate16, so no other target ever names it.
//
// data_020a0ea8 is Memory::tmpHeapPtr, the saved-default slot
// RestoreFromTemporary reads and SetupSolidHeapAsDefault writes; its only
// referencing TUs ride this gate, so the storage lives here beside the
// table (heap_vtable.cpp hosts its sibling data_020a0ea0, the default-heap
// pointer, for every target).
//
// TWO NAME SPACES again: the .cpp bodies are MSVC methods, the .c bodies
// and several callees are flat C. The /alternatename spellings below came
// verbatim from the link errors (the heap_globals law).

#include "types.h"

// Flat C references -> the MSVC-method definitions the matched TUs emit
// (decorated names read from the compiled objs with dumpbin /SYMBOLS):
// the .c face TUs and Virtual34 call SetDefault/Destroy/ResizeToFit by
// their flat Itanium names while the defining TUs are real Heap methods.
#pragma comment(linker, "/alternatename:__ZN4Heap10SetDefaultEv=?SetDefault@Heap@@QAEHXZ")
#pragma comment(linker, "/alternatename:__ZN4Heap7DestroyEv=?Destroy@Heap@@QAEXXZ")
#pragma comment(linker, "/alternatename:__ZN4Heap11ResizeToFitEv=?ResizeToFit@Heap@@QAEIXZ")
// G is the ROM's shorthand for the default-heap pointer (decl_common.h
// `extern int G`), the same 0x020a0ea0 word heap_vtable.cpp hosts as
// data_020a0ea0 / Memory::defaultHeapPtr. One storage, one more name.
#pragma comment(linker, "/alternatename:_G=_data_020a0ea0")
// The remaining cross-namespace edges, spellings verbatim from the link
// errors: Virtual38 and the SolidHeap V-methods reference these as MSVC
// statics/methods while the definitions are flat C (or, for Rescue, a
// method whose matched TU returns int where the hostgen caller spelled
// void; same __thiscall, r0/EAX ignored, the ROM's own shape).
#pragma comment(linker, "/alternatename:?InitializeSolidHeapAsDefault@Heap@@SAPAU1@IPAU1@H@Z=__ZN4Heap28InitializeSolidHeapAsDefaultEjPS_i")
#pragma comment(linker, "/alternatename:?Allocate@Memory@@SAPAXIHPAUHeap@@@Z=__ZN6Memory8AllocateEjiP4Heap")
#pragma comment(linker, "/alternatename:?RestoreFromTemporary@Heap@@SAXXZ=__ZN4Heap20RestoreFromTemporaryEv")
#pragma comment(linker, "/alternatename:?_Destroy@Heap@@QAEXXZ=__ZN4Heap8_DestroyEv")
#pragma comment(linker, "/alternatename:?Rescue@Heap@@QAEXXZ=?Rescue@Heap@@QAEHXZ")
#pragma comment(linker, "/alternatename:?Allocate@SolidHeapAllocator@@QAEPAXIH@Z=__ZN18SolidHeapAllocator8AllocateEji")
#pragma comment(linker, "/alternatename:?Reallocate@SolidHeapAllocator@@QAEPAXPAXI@Z=__ZN18SolidHeapAllocator10ReallocateEPvj")

/* the matched MSVC-method bodies, dispatched qualified through local
   shadows; the signatures are copied from each TU's own declaration */
class SolidHeap
{
public:
    void *VAllocate(u32 size, u32 align);           /* slot 3 */
    void VDeallocate(void *p);                      /* slot 4 */
    void VDeallocateAll();                          /* slot 5 */
    bool VIntact();                                 /* slot 6 */
    void VRescue();                                 /* slot 7 */
    void *VReallocate(void *p, u32 size);           /* slot 8 */
    int VSizeof(void *p);                           /* slot 9 */
    unsigned VMaxAllocationUnitSize();              /* slot 10 */
    unsigned VMaxAllocatableSize();                 /* slot 11 */
    unsigned VMemoryLeft();                         /* slot 12 */
    u32 VSetNodeID(unsigned id);                    /* slot 13 */
    u32 VGetNodeID();                               /* slot 14 */
};

/* the matched Virtual34/38 are real ActorBase methods; this local decl
   produces the same ?Virtual34@ActorBase@@QAEHII@Z the TUs define */
struct ActorBase
{
    int Virtual34(u32 a, u32 b);
    int Virtual38(u32 a, u32 b);
};

extern "C" {

extern void *data_0208e3a4[31];       /* _ZTV5Actor, hal/actor_vtables.cpp */

void *_ZTV9SolidHeap[16];             /* installed by SolidHeap C1 */
void *data_020a0ea8;                  /* Memory::tmpHeapPtr */

/* the flat .c bodies */
void *_ZN9SolidHeapD1Ev(void *self);              /* slot 0 */
void *_ZN9SolidHeapD0Ev(void *self);              /* slot 1 */
void _ZN9SolidHeap8VDestroyEv(void *self);        /* slot 2 */
u32 _ZN9SolidHeap12VResizeToFitEv(void *self);    /* slot 15 */

}

static void __fastcall sh_d1(void *s, void *)       { _ZN9SolidHeapD1Ev(s); }
static void __fastcall sh_d0(void *s, void *)       { _ZN9SolidHeapD0Ev(s); }
static void __fastcall sh_vdestroy(void *s, void *) { _ZN9SolidHeap8VDestroyEv(s); }
static void *__fastcall sh_alloc(void *s, void *, u32 size, u32 align)
{ return ((SolidHeap *)s)->VAllocate(size, align); }
static void __fastcall sh_dealloc(void *s, void *, void *p)
{ ((SolidHeap *)s)->VDeallocate(p); }
static void __fastcall sh_dealloc_all(void *s, void *)
{ ((SolidHeap *)s)->VDeallocateAll(); }
static int __fastcall sh_intact(void *s, void *)
{ return ((SolidHeap *)s)->VIntact(); }   /* int widens bool the ARM r0 way */
static void __fastcall sh_rescue(void *s, void *)
{ ((SolidHeap *)s)->VRescue(); }
static void *__fastcall sh_realloc(void *s, void *, void *p, u32 size)
{ return ((SolidHeap *)s)->VReallocate(p, size); }
static int __fastcall sh_sizeof(void *s, void *, void *p)
{ return ((SolidHeap *)s)->VSizeof(p); }
static u32 __fastcall sh_maxunit(void *s, void *)
{ return ((SolidHeap *)s)->VMaxAllocationUnitSize(); }
static u32 __fastcall sh_maxalloc(void *s, void *)
{ return ((SolidHeap *)s)->VMaxAllocatableSize(); }
static u32 __fastcall sh_memleft(void *s, void *)
{ return ((SolidHeap *)s)->VMemoryLeft(); }
static void __fastcall sh_setnodeid(void *s, void *, u32 id)
{ ((SolidHeap *)s)->VSetNodeID(id); }
static u32 __fastcall sh_getnodeid(void *s, void *)
{ return ((SolidHeap *)s)->VGetNodeID(); }
static u32 __fastcall sh_resizetofit(void *s, void *)
{ return _ZN9SolidHeap12VResizeToFitEv(s); }

static int __fastcall ab_v34(void *s, void *, u32 a, u32 b)
{ return ((ActorBase *)s)->ActorBase::Virtual34(a, b); }
static int __fastcall ab_v38(void *s, void *, u32 a, u32 b)
{ return ((ActorBase *)s)->ActorBase::Virtual38(a, b); }

extern "C" void hal_seat_solidheap(void)
{
    void **vt = _ZTV9SolidHeap;
    vt[0]  = (void *)sh_d1;
    vt[1]  = (void *)sh_d0;
    vt[2]  = (void *)sh_vdestroy;
    vt[3]  = (void *)sh_alloc;
    vt[4]  = (void *)sh_dealloc;
    vt[5]  = (void *)sh_dealloc_all;
    vt[6]  = (void *)sh_intact;
    vt[7]  = (void *)sh_rescue;
    vt[8]  = (void *)sh_realloc;
    vt[9]  = (void *)sh_sizeof;
    vt[10] = (void *)sh_maxunit;
    vt[11] = (void *)sh_maxalloc;
    vt[12] = (void *)sh_memleft;
    vt[13] = (void *)sh_setnodeid;
    vt[14] = (void *)sh_getnodeid;
    vt[15] = (void *)sh_resizetofit;

    /* the per-instance heap hooks, replacing star_trap13/14 after the
       registry install's fill has run */
    data_0208e3a4[13] = (void *)ab_v34;
    data_0208e3a4[14] = (void *)ab_v38;
}
