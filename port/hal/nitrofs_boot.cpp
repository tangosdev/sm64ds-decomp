// The DS cartridge-header mirror, written -- and the three faces the ROM's own
// FS_Init needs on a host that has no game card in a slot.
//
// ---- WHY THIS FILE EXISTS --------------------------------------------------
//
// src/func_0205d96c.c is NitroSDK's FS_Init. It reads four words out of the
// cartridge header's mirror in main RAM:
//
//     *(int *)0x027FFE40   FNT offset      cartridge header +0x40
//     *(int *)0x027FFE44   FNT size        cartridge header +0x44
//     *(int *)0x027FFE48   FAT offset      cartridge header +0x48
//     *(int *)0x027FFE4C   FAT size        cartridge header +0x4c
//
// and returns early -- registering the archive but never loading it -- if
// either offset reads 0 or -1. Until this file, port/hal/fs_names.cpp said
// those pages were unmapped and stood a host body in for the whole function.
//
// THE PAGES WERE NEVER UNMAPPED. port/ntr/include/ntr/mmio.h:36 declares
// SHARED_BASE 0x027ff000 with SHARED_SIZE 0x1000, port/ntr/io.cpp:341-348
// lists it in kRegions with fatal = true (the process refuses to start without
// it), and port/hal/comms_seam.cpp:78 says so in its own words. All three
// addresses FS_Init reads are inside that region, and port/hal/os_lockid.cpp
// has been writing 0x027fffb0 -- 0x70 bytes further up the SAME page -- since
// lane BOOT. The mirror was mapped and zeroed and nothing ever wrote it, so
// the ROM's own body would have read 0 and returned. That is the whole of what
// was in the way, and this file is the four stores that clear it.
//
// The region comes up in ntr/io.cpp's TLS callback (.CRT$XLC), which the
// loader runs at DLL_PROCESS_ATTACH -- before the CRT and before every static
// initialiser -- so a seed written from a static initialiser is writing to
// memory the process already holds. io.cpp:1006-1041 is the mechanism.
//
// ---- WHERE THE FOUR WORDS COME FROM, AND WHY NOT FROM A CONSTANT -----------
//
// tools/asset_catalog.py:194-244 unpacks them out of the cartridge header at
// ROM_HEADER_FNT (0x40) and ROM_HEADER_FAT (0x48) with one struct.unpack_from,
// writes the two tables they point at to build/assets/nitrofs_{fnt,fat}.bin
// and the four values to build/assets/nitrofs.tsv. hal/fs_names.cpp already
// reads that file, refuses loudly when it is missing, and serves the ROM's own
// FNT and FAT walker out of the two blobs. So the words are taken from the one
// reader the port already has (port_nitrofs_header_words), not re-parsed here
// and not typed into a source file: change the cartridge, regenerate the
// catalog, and the mirror follows without an edit anywhere.
//
// ---- THE THREE FACES, AND WHAT EACH ONE NAMES ------------------------------
//
// Every one of these is a per-source COMPILE_DEFINITIONS rename in
// port/CMakeLists.txt, the shape lane R2ABC established for func_02060890 and
// lane BOOTR1 for main: the matched src TU stays exactly where it is,
// unenrolled and unclaimed, and the port says in ONE place that the edge is
// not ROM code yet. A host body under the ROM's own name would instead be a
// new SHADOW row in port/tools/linkage.py's replacement queue -- work invented
// by this change rather than work it did.
//
//   func_02060890  -> port_nitrofs_card_bringup      (in src/func_0205d96c.c)
//   func_020570c0  -> port_rom_exmemcnt_card_grant   (in src/func_020570f0.c)
//   func_020570d8  -> port_rom_exmemcnt_card_release (in src/func_0205710c.c)
//
// ---------------------------------------------------------------------------
#include <cstdio>
#include <cstdlib>

extern "C" {

void port_nitrofs_header_words(unsigned *fnt_off, unsigned *fnt_size,
                               unsigned *fat_off, unsigned *fat_size);
extern int data_020a8074[];

// ---- THE SEED --------------------------------------------------------------
// Four stores, at the four addresses src/func_0205d96c.c reads, from the four
// values tools/asset_catalog.py took out of the cartridge header. Called from
// hal/fs_names.cpp's static initialiser immediately before the ROM's own
// once-guard func_0205d89c, so the ordering is inside one translation unit and
// does not depend on static-initialiser order between files.
void port_nitrofs_header_mirror_seed(void)
{
    static int done;
    volatile unsigned *fnt = (volatile unsigned *)0x027ffe40u;
    volatile unsigned *fat = (volatile unsigned *)0x027ffe48u;
    unsigned fo = 0, fs = 0, ao = 0, as = 0;

    if (done)
        return;
    done = 1;

    port_nitrofs_header_words(&fo, &fs, &ao, &as);
    fnt[0] = fo;
    fnt[1] = fs;
    fat[0] = ao;
    fat[1] = as;

    std::fprintf(stderr,
        "[nitrofs] cartridge-header mirror seeded: 0x027ffe40 fnt %#010x+%#010x, "
        "0x027ffe48 fat %#010x+%#010x (header +0x40..+0x4c, out of "
        "build/assets/nitrofs.tsv via tools/asset_catalog.py)\n",
        fnt[0], fnt[1], fat[0], fat[1]);
}

// ---- THE READ-BACK ---------------------------------------------------------
// FS_Init hands the four words to func_0205cb68, which lays them into the
// archive record data_020a8074 (src/func_0205cb68.c: +0x20 and +0x30 the FAT
// offset, +0x24 the FAT size, +0x28 and +0x34 the FNT offset, +0x2c the FNT
// size, +0x3c and +0x44 the read function, +0x40 the write function). Reading
// them back out of the record is how this port proves the ROM's own body read
// the mirror rather than that the mirror merely holds the right bytes: nothing
// but func_0205cb68 writes those offsets.
//
// It REFUSES rather than warns, for hal/fs_names.cpp's reason: every target
// carrying this seat calls FS_Init from a static initialiser, so a disagreement
// here means the archive is misconfigured before main and every open-by-name
// after it would fail somewhere else entirely.
void port_nitrofs_boot_report(void)
{
    const volatile unsigned *fnt = (const volatile unsigned *)0x027ffe40u;
    const volatile unsigned *fat = (const volatile unsigned *)0x027ffe48u;
    unsigned rec_fat = (unsigned)data_020a8074[8];    /* +0x20 */
    unsigned rec_fats = (unsigned)data_020a8074[9];   /* +0x24 */
    unsigned rec_fnt = (unsigned)data_020a8074[10];   /* +0x28 */
    unsigned rec_fnts = (unsigned)data_020a8074[11];  /* +0x2c */

    std::fprintf(stderr,
        "[nitrofs] the ROM's FS_Init read the mirror into data_020a8074: "
        "+0x20 fat %#010x  +0x24 fat_size %#010x  +0x28 fnt %#010x  "
        "+0x2c fnt_size %#010x  +0x3c read %#010x\n",
        rec_fat, rec_fats, rec_fnt, rec_fnts,
        (unsigned)data_020a8074[15]);

    if (rec_fat != fat[0] || rec_fats != fat[1] ||
        rec_fnt != fnt[0] || rec_fnts != fnt[1]) {
        std::fprintf(stderr,
            "FATAL: the archive record does not carry the mirrored header "
            "words -- FS_Init did not run, or ran against a different mirror\n");
        std::fflush(stderr);
        std::exit(2);
    }
}

// ---- FACE: the card driver's own init, as FS_Init reaches it ---------------
// PORT_HOST_ABI: src/func_0205d96c.c's fifth statement is func_02060890(), the
// card driver bring-up. The port does NOT run it from here, and the reason is
// ordering rather than hardware:
//
//   * The card driver's work object data_020a8180 is seeded by
//     port/ntr/backup.cpp's PortBackupFill, which is a STATIC INITIALISER in
//     another translation unit. The port's FS_Init runs from hal/fs_names.cpp's
//     static initialiser. C++ leaves the order between two translation units'
//     initialisers unspecified, so a bring-up called from here would sometimes
//     read a zeroed work object, see its "already initialised" guard word at
//     +0x34 clear, and run the whole ROM body -- including func_0206002c(),
//     which CREATES THE CARD THREAD -- before main and before the seeder.
//   * The bring-up the ROM's boot order actually wants is already seated:
//     func_02042f68's arm 1 (hal/boot_arms.cpp's port_rom_card_bringup, lane
//     R2ABC), which runs inside main and prints its own [rom-a054] line every
//     boot saying the ROM's guard returns because the driver is up.
//   * Behind func_02060890 the ROM has two statements no host can link at all:
//     `data_020a8780 = func_02060a64` (the FLASH read path, hosted nowhere and
//     reached by nothing in this game) and func_02061138() -> func_020610fc, a
//     hand-asm deliberate `b self`. Both are R2ABC's word for word.
//
// So this is a no-op that names the floor, not a second copy of a body that
// already exists twenty lines away in another file.
void port_nitrofs_card_bringup(void)
{
}

// ---- FACES: EXMEMCNT bit 11, the card's ARM7/ARM9 ownership bit ------------
// PORT_HOST_ABI: src/func_020570c0.c and src/func_020570d8.c are two
// instructions each --
//
//     func_020570c0:  *(u16 *)G |= 0x0800;
//     func_020570d8:  *(u16 *)G &= ~0x0800;
//
// -- and `G` is the decomp's GENERIC PLACEHOLDER for an absolute address that
// is not in config/arm9/relocs.txt because it is an I/O register. The ROM's own
// literal pools say which one, and port/hal/boot_os.cpp:180-190 already carries
// the table read out of them:
//
//     func_020570c0  pool 0x020570d4  0x04000204  EXMEMCNT, |= 0x0800
//     func_020570d8  pool 0x020570ec  0x04000204  EXMEMCNT, &= ~0x0800
//
// EXMEMCNT bit 11 is the NDS slot's access right: clear gives the cartridge to
// the ARM9, set gives it back to the ARM7. That is exactly what the two
// callers do with them -- src/func_0205710c.c passes the CLEARING one as the
// cleanup that runs when the cartridge lock is taken, src/func_020570f0.c the
// SETTING one for when it is released.
//
// LINKING THE TWO MATCHED TUs WOULD BE WRONG, not merely unhelpful. `G`
// resolves in this binary: hal/heap_vtable.cpp defines it as the default-heap
// word. So the ROM's own bodies, linked as they stand, would OR and AND bit 11
// of a heap pointer four times per boot. boot_os.cpp refuses func_02058ec8 and
// func_02059594 for that same store, and this port's rule against non-faithful
// fixes cuts the other way too: a matched TU that writes the wrong address is
// not a link, it is a bug with a good name.
//
// The bodies below write the register the ROM's literal pool names. They are
// faces, not seats: the two matched TUs stay unenrolled and unclaimed, so
// nothing here moves linkage.py's count, and the day the decomp spells
// REG_EXMEMCNT instead of `G` both faces retire into two real TUs.
//
// ntr maps 0x04000000 + 0x2000 (ntr/io.cpp kRegions, fatal), so the store lands
// in held memory. Nothing on the host reads bit 11 back -- there is no ARM7
// arbitrating a cartridge slot -- which is precisely why running the ROM's own
// lock arm is safe here.
void port_rom_exmemcnt_card_grant(void)
{
    *(volatile unsigned short *)0x04000204u |= 0x0800u;
}

void port_rom_exmemcnt_card_release(void)
{
    *(volatile unsigned short *)0x04000204u &= (unsigned short)~0x0800u;
}

}  // extern "C"
