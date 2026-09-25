/* THE TWELVE VIRTUAL MEMBER POINTERS fBase_c::Process IS HANDED (run linkfull,
 * lane PMF3).
 *
 * The four ActorBase::Process wrappers -- func_0204335c (init), func_02043288
 * (behaviour), func_0204322c (render) and func_020432e4 (cleanup) -- each pass
 * three of arm9's static member-pointer records into
 *
 *     fBase_c::Process(self, <main>, <before>, <after>)
 *
 * (src/_ZN7fBase_c7ProcessEMS_FivEMS_FbvEMS_FvjE.cpp), which calls before(),
 * then main() when before() said go, then after(code). Read out of
 * extracted/arm9_dec.bin (base 0x02004000), no relocation on any of the
 * twenty-four words, every record is the ARM virtual form {vtable byte offset,
 * 1}: bit 0 of the adjustment word says VIRTUAL, the adjustment itself is 0,
 * and the ROM's call loads the vptr of `this` and calls the slot at that byte
 * offset.
 *
 *     0x02099e74 {0x28,1} slot 10  BeforeRender           (func_0204322c)
 *     0x02099e7c {0x10,1} slot  4  BeforeCleanupResources (func_020432e4)
 *     0x02099e84 {0x14,1} slot  5  AfterCleanupResources  (func_020432e4)
 *     0x02099e8c {0x0c,1} slot  3  CleanupResources       (func_020432e4)
 *     0x02099e94 {0x08,1} slot  2  AfterInitResources     (func_0204335c)
 *     0x02099e9c {0x2c,1} slot 11  AfterRender            (func_0204322c)
 *     0x02099ea4 {0x18,1} slot  6  Behavior               (func_02043288)
 *     0x02099eac {0x1c,1} slot  7  BeforeBehavior         (func_02043288)
 *     0x02099eb4 {0x20,1} slot  8  AfterBehavior          (func_02043288)
 *     0x02099ebc {0x00,1} slot  0  InitResources          (func_0204335c)
 *     0x02099ec4 {0x04,1} slot  1  BeforeInitResources    (func_0204335c)
 *     0x02099ecc {0x24,1} slot  9  Render                 (func_0204322c)
 *
 * MSVC's member pointer (/vmg /vmm, the ROM's eight bytes) has no virtual
 * flag: Process calls word 0 with ecx = this + word 1 and nothing else. So
 * each record here is {face, 0}, and the face does what the ROM's virtual
 * branch does: read the vptr and call the slot, with the receiver in ECX
 * (the host tables are ROM-numbered and hold __thiscall-compatible words, the
 * convention port/tools/hostgen.py's VIRTUAL_CALL rows for these four
 * wrappers used when they inlined Process at the caller). The after() slots
 * take the code as their one stack argument and pop it (ret 4), as a
 * __thiscall member pointer call expects.
 *
 * Nothing in the ROM writes the records. Their C names are the ROM's, which is
 * what the wrappers' own `extern PMF data_02099e..;` lines ask for
 * (hal/player_bridges.cpp used to host three of them as zeroed words nothing
 * read), and like every hosted DS global they sit inside the .dsstate bracket
 * (port/tools/dsstate_guard.py). port/tools/pmf_guard.py checks every face
 * through its ECX row ^_data_02099e(..)$.
 */
#include "dsstate_seg.h"

namespace {

typedef int (__fastcall *PortVSlotFn)(void *, void *);
typedef void (__fastcall *PortVSlotAfterFn)(void *, void *, unsigned);

template <int SLOT>
int __fastcall port_process_vcall(void *self, void *dead_edx)
{
    (void)dead_edx;
    return ((PortVSlotFn)(*(void ***)self)[SLOT])(self, 0);
}

template <int SLOT>
void __fastcall port_process_vcall_after(void *self, void *dead_edx,
                                         unsigned code)
{
    (void)dead_edx;
    ((PortVSlotAfterFn)(*(void ***)self)[SLOT])(self, 0, code);
}

}  // namespace

extern "C" {

struct PortProcessCell { PortVSlotFn code; int adjust; };
struct PortProcessAfterCell { PortVSlotAfterFn code; int adjust; };

DSSTATE_BEGIN

PortProcessCell data_02099e74 = {port_process_vcall<10>, 0};
PortProcessCell data_02099e7c = {port_process_vcall<4>, 0};
PortProcessAfterCell data_02099e84 = {port_process_vcall_after<5>, 0};
PortProcessCell data_02099e8c = {port_process_vcall<3>, 0};
PortProcessAfterCell data_02099e94 = {port_process_vcall_after<2>, 0};
PortProcessAfterCell data_02099e9c = {port_process_vcall_after<11>, 0};
PortProcessCell data_02099ea4 = {port_process_vcall<6>, 0};
PortProcessCell data_02099eac = {port_process_vcall<7>, 0};
PortProcessAfterCell data_02099eb4 = {port_process_vcall_after<8>, 0};
PortProcessCell data_02099ebc = {port_process_vcall<0>, 0};
PortProcessCell data_02099ec4 = {port_process_vcall<1>, 0};
PortProcessCell data_02099ecc = {port_process_vcall<9>, 0};
DSSTATE_END

}  /* extern "C" */
