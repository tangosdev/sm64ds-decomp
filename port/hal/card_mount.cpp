// The ROM's card mount table, hosted -- and the ROM's own LoadArchive linked
// on top of it.
//
// Run link100, lane CARDFS. This retires the refusal hal/scene_boot.cpp carried
// for four waves, which read:
//
//     The alternative -- linking the matched TU -- would need data_0208ecf4
//     hosted as raw ROM bytes with its DS string pointers, plus func_02018934
//     and the four-TU card-loader chain under it, to produce a mount the fs
//     seam then ignores. That is a fake, not a fix.
//
// The refusal's own reasoning is what answers it. src/LoadArchive.c asks one
// question -- "is archive N resident?" -- and answers it by reading one word:
//
//     struct Entry *e = &data_0208ecf4[idx];
//     if (e->field_0 == 0) {
//         e->field_4 = data_020a0ea0;
//         e->field_0 = func_02018934(e->field_c, e->field_10, e->field_4);
//     }
//     return e->field_0 != 0;
//
// On the DS that word is null until the card loader mounts the NARC. On the
// host every one of the thirteen archives is a file on disk that hal/fs.cpp
// already reads, keeps for the life of the process and never releases -- so
// "resident" is not a claim the port has to invent, it is a fact the port was
// already sitting on and lying about through a face. This file hosts the ROM's
// table, fills each entry's residency word with the archive that really is
// resident, and lets the ROM's own body do the reading. Nothing answers for it.
//
// ---- WHAT IS IN THE ENTRY, AND WHERE EACH WORD COMES FROM -------------------
//
// The ROM's entry is twenty bytes (0x0208ecf4 to the next dsd symbol
// 0x0208edf8 is 0x104 = 13 * 0x14), and src/func_020185c0.c and
// src/LoadArchive.c between them name every field:
//
//     +0x00  void *        the mounted archive object     runtime, starts 0
//     +0x04  void *        the heap it will be freed to   runtime, starts 0
//     +0x08  u16           first interior file id
//     +0x0a  u16           one past the last
//     +0x0c  const char *  three-character short name     DS rodata pointer
//     +0x10  const char *  the NARC's NitroFS path        DS rodata pointer
//
// +0x08 and +0x0a come from port_archive_map, which port/tools/romdata.py
// generates BY READING THESE EXACT HALFWORDS OUT OF THIS TABLE in the local
// arm9 image (romdata.py, "The archive-mount table at data_0208ecf4"). So the
// ids in the hosted table are the ROM's own ids, on the ROM's own path into the
// port, and not one number is typed here. +0x00 and +0x04 are the ROM's own
// initial values, which are zero: the table ships zeroed in .data and the card
// loader is what fills it.
//
// ---- THE TWO STRING POINTERS ARE HOSTED (run link100, lane CARDFS2) --------
//
// They were left NULL when this file was written, and the paragraph that stood
// here said why: they hold DS addresses in arm9 rodata, a mounted word holding
// a DS address must never be followed, and a null at least faults instead of
// jumping into whatever host memory happens to be mapped. It also named the
// mechanism that would fix it -- port/tools/romdata.py's NAMED list, which
// already carried two of their immediate neighbours (data_0208eb8c and
// data_0208eb9c, the "myFS_OpenFile"/"myFS_ReadFile" crash strings).
//
// That is what this lane did. Twenty-six NAMED rows now host the thirteen
// three-character short names (the contiguous four-byte run
// 0x0208eb58..0x0208eb8c) and the thirteen NitroFS paths (thirteen of the
// fourteen 0x14-byte rows from 0x0208ebac; 0x0208ec74 is the crash string
// src/func_02018dc4.c uses and is NOT one of them). All twenty-six are ROM
// bytes read out of extracted/arm9_dec.bin, and all twenty-six are
// relocation-free over their whole span.
//
// WHICH STRING BELONGS TO WHICH ENTRY IS NOT WRITTEN DOWN ANYWHERE HERE, and
// that matters, because the two orders disagree: entry 0's short name is the
// SECOND symbol in the name run ("ar0" at 0x0208eb5c, while 0x0208eb58 is
// "vs2"), and the paths are not in entry order either. romdata.py resolves
// each entry's +0x0c and +0x10 word BY ADDRESS out of the same table it
// already reads the id ranges from, and emits port_archive_name[13] and
// port_archive_path[13] pointing at the hosted rows. A pointer with no NAMED
// row is a hard error in the generator, not a quiet null.
//
// SO THE SENTENCE THIS FILE USED TO CARRY IN CAPITALS IS RETIRED: the mount
// branch no longer opens a null path. It is still not taken, and the reason is
// no longer these two words -- see THE MOUNT BRANCH below.
//
// ---- WHAT GOES INTO THE RESIDENCY WORD -------------------------------------
//
// Not the raw NARC image. src/func_0204eda4.c and src/func_0204ede8.c -- the
// two readers on the other side of the table -- read the mounted object at
// +0x54 (the BTAF chunk) and +0x58 (the file-image base), which is the shape
// src/func_0204ee40.c gives it:
//
//     func_0205cd34(self);                      /* zero the first 0x50 */
//     *(char **)((char *)self + 0x50) = p;      /* the NARC image */
//     *(char **)((char *)self + 0x54) = fatb;   /* the BTAF chunk */
//     *(char **)((char *)self + 0x58) = fimg+8; /* the file data base */
//
// So the object published here is 0x60 bytes, zeroed, carrying those same three
// words with those same three values, found by the same chunk walk over the
// same NARC bytes. Anything that reads +0x50, +0x54 or +0x58 gets the truth,
// which is what keeps this from being a landmine for the lane that eventually
// links func_020185c0 and func_020186c0.
//
// ONE HALF OF func_0204ee40 IS STILL NOT RUN, AND IT IS NO LONGER THE NAME
// THAT STOPS IT. After writing those three words the ROM registers the archive
// with NitroSDK (func_0205cc80, FS_RegisterArchiveName, the three characters at
// +0x0c) and loads its FAT/FNT spans as a MEMORY archive (func_0205cb68 with
// both proc arguments zero, which src/func_0205cb68.c resolves to the default
// pair func_0205d2dc / func_0205d2b0). That makes paths of the form
// "ar0:/dir/file" resolvable. The short name is hosted now, so the sentence
// that used to end this paragraph -- "the registration cannot be run without
// the short name that is not hosted" -- is retired. Two other things stand in
// its place and both are recorded rather than worked around:
//
//   NOTHING ON THIS LINK LINE OPENS SUCH A PATH. hal/fs_names.cpp's seam serves
//   the cartridge's own "rom" archive, and every archive-interior read on the
//   host resolves through the id range (port_fs_archive_fill), not through a
//   name. Registering thirteen archives to satisfy no caller is work with no
//   observable, which is the same trade this file already refuses elsewhere.
//
//   AND THE ORDER WOULD MATTER. src/func_0205cc80.c registers into
//   data_020a8048, and when that list head is EMPTY it also makes the new
//   archive the current directory (data_020a804c). This file publishes from
//   hal/fs.cpp's catalog load, which is the first file access in the process;
//   hal/fs_names.cpp registers "rom" from func_0205d89c's once-guard. If the
//   publish went first, archive 0 ("ar0") would become NitroSDK's current
//   directory and every relative open-by-name in the ROM's own walker would
//   resolve against the wrong FNT. A lane that takes the registration owes that
//   ordering an argument and a proof, not an assumption.
//
// ---- THE MOUNT BRANCH, AND WHY IT STILL DOES NOT RUN -----------------------
//
// src/LoadArchive.c takes its mount branch when +0x00 is zero, and this file
// keeps +0x00 non-zero so it never does. With the strings hosted, the reason is
// no longer a null path. It is three, and none of them is a missing string:
//
//   1. THE ARITIES. src/func_02018934.c calls func_02018d98(FSFile *, path),
//      and src/func_02018d98.c defines that function taking NO parameters and
//      passing none to func_0205d518 -- the ARM original rides r0 and r1
//      through. It calls _ZN4Heap9_AllocateEji(heap, size, -0x10) and uses the
//      result, while src/_ZN4Heap9_AllocateEji.cpp is the two-instruction
//      tail-call veneer at 0x0203c29c decompiled as a void() calling a void().
//      On the host both are cdecl: the callee reads the veneer own frame, not
//      the caller's, and the return value rides a register nothing promises.
//      These are the two ride-throughs port/slice_cardfs.txt named when it
//      seated them, and they are host-ABI faults, not wrong answers.
//
//   2. THE MEMORY MODEL. The ROM's mount is DEMAND driven and short-lived:
//      func_02018934 allocates the whole NARC plus 0x60 off data_020a0ea0, and
//      src/func_02018770.c unloads it again right after the read (that is what
//      data_0208eb54, the "which archive did I just mount" byte, is for). This
//      file's publish is eager and permanent, which is right for a host that
//      already holds all thirteen images for the life of the process -- and
//      exactly wrong to run the ROM's branch under, because thirteen mounts
//      that are never unloaded would put 1,035,956 bytes plus thirteen headers
//      into the game heap and leave them there.
//
//   3. What is NOT a reason, said so the next lane does not re-derive it: the
//      READS have a host source. "/ARCHIVE/arc0.narc" is a real row in the
//      cartridge's own name table and file id 105 in build/assets/files.tsv, so
//      func_02018d98's open-by-name would walk the ROM's FNT through
//      hal/fs_names.cpp and the bytes would come off disk like every other
//      file. The branch is blocked on the two arities and on the lifetime, not
//      on anything this port cannot see.
//
// ---- WHY THE PUBLISH IS EAGER ----------------------------------------------
//
// hal/fs.cpp loads an archive image the first time a file inside it is asked
// for. LoadArchive is asked BEFORE that -- Stage::InitResources mounts the
// level's archive and then loads out of it -- so a lazy publish would leave the
// residency word null at exactly the moment the ROM reads it. All thirteen are
// therefore loaded once, from hal/fs.cpp's own catalog load, through hal/fs.cpp's
// own loader and into hal/fs.cpp's own cache. The cost is measured, not
// estimated: 1,035,956 bytes for all thirteen NARCs, which is the number
// fs.cpp's "they are small" comment was already relying on, and the port
// already held every one of them for the life of the process once touched.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dsstate_seg.h"

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

/* port/tools/romdata.py's host-shaped copy of this same ROM table: the id
   range and the NARC path, read out of the image at 0x0208ecf4. */
struct port_arc_entry { u16 base, end; const char *narc; };
extern "C" struct port_arc_entry port_archive_map[13];

/* The same generator's answer for each entry's two DS string pointers: the
   hosted ROM bytes at the address that entry's +0x0c and +0x10 word holds. */
extern "C" const char *port_archive_name[13];
extern "C" const char *port_archive_path[13];

/* hal/fs.cpp: load archive i's whole image (cached, never freed) and hand back
   the bytes it already holds. Zero on a miss. */
extern "C" int port_fs_archive_get(int i, unsigned char **img, long *len);

/* hal/fs.cpp calls this through a pointer once its catalog is up, so the
   smoke targets that link fs.cpp without this file still link. */
extern "C" { extern void (*port_card_mount_publish_all)(void); }

/* ---- the two hosted DS globals ------------------------------------------ */

DSSTATE_BEGIN
extern "C" {

/* 0x0208ecf4, thirteen entries of 0x14 bytes: the whole 0x104-byte span to the
   next dsd symbol at 0x0208edf8. */
struct PortCardMount {
    void *f0;           /* mounted archive object; 0 means not mounted */
    void *f4;           /* the heap LoadArchive would free it back to */
    u16 f8;             /* first interior file id */
    u16 fa;             /* one past the last */
    const char *fc;     /* short name  -- see the banner: not hosted */
    const char *f10;    /* NARC path   -- see the banner: not hosted */
};
PortCardMount data_0208ecf4[13];

/* 0x0209d3b4, one bss word: the archive id src/func_02018598.c pairs with a
   file id to make the FSFileID that opens a loose file. Four bytes by the
   delta to data_0209d3b8, and its ROM value is zero because it is bss. */
int data_0209d3b4;

}  /* extern "C" */
DSSTATE_END

/* The mounted objects themselves are HOST storage, not DS storage: on the DS
   they are 0x60 bytes off the game heap, here they are a static array, and
   neither their address nor their contents is a DS value a save state has to
   roll back (they are written once and never again). They stay out of
   .dsstate for the same reason hal/fs.cpp's image cache does. */
static u8 g_mount_obj[13][0x60];
static int g_published;
static long g_published_bytes;
static int g_audit_registered;

/* The NARC chunk walk, byte for byte the one src/func_0204ee40.c makes:
   header at +0xc gives the offset of the first chunk and +0xe the count; each
   chunk is {u32 magic, u32 size}. BTAF/BTNF/GMIF are 'FATB'/'FNTB'/'GMIF' as
   little-endian words, which is how the ROM's own switch spells them. */
static int narc_chunks(u8 *a, long len, u8 **fatb_out, u8 **fimg_out)
{
    u8 *cur;
    u8 *fatb = 0, *fimg = 0;
    int i, n;

    if (len < 0x10 || memcmp(a, "NARC", 4) != 0)
        return 0;
    if (*(u16 *)(a + 4) != 0xfffe || *(u16 *)(a + 6) != 0x0100)
        return 0;

    cur = a + *(u16 *)(a + 0xc);
    n = *(u16 *)(a + 0xe);
    for (i = 0; i < n; ++i) {
        u32 magic;
        if (cur < a || cur + 8 > a + len)
            return 0;
        magic = *(u32 *)cur;
        if (magic == 0x46415442)        /* 'FATB' */
            fatb = cur;
        else if (magic == 0x46494d47)   /* 'GMIF' */
            fimg = cur;
        cur += *(int *)(cur + 4);
    }
    if (!fatb || !fimg)
        return 0;
    *fatb_out = fatb;
    *fimg_out = fimg + 8;
    return 1;
}

/* The witness described in the banner. The ROM's LoadArchive writes the heap
   word on its way into the mount branch and nothing here ever writes it, so a
   non-zero heap word is proof the branch ran -- which, while the two string
   pointers are null, would have faulted. Printed once at exit. */
extern "C" void port_card_mount_audit(void)
{
    int i, mounted = 0, romwrote = 0, named = 0;
    for (i = 0; i < 13; ++i) {
        if (data_0208ecf4[i].f0)
            mounted++;
        if (data_0208ecf4[i].f4)
            romwrote++;
        /* Both strings, and the short name really is three characters
           and the path really starts at the root: a row that lost its
           NAMED backing shows up as a short count here rather than as a
           fault the first time the mount branch reads it. */
        if (data_0208ecf4[i].fc && data_0208ecf4[i].f10 &&
            data_0208ecf4[i].fc[3] == 0 && data_0208ecf4[i].f10[0] == '/')
            named++;
    }
    fprintf(stderr,
            "[cardfs] table 0x0208ecf4: %d/13 resident (%ld bytes), "
            "%d/13 carry a ROM-written heap word, %d/13 carry both ROM "
            "strings (entry 0: %s %s)\n",
            mounted, g_published_bytes, romwrote, named,
            data_0208ecf4[0].fc ? data_0208ecf4[0].fc : "(null)",
            data_0208ecf4[0].f10 ? data_0208ecf4[0].f10 : "(null)");
    fflush(stderr);
}

extern "C" void port_card_mount_publish_impl(void)
{
    int i;
    /* The two informational lines are OFF by default: hal/tools/battery.py
       runs hundreds of processes and a per-process banner is noise in every
       one of their logs. SM64DS_CARDFS=1 turns them on for a proof run; the
       FATAL path below is never gated. */
    const char *say = getenv("SM64DS_CARDFS");
    const char *verbose = getenv("SM64DS_CARDFS_VERBOSE");

    if (g_published)
        return;
    g_published = 1;

    for (i = 0; i < 13; ++i) {
        u8 *img = 0, *fatb = 0, *fimg = 0;
        long len = 0;

        /* The ids are the ROM's, on romdata.py's path out of the image, and
           so are the two strings: same table, same generator, resolved by the
           address the entry itself holds. */
        data_0208ecf4[i].f8 = port_archive_map[i].base;
        data_0208ecf4[i].fa = port_archive_map[i].end;
        data_0208ecf4[i].fc = port_archive_name[i];
        data_0208ecf4[i].f10 = port_archive_path[i];

        if (!port_fs_archive_get(i, &img, &len) || !img) {
            fprintf(stderr,
                    "FATAL: cardfs: archive %d (%s) did not load; the ROM's "
                    "LoadArchive would take its mount branch and the mount "
                    "table's DS path pointers are not hosted\n",
                    i, port_archive_map[i].narc);
            abort();
        }
        if (!narc_chunks(img, len, &fatb, &fimg)) {
            fprintf(stderr, "FATAL: cardfs: %s is not a NARC this loader "
                    "recognises\n", port_archive_map[i].narc);
            abort();
        }

        memset(g_mount_obj[i], 0, sizeof g_mount_obj[i]);
        *(u8 **)(g_mount_obj[i] + 0x50) = img;
        *(u8 **)(g_mount_obj[i] + 0x54) = fatb;
        *(u8 **)(g_mount_obj[i] + 0x58) = fimg;
        data_0208ecf4[i].f0 = g_mount_obj[i];
        g_published_bytes += len;

        if (verbose)
            fprintf(stderr, "[cardfs] %2d %-20s ids %u..%u  %ld bytes\n",
                    i, port_archive_map[i].narc,
                    (unsigned)data_0208ecf4[i].f8,
                    (unsigned)data_0208ecf4[i].fa, len);
    }

    if (say) {
        fprintf(stderr, "[cardfs] published 13/13 archives (%ld bytes) into "
                "data_0208ecf4; the ROM's LoadArchive now answers from the "
                "table\n", g_published_bytes);
        fflush(stderr);
    }

    if (say && !g_audit_registered) {
        g_audit_registered = 1;
        atexit(port_card_mount_audit);
    }
}

/* Installed at static-init time; hal/fs.cpp calls it once its catalog is up. */
static struct PortCardMountInstall {
    PortCardMountInstall() { port_card_mount_publish_all = port_card_mount_publish_impl; }
} g_card_mount_install;
