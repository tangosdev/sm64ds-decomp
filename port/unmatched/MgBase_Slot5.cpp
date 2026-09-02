/* HOST COPY of dScMgBase_c slot 5, func_ov004_020b0840. Run mg5, lane BASESET.
 *
 * THE SECOND OF THE TWO ARGUMENT RIDE-THROUGHS port/mg_fanout_costs.txt section
 * 6 named and left. Section 6b is slot 7 and was taken by lane MG1, which is
 * port/unmatched/MgBase_Slot7.cpp. Section 6c is this one, and both
 * mg_fanout_costs and port/hal/scene_mg.cpp's header say it wants "a
 * PORT_HOST_ABI host copy in port/unmatched/ that places the argument, leaving
 * src/ and the byte gate alone". That is exactly and only what this file is.
 *
 * ---- THE DEFECT ----------------------------------------------------------
 *
 * src/func_ov004_020b0840.c:12 declares
 *
 *     extern void func_0203cbc0(void);
 *
 * and calls it with no argument at line 28, inside
 *
 *     if (data_ov004_020beb60[0] != 0) {
 *       func_0203cbc0();
 *       data_ov004_020beb60[0] = 0;
 *     }
 *
 * The ROM has the pointer being freed live in r0 at that call. Disassembled from
 * extracted/overlays/overlay_0004.bin at base 0x020ad660:
 *
 *     020b0888  ldr   r0, [pc, #0x5c]     ; r0 = 0x020beb60, from the literal
 *                                         ;      pool word at 0x020b08ec
 *     020b088c  ldr   r0, [r0]            ; r0 = data_ov004_020beb60[0]
 *     020b0890  cmp   r0, #0
 *     020b0894  beq   0x020b08a8          ; the null guard
 *     020b0898  bl    0x0203cbc0          ; r0 STILL holds the pointer
 *     020b089c  ldr   r0, [pc, #0x48]
 *     020b08a0  mov   r1, #0
 *     020b08a4  str   r1, [r0]            ; data_ov004_020beb60[0] = 0
 *
 * Nothing between the load and the branch touches r0, so the call is a plain
 * one-argument call that mwccarm emitted no instruction for. On the host it is a
 * dropped pointer, and the callee is operator delete: the port's own body is
 *
 *     void func_0203cbc0(void *p) { _ZdlPv(p); }
 *
 * in port/unmatched/func_02073244_hostcopy.c. So the src form hands the heap
 * whatever happens to be in the caller's stack slot and frees it. Seven other
 * src TUs declare the same function WITH a pointer parameter, which is what
 * makes this a defect rather than a convention: src/func_ov006_020cc45c and
 * port/unmatched/Player_CleanupResources.cpp both pass one.
 *
 * ---- WHY IT IS REAL AND WHEN IT FIRES -------------------------------------
 *
 * data_ov004_020beb60 IS ALLOCATED ON EVERY MINIGAME BOOT. Slot 1,
 * BeforeInitResources, does it: src/func_ov004_020b0930.cpp:42 and its host copy
 * port/unmatched/MgBase_Slot1.cpp:62 both read
 *
 *     if (data_ov004_020beb60 == 0)
 *         data_ov004_020beb60 = _ZN6Memory13operator_new2Ej(0x4000);
 *
 * so the guard above passes from the first minigame boot onward and the free is
 * taken on every AfterCleanupResources(2). This is a live wild free on the exit
 * path of every minigame, not a latent one.
 *
 * IT IS NOT NEWLY REACHABLE, AND THIS FILE SAYS SO RATHER THAN CLAIMING THE
 * CREDIT. mg_fanout_costs section 6 groups it with slot 7 as "cannot be
 * exercised until section 4's dispatch exists", and that grouping is right about
 * slot 7 and wrong about this one: slot 5 is a vtable slot the scene machinery
 * dispatches at teardown, and nothing about the state setter gates it. What the
 * setter changes is how often a real session REACHES a teardown, because a class
 * whose message states never advanced tended to wedge before it exited. The
 * measurement on the unattended runs is a flat zero either way: every minigame
 * census in this lane reports `cleanup 0` for slot 5, so no battery run enters
 * this body at all. It is fixed here because it is a wild free with a known
 * remedy and an agreed filename, not because a number moved.
 *
 * ---- WHAT CHANGED --------------------------------------------------------
 *
 * ONE CALL. `func_0203cbc0()` became `func_0203cbc0(data_ov004_020beb60[0])`,
 * which is the value the ROM has in r0 at that instruction. The local
 * declaration changed to match, from `(void)` to `(void *)`, and it is a LOCAL
 * declaration in this file: include/ declares func_0203cbc0 nowhere, which is
 * why the src TU could carry a wrong one for as long as it did.
 *
 * Every other statement, constant, offset, call and branch is src's, in order.
 * src/func_ov004_020b0840.c is EXCLUDED from port/slice_mg1.txt and
 * port/slice_smb.txt, which are the two slices that carried it, and both say so
 * where the line was.
 *
 * The body was ruled REAL_DECOMP against the ROM before the fan-out seated it;
 * port/tools/inferred_stub_adjudicated.txt carries the row. Nothing here
 * re-opens that ruling, and the literal pool the disassembly above quotes
 * (0x0209b308, 0x0209d4a8, 0x020beb74, 0x020beb60 at 0x020b08e0..0x020b08ec) is
 * the four globals src names, in the src's own order of use.
 */
#include "types.h"
#include "decl_Scene.h"
#include "decl_common.h"
#include "dScMgBase_c.h"

extern "C" {

/* THE ONE CHANGED DECLARATION. src spells this `(void)`; the ROM passes the
   pointer in r0 and port/unmatched/func_02073244_hostcopy.c defines it as
   `void func_0203cbc0(void *p) { _ZdlPv(p); }`. */
void func_0203cbc0(void *p);

/* src's other local declarations, unchanged. */
void func_ov004_020ad90c(void);
void FreeGfxSlotsById(int arg);
void func_02012e1c(void);
void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int x);
extern int data_0209b308[];
extern int data_0209d4a8[];
extern int data_ov004_020beb60[];

// PORT_HOST_ABI: src drops the pointer argument to func_0203cbc0 (operator delete) that the ROM rides through r0; MSVC __cdecl would free stack garbage, so the host copy passes it
void func_ov004_020b0840(char *c, int arg)
{
    struct dScMgBase_c *self = (struct dScMgBase_c *)(void *)c;
    if (arg == 2) {
        if (data_0209b308[4] == 0)
            func_ov004_020ad90c();
        data_0209d4a8[0] = 0;
        data_ov004_020beb74[1] = 0;
        FreeGfxSlotsById(0x1d);
        if (data_ov004_020beb60[0] != 0) {
            /* the ONE change: the pointer the ROM has in r0 */
            func_0203cbc0((void *)data_ov004_020beb60[0]);
            data_ov004_020beb60[0] = 0;
        }
        if (self->unk_4628 != 0) {
            func_02012e1c();
            _ZN5Sound22StopLoadedMusic_Layer1Ej(1);
        }
        func_ov004_020b2c84();
    }
    _ZN5Scene21AfterCleanupResourcesEj(c, arg);
}

}  /* extern "C" */
