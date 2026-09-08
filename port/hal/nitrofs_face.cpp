// The FS_Init face, for the ONE target that cannot carry the ROM's own body.
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
