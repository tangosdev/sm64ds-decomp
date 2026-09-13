// smoke_player's copy of ONE ARM primitive, so it can carry rung R1's slice.
//
// Run link100, lane SND1. port/slice_snd1.txt seats src/func_0205ae64.c, the
// ROM's channel-7 registration. Its retry arm is
//
//     do { func_02059d8c(0x64); } while (!func_0205ba3c(7, 1));
//
// and func_02059d8c is `subs r0,r0,#4; bhs` -- hand-written ARM MSVC cannot
// assemble, transcribed as a no-op in hal/boot_hw.cpp with the reasoning at
// that line. hal/boot_hw.cpp rides SLICE214_SOURCES: walk_window and
// walk_window_hires only. smoke_player does not carry it, so without this file
// the reference would not resolve there.
//
// THE LOOP IS NEVER ENTERED in any of the three targets. hal/boot2_ipc.cpp
// claims channel 7 at power-on, which is what the DS's ARM7 has done before
// the ARM9's code runs at all, so func_0205ba3c(7, 1) is already true when
// func_0205ae64 first tests it and the body returns on the line above. This
// file exists to close the link, not to model a wait.
//
// EXCLUSIVE WITH hal/boot_hw.cpp, the same way hal/nitrofs_face.cpp is: a
// target that carried both would have func_02059d8c defined twice. It is on
// smoke_player alone, and it is invisible to port/tools/linkage.py's shadow
// count, which reads walk_window.map -- where the definition is
// hal/boot_hw.cpp's and has been for the life of that file.

extern "C" void func_02059d8c(int cycles) { (void)cycles; }
