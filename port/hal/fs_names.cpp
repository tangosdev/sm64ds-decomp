// The NitroSDK open-BY-NAME file system: the ROM's own archive, registered by
// the ROM's own code, reading the ROM's own name table.
//
// Run link60 Stage 4, lane NFS. This retires the blocker lane FDR uncovered and
// port/tools/battery.py's SCENE_BLOCKED row for scene 374 records:
//
//     FAULT c0000005 at func_0205cdf4+0x22 accessing 0x00000010, reached
//     through mg_init -> _ZN14dScMgCurling_c13InitResourcesEv (dScMgCurling_c::InitResources)
//     -> func_ov004_020adc74 -> func_020182bc -> func_02018e3c
//     -> func_0205d644 -> func_0205d714 -> func_0205cdf4
//
// reproduced on this lane's own binary before a line of this file was written
// (SM64DS_SCENE=374 SM64DS_SCENE_FRAMES=300, exit 139, crash.txt c0000005 at
// +0x0006f5c2 accessing 0x00000010 with eax = 0).
//
// IT RETIRES THAT BLOCKER AND THE SCENE STILL DOES NOT RUN. InitResources
// completes for the first time and the scene reaches its first behaviour tick,
// where it dies in the fader dispatch port/fader_boot_map.txt section 7a
// already named as a hazard. battery.py's row for scene 374 is CONVERTED to
// that, not deleted. The fault there is quarantined, so a bare run exits 0
// over a frozen actor and only SM64DS_FAULTS_FATAL tells the truth about it;
// port/nfs_names_map.txt section 6 is the write-up, and it is a write-up
// because this lane believed the zero exit code for an hour.
//
// ---- WHY THIS IS A NEW FILE AND NOT A CHANGE TO hal/fs.cpp -----------------
//
// hal/fs.cpp owns the ID-based seam: SharedFilePtr::Load resolves an ov0 handle
// through the asset catalog and hands back bytes. It has no reference to any
// symbol on the trace above and it never will, because open-by-name is a
// different entry into the same file system and the ROM implements it with
// different code. Review ruled the routing before this lane started: the
// open-by-name seam had NO OWNER. This file is the owner.
//
// The two seams still have to agree, and the last section of this file is the
// probe that proves they do rather than asserting it.
//
// ---- WHAT WAS ACTUALLY MISSING, WHICH IS ONE CALL ---------------------------
//
// Almost the whole of NitroSDK's FS is already compiled into walk_window and
// has been for waves: func_0205cdf4 (send a command), func_0205c5e4 (execute
// one), func_0205cf5c, func_0205c788, func_0205d8d8, func_0205d044,
// func_0205d920, func_0205d568 (open-fast), func_0205d688 (read),
// FS_CloseFile, func_02018d48, func_0201834c, the nine-entry default command
// table data_02086758 (hosted in hal/ptr_tables.cpp) and every handler in it,
// including func_0205c048, which IS the ROM's path-to-file-id walker.
//
// What no port code had ever done is REGISTER AN ARCHIVE. func_0205cc80 is the
// ROM's FS_RegisterArchiveName; it was in slice_scene1.txt, /OPT:REF-dropped
// because nothing called it. With no archive registered, data_020a804c -- the
// current-directory record func_0205d714 copies into every FSFile it opens --
// stays zero, func_0205cdf4 reads `list + 0x10` off the null, and 0x10 is the
// address in the fault.
//
// So this lane did not write a file system. It ran the registration.
//
// ---- data_020a8048 AND data_020a804c, WHICH WERE MIS-IDENTIFIED ------------
//
// hal/scene_boot.cpp hosts both, and its comment reads them as "the Vram__Map
// family's LCDC cursor and its three-word bank record". They are nothing of
// the kind, and lane FDR left the contradiction open because its fault did not
// depend on the answer. This one's fix does, so it is settled here by counting
// references in the image: EVERY literal-pool reference to either address in
// the whole arm9 binary is inside the FS module --
//
//     0x020a8048  read/written at 0x0205cc7c, 0x0205cd2c, 0x0205d288
//     0x020a804c  read/written at 0x0205cc78, 0x0205cd30, 0x0205d870
//
// -- and nothing in the Vram__Map family touches either. data_020a8048 is
// NitroSDK's archive-list head (func_0205cc80 stores the first registered
// archive into it and chains the rest off ->next); data_020a804c is the
// 12-byte FSDirPos current directory {FSArchive *arc, u16 own_id, u16 index,
// u32 pos}, which is exactly why three words is the right size for it.
//
// THE STORAGE DOES NOT MOVE. Both stay in hal/scene_boot.cpp's .dsstate block.
// Their sizes were already right, only the gloss beside them was wrong, and
// moving a .dsstate member moves the whole section base -- which moves the
// selftest BMP for reasons that have nothing to do with rendering (see the
// header of port/tools/battery.py). Only the comment changes.
//
// ---- THE THREE HOST FACES, AND WHY EACH ONE IS HARDWARE --------------------
//
// The ROM's FS_Init is func_0205d96c and the port cannot run it verbatim:
//
//   * it reads the DS cartridge header's mirror in main RAM as
//     *(int *)0x027FFE40 and *(int *)0x027FFE48. 0x027FF000 is outside every
//     region ntr maps, the same unmapped-page problem hal/os_arena.cpp and
//     hal/star_flow.cpp already face for 0x027FFDA0 and 0x027FFC40.
//   * it calls func_02057020, a hand-asm primitive that allocates an OS lock
//     id out of the bitmask at 0x027FFFB0 -- also unmapped.
//   * it calls func_02060890, the card driver's own init.
//
// so func_0205d96c is FACED here, spelled as close to src/func_0205d96c.c as a
// host can be: the same globals in the same order, the same four calls into the
// ROM's own code, and the two unmapped reads replaced by the same four words
// taken from the cartridge at catalog-generate time.
//
// The other two faces are the card itself. func_0205db2c is the archive READ
// function and its whole body is func_02060918, the card ROM DMA. func_0205da94
// is the archive's command PROC and its two live cases are func_0205ffb0 /
// func_0205ff90, the card lock and unlock. Neither has a host meaning.
// (Both faces are gone from walk_window and walk_window_hires: func_0205da94
// with lane NITROFS, and func_0205db2c with run linkfull lane S4CARD, whose
// ntr/card.cpp is the cartridge the ROM's own driver reads. Only smoke_player
// keeps port_nitrofs_read.)
//
// EVERYTHING ELSE IS THE ROM'S. func_0205cd34 (FS_InitArchive), func_0205cc80
// (FS_RegisterArchiveName), func_0205c7c4 (FS_SetArchiveProc) and func_0205cb68
// (FS_LoadArchive) are the matched src TUs, called with the ROM's arguments in
// the ROM's order, and the ROM's once-guard func_0205d89c is what calls the
// face. Its argument is -1, which is the PORT'S choice and not the cartridge
// boot's (a cartridge boot runs FS_Init(2)); the derivation is beside the boot
// object at the foot of this file.
//
// ---- WHERE THE BYTES COME FROM ---------------------------------------------
//
// NO WORD IN THIS FILE IS INVENTED. The archive's four table words (FAT and FNT
// offset and size) and the two tables themselves are copied out of the
// cartridge by tools/asset_catalog.py into build/assets/nitrofs_{fat,fnt}.bin
// and build/assets/nitrofs.tsv, alongside files.tsv, which the port already
// requires. The archive's NAME is the ROM's own four rodata bytes at
// 0x0209a048, emitted by port/tools/romdata.py like every other ROM constant.
//
// The read face then has to answer a question the DS answers with a cartridge:
// "give me `len` bytes at absolute ROM offset `src`". The port does not ship
// the cartridge, so the face maps the offset BACK through the FAT -- the same
// FAT the ROM's walker just indexed -- to a file id, and serves the bytes from
// extracted/dsd/files through hal/fs.cpp's catalog. Reads inside the FNT and
// FAT spans come from the two blobs. That is the whole of it: three ranges,
// none of them guessed.
//
// One consequence worth stating plainly, because it is the strongest thing
// this seat can say: the port does not resolve the name. The ROM does. The
// string "/MG/d_2d_mg_bg_curling1_ncg.bin" is walked by func_0205c048 through
// Nintendo's own directory table, and the file id that comes out is whatever
// the cartridge says it is.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dsstate_seg.h"

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

#ifndef PORT_REPO_ROOT
#define PORT_REPO_ROOT "."
#endif

extern "C" {

/* ---- the ROM's own code this seat calls ---------------------------------- */
void func_0205cd34(void *archive);                       /* FS_InitArchive    */
int  func_0205cc80(void *archive, void *name, void *len); /* register the name */
void func_0205c7c4(int *archive, int proc, int flags);   /* FS_SetArchiveProc */
int  func_0205cb68(int *archive, int base, int fat, int fat_size,
                   int fnt, int fnt_size, int read_fn, int write_fn);
void func_0205d89c(int dma);                             /* the once-guard    */
int  func_0205db24(void);                                /* the write stub    */

/* ---- hal/nitrofs_boot.cpp -------------------------------------------------
   The cartridge-header mirror at 0x027FFE40, and the read-back that proves the
   ROM's own FS_Init went through it. Both are called from this file's static
   initialiser, in that order, around the once-guard. */
void port_nitrofs_header_mirror_seed(void);
void port_nitrofs_boot_report(void);

/* ---- hal/fs.cpp's read-only catalog accessor ------------------------------
   The one thing this seam borrows from the id-based one, and it is borrowed
   rather than copied ON PURPOSE. A second catalog reader in this file could
   drift from fs.cpp's by a row and nothing would notice; sharing the table
   makes "the two seams agree about what file id 1672 is" true by construction
   instead of by assertion. It is a read-only lookup, not a routing change:
   fs.cpp still owns no symbol on the open-by-name trace. */
const char *port_fs_catalog_path(unsigned file_id);

/* ---- the ROM's archive name, four rodata bytes at 0x0209a048 -------------- */
extern unsigned char data_0209a048[];

/* ---- the DS globals FS_Init owns ------------------------------------------
   All hosted, all inside the .dsstate bracket, because a save state that
   rolled data_020a8048 (in hal/scene_boot.cpp) back without rolling the
   archive it points AT back with it would restore a consistent-looking list
   head over an inconsistent archive.

   EVERY SIZE HERE IS THE ROM SPAN, not the width of the field the one caller
   touches -- the undersized-global trap hal/scene_boot.cpp names. Read off
   config/arm9/symbols.txt as the delta to the next symbol:

     data_020a8058   4   0x020a805c is next
     data_020a805c   4   0x020a8060
     data_020a8060   4   0x020a8064
     data_020a8064   8   0x020a806c
     data_020a806c   8   0x020a8074
     data_020a8074  80   0x020a80c4

   The archive's 80 is confirmed twice over: the config's span says 0x50 and
   func_0205cd34 memsets 0x50, with the highest field anything writes being the
   command bitmask at +0x4c. Twenty words. */
DSSTATE_BEGIN
int data_020a8074[0x50 / 4];  /* the rom archive record                       */
int data_020a8058;            /* func_0205d89c's once flag                    */
void *data_020a8060;          /* FS_Init's argument: the card DMA channel     */
int data_020a805c;            /* the OS lock id func_02057020 allocates       */
int data_020a8064[2];         /* FS_Init zeroes both pairs; func_02018c00     */
int data_020a806c[2];         /* reads them                                   */
DSSTATE_END

}  /* extern "C" */

#ifdef PORT_ROM_CLEAN
/* A shipping build has no PORT_REPO_ROOT fallback for assets.
   hal/asset_root_refuse.cpp is the write-up and port/kit_pipeline.txt is the
   failure story. THIS SEAM IS THE ONE THE MASK COST US: the three files
   asset_root() below resolves are the ones the kit's extractor never wrote,
   and the fallback found them in the developer's checkout every single time
   anyone tested a kit.

   Declared out here rather than beside asset_root(), which is inside the
   unnamed namespace below: a C-linkage name has no business being introduced
   from inside one. */
extern "C" void port_asset_root_refuse(const char *wanted);
#endif

namespace {

/* ---- the cartridge's own FNT and FAT --------------------------------------
   Loaded once, lazily, from the catalog products. Refusing loudly rather than
   limping is deliberate and it is hal/fs.cpp's precedent: a port that quietly
   serves nothing for a missing catalog produces a crash three layers away from
   the cause.

   IT REFUSES WITH exit(2) AND NOT WITH abort(), which is run link60 lane FDR2
   taking the handover port/nfs_names_map.txt section 8 left open. The messages
   below are unchanged, word for word; only the way the process leaves is
   different, and the reason is that abort() told the truth badly. Every target
   carrying this seat calls FS_Init from a static initialiser, so a tree whose
   build/assets predates the seat dies BEFORE main -- and MSVC's abort() raises
   STATUS_STACK_BUFFER_OVERRUN, so the shell reports 0xC0000409 and every crash
   reporter in the chain reads a missing catalog file as a memory-safety crash
   in the port. It is a configuration error. exit(2) is the same loudness on
   stderr, one small integer on the way out, and a code a script can branch on.

   WHAT IS GIVEN UP, said plainly: exit() runs atexit handlers and static
   destructors on the way out and abort() does not, so a failure this early
   unwinds more of a half-built program than it used to. Nothing here depends
   on that unwind succeeding -- the message is already on stderr and flushed by
   the time exit is called -- but a successor debugging a HANG rather than a
   refusal should look here first. */
struct NitroTables {
    int loaded;
    u32 fnt_off, fnt_size, fat_off, fat_size;
    u8 *fnt;
    u8 *fat;
    /* The ARM9 overlay table (cartridge header +0x50/+0x54), for the virtual
       ROM image only. Optional, like hal/nitrofs_boot.cpp's overlay words: a
       catalog without it serves 0xFF there instead of refusing to start. */
    u32 ovt_off, ovt_size;
    u8 *ovt;
};

NitroTables g_tables;

const char *asset_root(void)
{
    const char *env = getenv("SM64DS_ASSET_ROOT");
#ifdef PORT_ROM_CLEAN
    if (!env)
        port_asset_root_refuse("build/assets/nitrofs.tsv");
#endif
    return env ? env : PORT_REPO_ROOT;
}

u8 *slurp(const char *what, const char *name, u32 want)
{
    char path[520];
    FILE *f;
    u8 *buf;
    long got;

    snprintf(path, sizeof path, "%s/build/assets/%s", asset_root(), name);
    f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "FATAL: NitroFS %s table missing: %s\n"
                "       run tools/asset_catalog.py generate <rom>\n",
                what, path);
        fflush(stderr);
        exit(2);
    }
    fseek(f, 0, SEEK_END);
    got = ftell(f);
    fseek(f, 0, SEEK_SET);
    if ((u32)got != want) {
        fprintf(stderr, "FATAL: NitroFS %s table is %ld bytes, nitrofs.tsv "
                "says %u -- regenerate the catalog\n", what, got, want);
        fflush(stderr);
        exit(2);
    }
    buf = (u8 *)malloc(want ? want : 1);
    if (!buf || fread(buf, 1, want, f) != want) {
        fprintf(stderr, "FATAL: NitroFS %s table short read: %s\n", what, path);
        fflush(stderr);
        exit(2);
    }
    fclose(f);
    return buf;
}

void tables_load(void)
{
    char path[520], line[256];
    FILE *f;

    if (g_tables.loaded)
        return;
    g_tables.loaded = 1;

    snprintf(path, sizeof path, "%s/build/assets/nitrofs.tsv", asset_root());
    f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "FATAL: NitroFS table index missing: %s\n"
                "       run tools/asset_catalog.py generate <rom>\n", path);
        fflush(stderr);
        exit(2);
    }
    fgets(line, sizeof line, f); /* header row */
    while (fgets(line, sizeof line, f)) {
        char key[64];
        unsigned value;
        if (sscanf(line, "%63[^\t]\t%u", key, &value) != 2)
            continue;
        if (!strcmp(key, "fnt_offset")) g_tables.fnt_off = value;
        else if (!strcmp(key, "fnt_size")) g_tables.fnt_size = value;
        else if (!strcmp(key, "fat_offset")) g_tables.fat_off = value;
        else if (!strcmp(key, "fat_size")) g_tables.fat_size = value;
        else if (!strcmp(key, "ovt9_offset")) g_tables.ovt_off = value;
        else if (!strcmp(key, "ovt9_size")) g_tables.ovt_size = value;
    }
    fclose(f);

    if (!g_tables.fnt_size || !g_tables.fat_size) {
        fprintf(stderr, "FATAL: %s carries no FNT/FAT spans -- regenerate the "
                "catalog with a tools/asset_catalog.py that writes them\n",
                path);
        fflush(stderr);
        exit(2);
    }
    g_tables.fnt = slurp("FNT", "nitrofs_fnt.bin", g_tables.fnt_size);
    g_tables.fat = slurp("FAT", "nitrofs_fat.bin", g_tables.fat_size);
}

/* The overlay table's blob, loaded on the first virtual-image read that
   needs it and never at start-up: a missing or short file costs that span
   its bytes (0xFF, said once), not the boot. */
void ovt_load(void)
{
    static int tried;
    char path[520];
    FILE *f;
    long got;

    if (tried)
        return;
    tried = 1;
    if (!g_tables.ovt_size)
        return;
    snprintf(path, sizeof path, "%s/build/assets/nitrofs_ovt9.bin", asset_root());
    f = fopen(path, "rb");
    if (f) {
        fseek(f, 0, SEEK_END);
        got = ftell(f);
        fseek(f, 0, SEEK_SET);
        if ((u32)got == g_tables.ovt_size) {
            g_tables.ovt = (u8 *)malloc(g_tables.ovt_size);
            if (g_tables.ovt && fread(g_tables.ovt, 1, g_tables.ovt_size, f) != g_tables.ovt_size) {
                free(g_tables.ovt);
                g_tables.ovt = 0;
            }
        }
        fclose(f);
    }
    if (!g_tables.ovt)
        fprintf(stderr, "[nfs] the virtual ROM image serves 0xFF over the ARM9 "
                "overlay table (%#x+%#x): %s is missing or not that size\n",
                g_tables.ovt_off, g_tables.ovt_size, path);
}

/* ---- THE FILES BEHIND THE VIRTUAL ROM IMAGE (run linkfull, lane S4FILE) ---
   Since this lane the game's own loaders -- LoadFile, LoadCompressedFileAt's
   func_0201817c, and the func_0201818c they share -- read loose files through
   the card, where they used to read them through hal/fs.cpp's host copy of
   SharedFilePtr::Load and its file cache. Two things moved under the read with
   them, and both live here because this is where the cartridge's bytes are
   served.

   THE CACHE. A card block is 0x200 bytes, and the image used to answer each one
   with a scan of all 2,175 FAT entries and an fopen / fseek / fread / fclose of
   every file the block touched: a 70 KB collision file cost 140 opens. The
   cartridge is read-only, so keeping a file's bytes after the first block that
   touches it changes nothing the game can see -- the card returns what it
   returned before -- and a sorted index finds the files a block covers without
   the scan. This is hal/fs.cpp's jump-hitch cache in its honest place: it
   caches the CARTRIDGE, under the read, instead of the decoded file above it.

   THE PATCHED RANGE. A mod serves a file the game reads by id (hal/fs_mods.cpp
   states the rules; hal/stage_geom.cpp's collision swap is the one whose file
   travels the ROM loaders today). The ROM learns a file's size from the FAT,
   so a mod file that grew cannot sit in the ROM file's own range: the FAT the
   card serves points a modded id at a range PAST THE CARTRIDGE instead, and
   that range holds the ROM file run through the mod chain (hal/fs.cpp,
   port_fs_card_patches). The cartridge's own range still holds the cartridge's
   own bytes, so every neighbour a block crosses into reads as it always did.
   Latched once, on the first card read, like every mod. */
struct ImgFile { u32 start, stop, id; };
ImgFile *g_img;          /* the FAT's live entries, sorted by start          */
u32 g_img_n;
u8 **g_img_bytes;        /* by FAT id: the file's bytes once touched          */

enum : u32 { PATCH_BASE = 0x10000000u, PATCH_ALIGN = 0x200u };
u8 *g_fat_served;        /* the FAT as the card serves it: patched entries    */
u8 *g_patch;             /* the patched range's bytes, from PATCH_BASE        */
u32 g_patch_len;

int img_cmp(const void *a, const void *b)
{
    const ImgFile *x = (const ImgFile *)a, *y = (const ImgFile *)b;
    return x->start < y->start ? -1 : x->start > y->start;
}

void img_index(void)
{
    u32 n = g_tables.fat_size / 8, i;
    if (g_img)
        return;
    g_img = (ImgFile *)calloc(n ? n : 1, sizeof *g_img);
    g_img_bytes = (u8 **)calloc(n ? n : 1, sizeof *g_img_bytes);
    if (!g_img || !g_img_bytes) {
        fprintf(stderr, "FATAL: out of host memory indexing the FAT\n");
        fflush(stderr);
        exit(2);
    }
    for (i = 0; i < n; ++i) {
        u32 start, stop;
        memcpy(&start, g_tables.fat + i * 8, 4);
        memcpy(&stop, g_tables.fat + i * 8 + 4, 4);
        if (stop <= start)
            continue;
        g_img[g_img_n].start = start;
        g_img[g_img_n].stop = stop;
        g_img[g_img_n].id = i;
        ++g_img_n;
    }
    qsort(g_img, g_img_n, sizeof *g_img, img_cmp);
}

/* The whole file behind FAT entry `f`, read on its first touch and kept. A
   file the catalog names and the disk does not hold is a broken extraction
   (a correct one holds all of them): the game would read 0xFF where its data
   should be and fail somewhere far from the cause, so this stops it here and
   names the file. */
const u8 *img_file(const ImgFile *f, const char *rel)
{
    u8 *buf = g_img_bytes[f->id];
    char path[520];
    FILE *fp;
    u32 want;
    if (buf)
        return buf;
    want = f->stop - f->start;
    snprintf(path, sizeof path, "%s/extracted/dsd/files/%s", asset_root(), rel);
    fp = fopen(path, "rb");
    buf = (u8 *)malloc(want);
    if (!fp || !buf || fread(buf, 1, want, fp) != want) {
        fprintf(stderr, "FATAL: the game read file id %u (%s) and it is %s: "
                "the extracted game files are incomplete; extract them again\n",
                f->id, rel, fp ? "shorter than the cartridge says" : "missing");
        fflush(stderr);
        exit(2);
    }
    fclose(fp);
    g_img_bytes[f->id] = buf;
    return buf;
}

/* hal/fs.cpp hands each modded file here once, at the latch. */
void patch_emit(unsigned id, const u8 *bytes, u32 len)
{
    u32 at, top;
    if (id >= g_tables.fat_size / 8 || !len)
        return;
    at = (g_patch_len + PATCH_ALIGN - 1) & ~(PATCH_ALIGN - 1);
    top = at + len;
    u8 *grown = (u8 *)realloc(g_patch, top);
    if (!grown) {
        fprintf(stderr, "[mods] out of host memory placing file id %u in the "
                "card's patched range; it keeps the cartridge's bytes\n", id);
        return;
    }
    g_patch = grown;
    memset(g_patch + g_patch_len, 0xFF, at - g_patch_len);
    memcpy(g_patch + at, bytes, len);
    g_patch_len = top;
    {
        const u32 start = PATCH_BASE + at, stop = PATCH_BASE + top;
        memcpy(g_fat_served + id * 8, &start, 4);
        memcpy(g_fat_served + id * 8 + 4, &stop, 4);
    }
}

}  /* anonymous namespace */

extern "C" void port_fs_card_patches(void (*emit)(unsigned, const u8 *, u32));

namespace {

void patch_latch(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    g_fat_served = (u8 *)malloc(g_tables.fat_size);
    if (!g_fat_served) {
        fprintf(stderr, "FATAL: out of host memory copying the FAT\n");
        fflush(stderr);
        exit(2);
    }
    memcpy(g_fat_served, g_tables.fat, g_tables.fat_size);
    port_fs_card_patches(patch_emit);
}

/* The FAT is 8 bytes per file id, {u32 start, u32 end}, in id order but NOT in
   offset order for every ROM, so the lookup is a scan rather than a bisect.
   2,175 entries and one call per file load; the cost is not worth an index.
   (smoke_player's synchronous face only; the card image uses img_index.) */
int fat_entry_of(u32 off, u32 len, u32 *start_out)
{
    u32 n = g_tables.fat_size / 8;
    u32 i;
    for (i = 0; i < n; ++i) {
        u32 start, end;
        memcpy(&start, g_tables.fat + i * 8, 4);
        memcpy(&end, g_tables.fat + i * 8 + 4, 4);
        if (end <= start)
            continue;
        if (off >= start && off < end && off + len <= end) {
            *start_out = start;
            return (int)i;
        }
    }
    return -1;
}

int g_read_calls;
int g_read_file_calls;

/* SM64DS_NFS_TRACE=1 prints every read the ROM's walker asks for. This is the
   only window onto what the cartridge's own code is doing inside a seam with
   no host logic in it, and it earned its keep on the first run: the read log
   is how the archive's re-entrancy showed up. */
int trace_on(void)
{
    static int state;
    if (!state) {
        const char *e = getenv("SM64DS_NFS_TRACE");
        state = (e && *e && strcmp(e, "0")) ? 2 : 1;
    }
    return state == 2;
}

/* FNV-1a 64 over a buffer: the trace's digest of what a read left in its
   destination. A digest rather than a dump so a whole sweep's reads compare
   line for line; not md5 because that would be new code or a new import for
   an equality test this small. */
unsigned long long fnv1a64(const unsigned char *p, u32 n)
{
    unsigned long long h = 0xcbf29ce484222325ull;
    u32 i;
    for (i = 0; i < n; ++i) {
        h ^= p[i];
        h *= 0x100000001b3ull;
    }
    return h;
}

/* THE READ TRACE'S TAP (run linkfull, lane S4CARD). Under SM64DS_NFS_TRACE the
   archive's two read slots (+0x3c, +0x44: func_0205cb68 fills both from the
   one pointer FS_Init hands it) are pointed at this wrapper, which calls the
   read function they held and then prints what it left in the destination.
   The line is the same on a build whose archive reads through
   port_nitrofs_read and on one whose archive reads through the ROM's own
   func_0205db2c and the card driver behind it, which is the per-read A/B the
   card model is held to.

   WHEN IT HASHES. port_nitrofs_read is synchronous. func_0205db2c returns 6
   and finishes on the ROM's card thread, which outranks the thread that
   asked (priority 4, set by src/func_0206002c.c), so the wake inside
   func_02060228 runs the whole transfer -- completion callback func_0205db78
   included -- before func_0205db2c returns here. The wrapper reads the
   driver's busy bit (data_020a8180 + 0x34, bit 2) to show that rather than
   assume it: "done" is the completion having run, "pending" would be a read
   still in flight, which this port has not produced. Trace-only: with the
   variable unset the slots are the ROM's and this code never runs. */
typedef int (*ArchiveRead)(void *archive, void *dst, u32 src, u32 len);
ArchiveRead g_traced_read;

}  /* anonymous namespace */

extern "C" unsigned char data_020a8180[];   /* hal/globals_link100.cpp */

namespace {

int traced_read(void *archive, void *dst, u32 src, u32 len)
{
    const int rc = g_traced_read(archive, dst, src, len);
    const int busy = (data_020a8180[0x34] & 4) != 0;
    fprintf(stderr, "  [nfs] read %#x +%#x rc %d %s fnv %016llx\n", src, len, rc,
            busy ? "pending" : "done", fnv1a64((const unsigned char *)dst, len));
    return rc;
}

}  /* anonymous namespace */

extern "C" {

/* ---- smoke_player's archive read function ---------------------------------
   ON walk_window AND walk_window_hires THIS IS NO LONGER THE READ FUNCTION
   (run linkfull, lane S4CARD). Those two targets compile src/func_0205d96c.c
   without the `func_0205db2c=port_nitrofs_read` rename that stood in
   port/CMakeLists.txt, so FS_Init registers the ROM's own func_0205db2c: one
   call to func_02060918, which hands the transfer to the ROM's card thread
   (func_02060228), whose func_02060a30 / func_02060a64 read the cartridge
   through ROMCTRL and the data port -- ntr/card.cpp -- and whose completion
   func_02060e38 calls func_0205db78, which wakes the waiting reader. The
   bytes come from port_nitrofs_rom_read below, the same FNT, FAT and files
   this body reads.

   It stays for smoke_player, whose FS_Init is hal/nitrofs_face.cpp's face
   over port_nitrofs_fs_init: that target carries neither the card driver nor
   the OS lock family the ROM's archive proc reaches.

   PORT_HOST_ABI (smoke_player only): src is one call to func_02060918, and
   the card returns 6 (async) with the completion arriving from the card
   thread. This reads synchronously and returns 0, which is the result the
   SDK's own synchronous archive readers return and which every caller here
   already handles: func_0205c528 treats 0 and 1 as done, and func_0205c5e4
   only takes the sleep-until-woken path on 6.

   Called as (archive, dst, absolute ROM offset, length) from two places -- the
   archive's +0x3c slot for file bytes (func_0205c448) and its +0x44 copy for
   name-table words (func_0205c528). FS_LoadArchive sets both from the same
   pointer, so there is one function and not two. Under SM64DS_NFS_TRACE its
   line is printed by the tap above (traced_read), with a digest of what the
   read left in dst. */
int port_nitrofs_read(void *archive, void *dst, u32 src, u32 len)
{
    u32 start = 0;
    int file_id;
    const char *rel;
    char path[520];
    FILE *f;

    (void)archive;
    tables_load();
    ++g_read_calls;

    if (len == 0)
        return 0;

    if (src >= g_tables.fnt_off && src + len <= g_tables.fnt_off + g_tables.fnt_size) {
        memcpy(dst, g_tables.fnt + (src - g_tables.fnt_off), len);
        return 0;
    }
    if (src >= g_tables.fat_off && src + len <= g_tables.fat_off + g_tables.fat_size) {
        memcpy(dst, g_tables.fat + (src - g_tables.fat_off), len);
        return 0;
    }

    file_id = fat_entry_of(src, len, &start);
    if (file_id < 0) {
        fprintf(stderr, "nfs: ROM offset %#x+%#x is in no FAT entry and in "
                "neither table span\n", src, len);
        return 1;
    }
    rel = port_fs_catalog_path((unsigned)file_id);
    if (!rel || !rel[0]) {
        fprintf(stderr, "nfs: FAT entry %d (offset %#x) has no catalog path\n",
                file_id, src);
        return 1;
    }
    snprintf(path, sizeof path, "%s/extracted/dsd/files/%s", asset_root(), rel);
    f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "nfs: file id %d missing on disk: %s\n", file_id, path);
        return 1;
    }
    if (fseek(f, (long)(src - start), SEEK_SET) != 0 ||
        fread(dst, 1, len, f) != len) {
        fprintf(stderr, "nfs: short read of %#x bytes at +%#x in %s\n",
                len, src - start, path);
        fclose(f);
        return 1;
    }
    fclose(f);
    ++g_read_file_calls;
    return 0;
}

/* ---- THE VIRTUAL ROM IMAGE, for ntr/card.cpp ------------------------------
   Run linkfull, lane S4CARD. The game card model answers a B7h command with a
   block of the cartridge; this assembles that block out of the same three
   sources the read face above serves from, laid at their own ROM offsets:

     the FNT and the FAT          build/assets/nitrofs_{fnt,fat}.bin
     the ARM9 overlay table       build/assets/nitrofs_ovt9.bin (header +0x50)
     every FAT entry's file       extracted/dsd/files/<the catalog's path>

   and 0xFF everywhere none of them lies (the header, the ARM binaries, the
   overlay files, the banner and the padding between files: the catalog carries
   no path for FAT ids 0..102, the overlays, and nothing in this port reads
   them through the card). A block may cross from one source into the next --
   BUILDTIME's page also holds the start of the NARC after it -- which the face
   above refused and a cartridge does not. Nothing is kept: every call reads
   what it needs, so a block costs a few small reads and there is no 16 MB
   buffer (port/tools/card_image_check.py compares served blocks to a .nds). */
void port_nitrofs_rom_read(u32 addr, u32 len, unsigned char *out)
{
    static unsigned long long unserved;
    const u32 end = addr + len;
    u32 i, lo_i, hi_i;

    memset(out, 0xFF, len);
    if (!len)
        return;
    tables_load();
    ovt_load();
    img_index();
    patch_latch();

    {   /* the three tables; the FAT as served, with any mod file's entry
           pointing into the patched range */
        struct { u32 off, size; const u8 *blob; } t[3] = {
            {g_tables.fnt_off, g_tables.fnt_size, g_tables.fnt},
            {g_tables.fat_off, g_tables.fat_size, g_fat_served},
            {g_tables.ovt_off, g_tables.ovt_size, g_tables.ovt},
        };
        for (i = 0; i < 3; ++i) {
            const u32 lo = t[i].off > addr ? t[i].off : addr;
            const u32 hi = t[i].off + t[i].size < end ? t[i].off + t[i].size : end;
            if (t[i].blob && lo < hi)
                memcpy(out + (lo - addr), t[i].blob + (lo - t[i].off), hi - lo);
        }
    }

    if (g_patch_len && end > PATCH_BASE && addr < PATCH_BASE + g_patch_len) {
        const u32 lo = addr > PATCH_BASE ? addr : PATCH_BASE;
        const u32 hi = end < PATCH_BASE + g_patch_len ? end : PATCH_BASE + g_patch_len;
        memcpy(out + (lo - addr), g_patch + (lo - PATCH_BASE), hi - lo);
    }

    /* The first entry that ends past addr, then every entry that starts
       before end: the files this block covers, in offset order. */
    lo_i = 0;
    hi_i = g_img_n;
    while (lo_i < hi_i) {
        const u32 mid = lo_i + (hi_i - lo_i) / 2;
        if (g_img[mid].start < addr && g_img[mid].stop <= addr)
            lo_i = mid + 1;
        else
            hi_i = mid;
    }
    if (lo_i > 0 && g_img[lo_i - 1].stop > addr)
        --lo_i;   /* an entry that starts before addr and still covers it */
    for (i = lo_i; i < g_img_n && g_img[i].start < end; ++i) {
        const ImgFile *f = &g_img[i];
        const u32 lo = f->start > addr ? f->start : addr;
        const u32 hi = f->stop < end ? f->stop : end;
        const char *rel;
        if (lo >= hi)
            continue;
        rel = port_fs_catalog_path(f->id);
        if (!rel || !rel[0]) {
            if (!unserved++)
                fprintf(stderr, "[nfs] the virtual ROM image has no file for FAT "
                        "entry %u (%#x..%#x); served as 0xFF\n", f->id,
                        f->start, f->stop);
            continue;
        }
        memcpy(out + (lo - addr), img_file(f, rel) + (lo - f->start), hi - lo);
    }
}

/* ---- FACE: the archive command proc (ROM: func_0205da94) ------------------
   PORT_HOST_ABI: the ROM's two live cases are func_0205ffb0 and func_0205ff90,
   the card lock and unlock, reached through the OS lock id func_02057020
   allocates out of the unmapped bitmask at 0x027FFFB0. A host with no card has
   nothing to lock, so the two cases keep their RESULTS and drop the hardware;
   case 1 and the fall-through keep the ROM's exact return values.

   IT IS SET AT ALL FOR COMPLETENESS, not because this path needs it. The
   commands the linked callers actually send are 0 (read, func_0205d688), 4
   (find-by-name, func_0205d714), 5 (func_0205d4a0), 6 (open-fast,
   func_0205d568), 7 (func_0205d5e8) and 8 (close, FS_CloseFile) -- every one
   of them inside the nine-entry default table. Commands 1, 9 and 10 are the
   three the ROM's bitmask 0x602 claims for this proc and nothing in the link
   sends them. Registering the proc anyway keeps the archive configured the way
   the cartridge configures it AND stops a command 9 or 10 from ever indexing
   data_02086758 (nine entries) off the end. */
int port_nitrofs_proc(void *file, int command)
{
    (void)file;
    if (trace_on())
        fprintf(stderr, "  [nfs] proc command %d\n", command);
    switch (command) {
    case 9:  return 0;   /* ROM: lock the card, then succeed              */
    case 10: return 0;   /* ROM: unlock the card, then succeed            */
    case 1:  return 4;   /* ROM: refuse the async read outright           */
    }
    return 8;            /* ROM: not mine, fall through to the default    */
}

/* ---- FACE: FS_Init (ROM: func_0205d96c) -----------------------------------
   Line for line against src/func_0205d96c.c, with the three unmapped or
   card-bound pieces named where they are dropped. Everything that remains is a
   call into the ROM's own matched code. smoke_player's alone (hal/
   nitrofs_face.cpp); it registers the synchronous port_nitrofs_read, which is
   what lets SM64DS_NFS_PROBE run from the static initialiser there. */
static int g_sync_face;

void port_nitrofs_fs_init(void *dma)
{
    g_sync_face = 1;
    data_020a8060 = dma;
    /* src: data_020a805c = func_02057020(), which allocates an OS lock id from
       the unmapped bitmask at 0x027FFFB0. Nothing on the host locks a card, so
       the id stays 0 and port_nitrofs_proc never reads it. */
    data_020a805c = 0;
    data_020a8064[0] = 0;
    data_020a8064[1] = 0;
    data_020a806c[0] = 0;
    data_020a806c[1] = 0;
    /* src: func_02060890(), the card driver's own init. No card. */

    tables_load();

    func_0205cd34(&data_020a8074);
    /* THE REGISTRATION. The ROM's function, the ROM's archive record, the
       ROM's name bytes and the ROM's length. This is the call whose absence
       was the whole blocker. */
    func_0205cc80(&data_020a8074, data_0209a048, (void *)3);

    /* src guards on the header mirror's four words being neither 0 nor -1
       before it loads the archive; the same guard, over the same four words,
       read out of the cartridge by tools/asset_catalog.py instead of out of
       0x027FFE40. */
    if (!g_tables.fat_off || g_tables.fat_off == 0xffffffffu)
        return;
    if (!g_tables.fnt_off || g_tables.fnt_off == 0xffffffffu)
        return;

    func_0205c7c4(data_020a8074, (int)(size_t)&port_nitrofs_proc, 0x602);
    func_0205cb68(data_020a8074, 0,
                  (int)g_tables.fat_off, (int)g_tables.fat_size,
                  (int)g_tables.fnt_off, (int)g_tables.fnt_size,
                  (int)(size_t)&port_nitrofs_read,
                  (int)(size_t)&func_0205db24);
}

/* THE ROM-NAMED WRAPPER MOVED, and on the two window targets it is gone.
   Run link100, lane NITROFS. The claim this file made -- "both pages are
   unmapped in the port" -- was false, and had been since ntr/io.cpp made
   SHARED_BASE a fatal region: 0x027FFE40, 0x027FFE48 and 0x027FFFB0 all sit
   inside 0x027ff000 + 0x1000, and hal/os_lockid.cpp has been writing the last
   of them for as long as this file claimed it could not. The mirror was
   MAPPED AND NEVER WRITTEN. hal/nitrofs_boot.cpp writes the four
   cartridge-header words into it, out of the same catalog this file reads,
   and walk_window and walk_window_hires now run the cartridge's own
   func_0205d96c from port/slice_nitrofs.txt. The once-guard's argument is
   unchanged and the derivation above still stands.

   smoke_player does not carry hal/boot_hw.cpp, so it cannot carry the OS lock
   family the matched archive proc reaches; it keeps the face, alone, in
   hal/nitrofs_face.cpp. port_nitrofs_fs_init above is still the body behind
   that face, unchanged; on the window targets nothing references it and
   /OPT:REF drops it. */

/* ---- the four cartridge-header words, for hal/nitrofs_boot.cpp ------------
   ONE READER. This file already parses build/assets/nitrofs.tsv, refuses
   loudly when it is missing, and serves the ROM's own FNT and FAT walker out
   of the two blobs beside it. The mirror seed asks it for the same four
   values rather than opening the file a second time, so the bytes the ROM
   reads at 0x027FFE40 and the bytes the read function serves cannot drift
   apart across a regeneration. */
void port_nitrofs_header_words(u32 *fnt_off, u32 *fnt_size,
                               u32 *fat_off, u32 *fat_size)
{
    tables_load();
    *fnt_off = g_tables.fnt_off;
    *fnt_size = g_tables.fnt_size;
    *fat_off = g_tables.fat_off;
    *fat_size = g_tables.fat_size;
}

/* ---- FACE: func_02018e3c, and it is an ABI face, not a hardware one -------
   PORT_HOST_ABI: a RIDE-THROUGH. The ROM body is five instructions,
;      push {r4, lr} / bl func_0205d644 / movs r4, r0 / bne +0x10
;      ldr r0, =data_0208ecd8 / bl func_02018e68 / bl Crash
   and r0 and r1 -- the FSFileID out pointer and the path -- are carried into
   func_0205d644 in the registers they already occupy. The decompilation says
   so by naming no parameters at all (`int func_02018e3c(void)` calling
   `func_0205d644()`), which is faithful to the ARM and unbuildable on a host:
   func_020182bc pushes two arguments, this callee forwards none, and
   func_0205d644 reads two stack words that are not them.

   THE MEASUREMENT THAT CONVICTED IT, because "cdecl does not ride" is an
   argument and this lane wanted evidence. With the archive registered, a
   scene-374 run under SM64DS_NFS_TRACE=1 shows the ROM's own walker read the
   FNT root entry, walk all four root names (BUILDTIME, ARCHIVE, data, MG) and
   fall off the terminator without matching -- while SM64DS_NFS_PROBE=1, which
   calls func_0205d644 from C++ with both arguments spelled out, resolves the
   same two names to file ids 1672 and 1676 on the same binary in the same
   boot. Same seam, same tables, same archive; the only difference is who
   supplied the arguments.

   Identical trade to hal/fs.cpp's SharedFilePtr::Construct face, whose header
   describes the same shape in the id-based seam, and to the two ride-through
   command handlers hal/ptr_tables.cpp types through.

   RETIRED, run link100 wave 15 lane SEAT15C. Everything above stays true of
   the ARM and of the source this face was written against; it is no longer
   true of src/func_02018e3c.c, which today reads

       extern int func_0205d644(void *out_file_id, const char *path);
       int func_02018e3c(void *out_file_id, const char *path){
         int r = func_0205d644(out_file_id, path);
         if(!r){ func_02018e68(data_0208ecd8); Crash(); }
         return r; }

   -- both parameters named, both forwarded, and its own banner records the
   same three callers this one does (func_020182bc, func_02018434,
   func_020189f0). That is statement-for-statement the body this face held, so
   the matched TU carries the row: port/slice_l15fs.txt. The declarations below
   stay because port_nitrofs_report and the probe further down use them.

   The SM64DS_NFS_TRACE / SM64DS_NFS_PROBE measurement that convicted the old
   source is kept above on purpose: it is the evidence for why the face was
   right when it was written, and it is the test to re-run if a future sync
   ever walks the source back. */
int func_0205d644(void *out_file_id, const char *path);
void func_02018e68(void *message);
void Crash(void);
extern int data_0208ecd8[];

/* ---- the report, and the cross-seam probe ---------------------------------
   Constraint from review: whatever this seam resolves a name to must be what
   the asset catalog serves for the same name. The two directions are checked
   against each other here rather than asserted anywhere.

   FORWARD is the ROM's: func_0205d644 is FS_ConvertPathToFileID, and it walks
   the cartridge's own FNT through this file's read face. BACKWARD is
   hal/fs.cpp's: the catalog's path for that id. If the ROM's walker and the
   catalog disagree about a name, the two strings differ and this says so.

   The subject is the file scene 374 actually opens, which makes the probe a
   test of the live path rather than of a convenient one. */
void port_nitrofs_report(void)
{
    static const char *const probe[] = {
        "/MG/d_2d_mg_bg_curling1_ncg.bin",
        "/MG/d_2d_mg_bg_curling2_ncg.bin",
    };
    unsigned i;

    /* +0x00 is the packed name func_0205cc80 writes last; nonzero there is
       the registration having happened, not merely been attempted. */
    printf("[nfs] rom archive at %p, name word %#x (%s), %d read(s), "
           "%d served from disk\n",
           (void *)data_020a8074, (unsigned)data_020a8074[0],
           data_020a8074[0] ? "REGISTERED" : "NOT REGISTERED",
           g_read_calls, g_read_file_calls);

    for (i = 0; i < sizeof probe / sizeof probe[0]; ++i) {
        int fid[2];
        const char *catalog;
        fid[0] = 0;
        fid[1] = -1;
        if (!func_0205d644(fid, probe[i])) {
            printf("[nfs] %s: the ROM's own walker did not resolve it\n",
                   probe[i]);
            continue;
        }
        catalog = port_fs_catalog_path((unsigned)fid[1]);
        printf("[nfs] %s -> file id %d, catalog says \"%s\"%s\n",
               probe[i], fid[1], catalog ? catalog : "(none)",
               (catalog && !strcmp(catalog, probe[i] + 1))
                   ? "  AGREE" : "  DISAGREE");
    }
    fflush(stdout);
}

/* SM64DS_NFS_PROBE, after the card bring-up (run linkfull, lane S4CARD). On
   walk_window and walk_window_hires the archive reads through the ROM's card
   driver, whose reads finish on the card thread, and that thread is created by
   func_02060890 in main's func_0201a054 seam -- so a probe from the static
   initialiser below would wait for a thread that does not exist yet.
   hal/boot_arms.cpp calls this right after that arm; smoke_player's synchronous
   face still probes from the static initialiser. */
void port_nitrofs_probe_after_boot(void)
{
    const char *probe = getenv("SM64DS_NFS_PROBE");
    if (!g_sync_face && probe && *probe && strcmp(probe, "0"))
        port_nitrofs_report();
}

}  /* extern "C" */

namespace {

/* Run the ROM's once-guard, which runs the face above, which runs the ROM's
   registration.

   THE ARGUMENT IS -1, AND IT IS THE PORT'S CHOICE, NOT THE CARTRIDGE'S. An
   older note here said the DS ends up with -1; it does not. func_0205d89c has
   two callers: func_02018aa4 passes 2 and func_02040c34 passes -1. On a
   cartridge boot src/func_0201a054.c:60-61 calls func_02018aa4 (its `if (!r4)`
   arm, r4 being the download-play word at 0x027ffc40), so the DS runs
   FS_Init(2): card DMA on channel 2, completion on IRQ 19. func_02040c34 is
   the wireless system init (src/func_02040c34.c:60), not the cartridge boot.

   THE PORT KEEPS -1 ON PURPOSE (stage A of run linkfull lane S4CARD's card
   model). With a channel above 3, src/func_02060918.c skips the DMA-channel
   stop and src/func_02060b64.c's DMA test is false, so every card read takes
   the CPU path -- ROMCTRL and the data port, which ntr/card.cpp models -- with
   no DMA and no IRQ 19. FS_Init(2) in the ROM's order, with card-timed DMA
   and IRQ 19 delivered, needs a DMA model first; it is stage B, not built.

   And FS_Init runs from a static initialiser, before main, which is why its
   func_02060890 call is renamed onto a no-op (port/CMakeLists.txt): the ROM's
   own bring-up runs later, inside main. The first open-by-name in the game is
   inside a scene's InitResources, well after both. */
struct NitroFsNamesBoot {
    NitroFsNamesBoot()
    {
        const char *probe;
        /* THE MIRROR FIRST, AND FROM HERE RATHER THAN FROM ITS OWN STATIC
           INITIALISER. The cartridge-header words have to be in place before
           the ROM's FS_Init reads them, and C++ says nothing about the order
           of two translation units' initialisers -- so the seed is an ordinary
           call from this one, two lines above the body that needs it. The
           read-back after the guard is the proof that the ROM's own
           func_0205cb68 laid them into the archive record. */
        port_nitrofs_header_mirror_seed();
        func_0205d89c(-1);
        port_nitrofs_boot_report();
        /* The read trace's tap (traced_read above): both read slots, only
           under SM64DS_NFS_TRACE, and only when they hold the one pointer
           FS_Init registered. */
        if (trace_on() && data_020a8074[15] && data_020a8074[15] == data_020a8074[17]) {
            g_traced_read = (ArchiveRead)(size_t)data_020a8074[15];
            data_020a8074[15] = data_020a8074[17] = (int)(size_t)&traced_read;
        }
        /* SM64DS_NFS_PROBE=1 runs the cross-seam check at boot instead of
           waiting for a caller. It exists because the useful comparison is
           between THIS path and the game's, and the game's dies later in the
           run: the probe calls func_0205d644 with both arguments spelled out
           from C++, so a name that resolves here and fails through the game's
           own caller isolates the fault to the caller. HERE only on
           smoke_player's synchronous face; the card targets probe after the
           card bring-up (port_nitrofs_probe_after_boot above). */
        probe = getenv("SM64DS_NFS_PROBE");
        if (g_sync_face && probe && *probe && strcmp(probe, "0"))
            port_nitrofs_report();
    }
};
NitroFsNamesBoot nitro_fs_names_boot;

}  /* anonymous namespace */
