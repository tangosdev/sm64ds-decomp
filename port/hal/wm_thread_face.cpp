// THE TWO TARGETS THAT DO NOT TAKE RUNG W6, AND WHAT THEY KEEP INSTEAD.
// Run link100, lane WM5.
//
// hal/wm_thread.cpp and port/slice_wm5.txt go to walk_window and
// walk_window_hires. They do NOT go to smoke_player, and that is forced rather
// than chosen: src/func_02042254.c (the worker body) and src/func_020424c0.c
// (the header-region reads) both call func_0205d5e8, which is on
// port/slice_gate214.txt, and SLICE214_SOURCES goes to walk_window and
// walk_window_hires only -- smoke_player has never carried the boot spine.
// Measured, not assumed: the first build of this rung linked both of those
// clean and failed smoke_player on exactly that one unresolved external.
//
// tests/mp_comms_seam.cpp does not take it either, for the reason rungs W1, W2
// and W4 already record: it links hal/comms_seam.cpp without the WM command
// layer, without a heap and without the host ARM7.
//
// So this file is what those two link instead, and it is a second new file for
// the same reason lane NITROFS needed one ("nitrofs_face.cpp is a second new
// file because smoke_player carries no boot_hw.cpp"): the retired bodies used
// to live in hal/comms_seam.cpp, and hal/comms_seam.cpp is compiled ONCE for
// all three big targets (SLICE_COMMS_SOURCES rides the sharing block), so a
// per-target define cannot keep a body in smoke_player and drop it from
// walk_window. A separate file on a separate target list can.
//
// FOUR THINGS, and nothing else:
//
//   func_02040c34    the stand-in hal/comms_seam.cpp carried until this rung,
//                    moved here verbatim with its reasoning. src/func_0203ea5c.c
//                    and hal/comms_conductor_wide.cpp both call it.
//   data_020a2400    the 0x1bc0 of DS BSS from the pool base to data_020a3fc0.
//                    src/func_02040bb0.c, src/func_02040f30.c and
//                    src/func_02041a00.c all name it and all three are already
//                    on smoke_player, so it has to resolve there. It stands
//                    alone at ".dsstate$ywmd13" exactly where hal/wm_arm7.cpp
//                    had it, because on these two targets nothing derives the
//                    same base from data_020a1fc0 and so nothing needs a band.
//   data_020a3fc0    the 0xB80 of DS BSS the two status-word accessors own.
//                    src/func_02040a5c.c and src/func_02040a84.c are on
//                    port/slice_gate221.txt, which does go to smoke_player, so
//                    the name has to resolve there. In walk_window it is the
//                    tail of hal/wm_thread.cpp's contiguous band; here it
//                    stands alone at ".dsstate$ywm" exactly as it did before,
//                    because without src/func_02040c34.c nothing on these two
//                    targets reaches it by pointer arithmetic and the band's
//                    head is not linked to sit behind.
//   the two census entry points, as no-ops. hal/boot2_ipc.cpp calls them from
//                    the host ARM7's turn and from its exit report, and that
//                    file goes to all three big targets.
// ---------------------------------------------------------------------------

extern "C" {

// src/func_02040c34.c: starts the DS's wireless THREAD with two callbacks,
// which are the ROM's own src/func_0203f644.c and src/func_0203f604.c. A host
// transport has no thread to start -- it is polled from the seam's own pump,
// which EXISTS as of run mg16 lane MP3: hal/comms_conductor.cpp installs it on
// hal/os_thread.h's hook and the ROM's own wait sleeps through it, so poll()
// runs once per turn of src/func_0203ea5c.c's real wait loop. So this records
// the request and returns. The callbacks are deliberately NOT invoked: on the
// DS they run in the wireless thread's context and mutate the same state the
// poll path does, so calling them from here would double-apply it. A transport
// that wants them driven does it from poll().
// PORT_HOST_ABI: hosted WM/radio seam face; the ROM body starts the DS wireless thread in the NITRO WM SDK over arm7.bin, and this target does not link port/slice_wm5.txt (see the banner); the host is polled from the seam pump instead.
void func_02040c34(int role, int b, void *cb_recv, void *cb_send, int e) {
    (void)role; (void)b; (void)cb_recv; (void)cb_send; (void)e;
}

// The pool base, sized by ROM span (data_020a3fc0 - data_020a2400). Same
// section name and same align as hal/wm_arm7.cpp's ".dsstate$ywmd13" carried
// until this rung, so on the two targets that keep it nothing about the
// save-state layout changed at all.
#pragma section(".dsstate$ywmd13", read, write)
__declspec(allocate(".dsstate$ywmd13")) __declspec(align(4))
unsigned char data_020a2400[0x1bc0] = {0};

// The status-word pair's storage, sized by ROM span (0x020a4b40 - 0x020a3fc0).
// See hal/comms_seam.cpp's retired banner for the sweep that identified its
// readers and hal/wm_thread.cpp for what rung W6 found that the sweep could
// not see.
#pragma section(".dsstate$ywm", read, write)
__declspec(allocate(".dsstate$ywm")) __declspec(align(4))
unsigned char data_020a3fc0[0xB80] = {0};

// The census hal/boot2_ipc.cpp drives. On these two targets there is no
// wireless worker thread to census, and saying so costs nothing.
void port_wm5_worker_census(void) {}
void port_wm5_report(void) {}

}  // extern "C"
