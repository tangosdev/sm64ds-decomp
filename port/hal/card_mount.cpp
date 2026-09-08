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
// ---- THE TWO STRING POINTERS ARE NOT HOSTED, AND THAT IS THE HONEST GAP -----
//
// +0x0c and +0x10 hold DS addresses in arm9 rodata (0x0208eb58..0x0208ecc4;
// entry 0 is 'ar0' at 0x0208eb5c and '/ARCHIVE/arc0.narc' at 0x0208ecb0). They
// are the "relocated words carry DS addresses" class, and the port's rule for
// that class is absolute: a mounted word holding a DS address must never be
// followed. They are left NULL here rather than mounted raw, so a follow is a
// null dereference and not a jump into host memory that happens to be mapped.
//
// THE PORT COULD HOST THEM AND THIS LANE COULD NOT. The strings are twenty-six
// arm9 .data symbols with dsd names of their own (data_0208eb58, data_0208eb5c,
// ... data_0208eb88 for the short names; data_0208ebac .. data_0208ecb0 for the
// paths), and port/tools/romdata.py's NAMED list is exactly the mechanism that
// hosts symbols like them -- two of their immediate neighbours, data_0208eb8c
// and data_0208eb9c, are already rows in it. Adding twenty-six more rows is a
// romdata.py change and romdata.py is not this lane's file, so the words stay
// null and the consequence is written down instead of being found later:
//
//     WHILE +0x0c AND +0x10 ARE NULL, THE ROM'S MOUNT BRANCH MUST NOT RUN.
//     If LoadArchive is ever called for an entry whose +0x00 is still zero it
//     will call func_02018934(NULL, NULL, heap), whose first act is to open a
//     null path through the NitroFS name walker. That is a fault, not a quiet
//     wrong answer, and port_card_mount_audit below is what proves it never
//     happens: the ROM writes +0x04 on its way into that branch and this file
//     never writes +0x04, so a non-zero heap word anywhere in the table is a
//     mechanical witness that the branch was taken.
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
// ONE HALF OF func_0204ee40 IS NOT RUN AND IT IS THE HALF THAT NEEDS THE NAME.
// After writing those three words the ROM registers the archive with NitroSDK
// (func_0205cc80, FS_RegisterArchiveName, three characters from +0x0c) and
// loads its FAT/FNT spans (func_0205cb68). That makes paths of the form
// "ar0:/dir/file" resolvable. Nothing on this link line opens such a path --
// hal/fs_names.cpp's seam serves the cartridge's own "rom" archive and every
// archive-interior read on the host goes through the id range, not the name --
// and the registration cannot be run without the short name that is not hosted.
// So it is not run, and it is not faked either: the object simply is not in
// NitroSDK's archive list, and this paragraph is where that is recorded.
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
    int i, mounted = 0, romwrote = 0;
    for (i = 0; i < 13; ++i) {
        if (data_0208ecf4[i].f0)
            mounted++;
        if (data_0208ecf4[i].f4)
            romwrote++;
    }
    fprintf(stderr,
            "[cardfs] table 0x0208ecf4: %d/13 resident (%ld bytes), "
            "%d/13 carry a ROM-written heap word\n",
            mounted, g_published_bytes, romwrote);
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

        /* The ids are the ROM's, on romdata.py's path out of the image. */
        data_0208ecf4[i].f8 = port_archive_map[i].base;
        data_0208ecf4[i].fa = port_archive_map[i].end;

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
