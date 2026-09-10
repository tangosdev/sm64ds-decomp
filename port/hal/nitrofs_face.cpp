// The faces for the ONE target that cannot carry the ROM's own bodies.
//
// hal/fs_names.cpp used to define func_0205d96c itself. Run link100 lane
// NITROFS seated the matched TU src/func_0205d96c.c instead (port/
// slice_nitrofs.txt), which the header of hal/nitrofs_boot.cpp explains. That
// seat cannot go to every target, and the reason is a link line rather than a
// ruling:
//
//   src/func_0205d96c.c takes the address of func_0205da94, the archive
//   command proc, and the matched proc reaches the ROM's cartridge lock --
//   func_0205ffb0 / func_0205ff90 and the six OS-lock TUs under them. Those
//   bottom out on four primitives hal/boot_hw.cpp transcribes (func_02057198,
//   func_0205a74c, func_02059d8c, IRQ::DisableAll / IRQ::RestoreAll), and
//   hal/boot_hw.cpp rides SLICE214_SOURCES, which goes to walk_window and
//   walk_window_hires ONLY. smoke_player carries hal/fs_names.cpp but not
//   SLICE214_SOURCES, so on that target the whole chain is undefined.
//
// So smoke_player keeps the face, on its own, in this file, and the two window
// targets carry the cartridge's own function. port_nitrofs_fs_init is still
// hal/fs_names.cpp's -- only the ROM-NAMED wrapper moved, because a ROM name
// defined on a target that also links the matched TU is a duplicate symbol.
//
// PORT_HOST_ABI: src reads the DS cartridge-header mirror at 0x027FFE40 and
// 0x027FFE48 and calls func_02057020, which reads 0x027FFFB0. On this target
// the same four words are taken from the cartridge by tools/asset_catalog.py,
// and then the ROM's own registration calls run verbatim. The pages ARE
// mapped (ntr/io.cpp kRegions) and hal/nitrofs_boot.cpp writes them; what this
// target lacks is the lock family behind the archive proc, not the mirror.
extern "C" void port_nitrofs_fs_init(void *dma);

extern "C" void func_0205d96c(void *dma) { port_nitrofs_fs_init(dma); }


// ---------------------------------------------------------------------------
// THE OVERLAY-UNLOAD FACE, same target and the same kind of reason (run
// link100, lane LOADOV).
//
// src/func_02017e94.c is the ROM's own overlay unload: it scans
// data_0209d3c4[12] for an entry whose first word is the id, returns when
// there is none, and otherwise drops the record and invalidates the code
// region. On this host the id is a host address (hal/scene_boot.cpp's
// overlay_NN block says why) and nothing ever fills that table, so the ROM's
// own early return is the arm every call takes. port/slice_loadov.txt links
// the matched TU for that reason -- but only on walk_window and
// walk_window_hires, because the TU still REFERENCES the rest of the chain
// (func_0205dc0c, func_02018c00, func_0203d7b8, func_02017fd0 and the
// twenty-odd TUs under them), that chain is port/slice_gate214.txt's, and it
// bottoms out on hal/boot_hw.cpp's ARM primitives. smoke_player carries
// neither slice_gate214 nor hal/boot_hw.cpp, so on that target the whole chain
// is undefined and the face below is what it links.
//
// A ROM name defined on a target that also links the matched TU is a duplicate
// symbol, which is why this face lives here and not in hal/scene_boot.cpp: that
// file goes to all three targets.
//
// PORT_HOST_ABI: the chain under the ROM's own body (func_0205dc0c,
// func_02018c00 and the OS-lock and assert TUs below them) is
// port/slice_gate214.txt's and rides hal/boot_hw.cpp, and this target carries
// neither. The observable is nothing: with no overlay resident the ROM's body
// returns without writing.
extern "C" void func_02017e94(int) {}
