/* PORT_HOST_ABI. Force-included ahead of src/minigames/d_s_mg_base.cpp so the
 * matched dScMgBase_c::AfterCleanupResources frees the block the cartridge
 * frees instead of a word of its caller's stack frame. Nothing in src/ or
 * include/ changes. Run linkfull, lane PSYCHE1.
 *
 * ---- THE DEFECT, AND WHY unmatched/MgBase_Slot5.cpp DID NOT COVER IT ------
 *
 * The TU declares `extern void func_0203cbc0(void);` and calls it with no
 * argument inside `if (data_ov004_020beb60 != 0)`. The cartridge has the
 * pointer being freed in r0 at that call (0x020b0888 loads it, 0x020b0890
 * tests it, 0x020b0898 branches with r0 untouched), so mwccarm emitted no
 * argument and the byte gate is satisfied. Under x86 cdecl the callee, which
 * is operator delete (port/unmatched/func_02073244_hostcopy.c), reads whatever
 * the caller's frame left in that slot.
 *
 * unmatched/MgBase_Slot5.cpp is the host copy written for exactly this, and the
 * minigame faces that name the flat symbol _ZN11dScMgBase_c21AfterCleanupResourcesEj
 * still reach it. But the 09-13 main -> port sync put the merged TU
 * src/minigames/d_s_mg_base.cpp back on port/slice_mg1.txt, so MSVC's own
 * member ?AfterCleanupResources@dScMgBase_c@@UAEXI@Z is in the link too, and
 * every derived class whose override ends in the qualified base call
 * `dScMgBase_c::AfterCleanupResources(vfSuccess)` binds to it: the Single3D
 * family (src/minigames/d_s_mg_single3_d_base.cpp:273), Amida, Smartball and
 * Luigi. Measured on the INT48 build under cdb, Psyche Out (scene 389) under a
 * scripted stylus: data_ov004_020beb60 = 0x3011c44c at the call and the word
 * handed to operator delete = 0x00000001, which faults at 0xfffffff4 inside
 * MemoryNode::Target's constructor on the way to the allocator.
 *
 * ---- THE FIX ------------------------------------------------------------
 *
 * One name, for this translation unit only. The declaration and the one call
 * become port_mg_base_slot5_free_r0, a seam in unmatched/MgBase_Slot5.cpp that
 * frees data_ov004_020beb60[0]: the value the cartridge has in r0 at 0x020b0898,
 * read from the same global in the same place. port/CMakeLists.txt checks the
 * TU still spells exactly one such declaration and one such call, and fails
 * the configure if not, so this cannot go quietly stale.
 *
 * The house route for a ride-through is a REG_RIDE_ARG row in
 * port/tools/hostgen.py (its "Player" row is the same shape: a declaration
 * widened to take the pointer and the one call given it), with this TU moved
 * onto a hostgen list. That file was another lane's the night this was
 * written. Such a row retires this header, its CMake block and the seam.
 */
#ifndef PORT_MGBASE_SLOT5_HOSTABI_H
#define PORT_MGBASE_SLOT5_HOSTABI_H
#define func_0203cbc0 port_mg_base_slot5_free_r0
#endif
