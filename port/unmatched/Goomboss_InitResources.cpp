/* RETIRED HOST COPY of src/_ZN8Goomboss13InitResourcesEv.cpp (ov074 0x02121e98,
 * 0x404 bytes), vtable slot 0 of _ZTV8Goomboss. Nothing in this file is
 * compiled any more.
 *
 * It was the matched source with five statements changed, two declarations
 * corrected and one block moved, because MSVC refused the old src's bare
 * `return;` in an int body (C2561). That refusal is gone from the synced src,
 * which returns func_ov074_02122634's value. What was left was mechanical, and
 * run linkfull lane SMALLS2 moved it into two tools/hostgen.py rows keyed by
 * the TU's stem: DATA_C_LINKAGE gives the eleven file-scope ROM data externs C
 * linkage (this file had moved them inside its extern "C" block), and
 * OVERLAY_OBJECT_AT resolves the two level-overlay file objects the TU spells
 * func_021123f4 / func_021124ac through ov053's mount, port_ov053_at (this
 * file made that substitution by hand). func_01ffa344 needs nothing: its host
 * body in hal/cxx_aliases.cpp takes the one argument the ROM passes, so the
 * explicit second argument this file added is gone. The matched TU is built
 * from its build/host-src copy by the SMALLS2 block at the end of
 * port/CMakeLists.txt (port/slice_w31_smalls2_hostgen.txt), and its
 * port/faces_sync.txt row is a REVERSE row: slot 0's flat name is the
 * generated face into Goomboss::InitResources, the ROM's own code.
 *
 * One difference is the matched TU's to own: this copy built the ground probe
 * in a 0x4c-byte stand-in (struct RG) where the class is 0x50
 * (include/dBgCh_Gnd.h asserts it), so dBgCh_Gnd's +0x4c word fell on the next
 * local; the matched TU constructs the real dBgCh_Gnd.
 *
 * The file stays, empty, because port/CMakeLists.txt's ov074 block still names
 * it; deleting it is a one-line change there.
 */
