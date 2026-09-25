// The ROM's card mount table, hosted -- and the ROM's own archive loader
// running on top of it.
//
// Run link100, lane CARDFS, hosted data_0208ecf4 and linked src/LoadArchive.c
// against it; lane CARDFS2 hosted the entries' two DS strings. Until run
// linkfull, lane S4ARC, this file then PUBLISHED all thirteen archives from a
// static initialiser: it filled every entry's residency word with a 0x60-byte
// stand-in object pointing into hal/fs.cpp's own copy of the NARC, so the ROM's
// LoadArchive always found its archive resident and its mount branch never ran,
// and the matching UnloadArchive / UnloadArchives were empty faces (the ROM's
// bodies would have freed static storage to a null heap). That publish is
// RETIRED. What stood in its way is written down below, with what answered it.
//
// ---- WHAT THE CARTRIDGE DOES ----------------------------------------------
//
// src/LoadArchive.c asks "is archive N resident?" by reading one word, and
// mounts it when it is not:
//
//     struct Entry *e = &data_0208ecf4[idx];
//     if (e->field_0 == 0) {
//         e->field_4 = data_020a0ea0;                  the default heap
//         e->field_0 = func_02018934(e->field_c, e->field_10, e->field_4);
//     }
//
// func_02018934 opens the NARC by its path (func_02018d98 -> func_0205d518),
// allocates the whole image plus a 0x60-byte archive object off that heap from
// its tail (Heap::_Allocate, alignment -0x10), reads the image in, and hands it
// to func_0204ee40, which walks the NARC's chunks, stores the image, BTAF and
// file-base words at +0x50 / +0x54 / +0x58, registers the archive under its
// three-character name (func_0205cc80) and loads it as a NitroSDK memory
// archive (func_0205cb68). Unmounting is the mirror: src/UnloadArchive.c and
// src/UnloadArchives.c call func_02018908, which unloads the archive
// (func_0204ee10: func_0205caa8 then func_0205cbe4) and frees the block back to
// the heap word the mount wrote at +0x04.
//
// Who mounts, on the cartridge: Stage::InitResources for a level (archive 0,
// the language text pair through LoadTextNarcs, archive 7 on the GAME heap for
// the castle grounds, one VS archive for a VS map), the scenes' own
// InitResources, and func_020185c0 on a read of an archive member whose archive
// is not resident -- it mounts, notes the index in data_0208eb54, and the
// reader's tail (src/func_02018770.c) unmounts it again after the read. The
// title and the DS theatre unmount everything (UnloadArchives).
//
// ---- WHAT THIS FILE HOLDS NOW ---------------------------------------------
//
// The table itself and its ROM .data values, nothing else. The id range at
// +0x08 / +0x0a comes from port_archive_map, which port/tools/romdata.py reads
// out of these exact halfwords in the arm9 image, and the two string pointers
// at +0x0c / +0x10 from port_archive_name / port_archive_path, which that
// generator resolves BY ADDRESS out of the same table to the hosted ROM strings
// (twenty-six NAMED rows; entry 0's short name is the second symbol in the name
// run, and the paths are not in entry order either, so nothing here is typed).
// +0x00 and +0x04 are the ROM's own initial values, zero: the table ships
// zeroed there and the loader is what fills them. The mounted objects are the
// ROM's, on the ROM's heap, and the table is inside .dsstate with them (the
// arena is part of every save state, and so are NitroSDK's archive list,
// data_020a8048 / data_020a804c in hal/scene_boot.cpp, and the "rom" archive
// record in hal/fs_names.cpp), so a restore rolls the mount state back whole.
//
// ---- WHAT THE PUBLISH'S BANNER SAID BLOCKED THE MOUNT, AND THE ANSWERS -----
//
//   THE ARITIES. src/func_02018d98.c took no parameters and passed none on,
//     and Heap::_Allocate's veneer was a void() calling a void(). Both are
//     re-spelled in src/ now: func_02018d98(void *file, const char *path)
//     passes both to func_0205d518, and src/_ZN4Heap9_AllocateEji.cpp forwards
//     (size, align) to Heap::Allocate and returns its block, reached from the C
//     name through a port/faces_sync.txt R row onto the real member. The last
//     ride-through on this path, src/func_02018770.c calling UnloadArchive()
//     with nothing where the ARM leaves the index in r0, is re-spelled on main
//     (PR #3130) and synced here.
//   THE MEMORY MODEL. The publish was eager and permanent, and running the
//     ROM's branch under it would have put every image on the game's heap for
//     good. With the publish gone the lifetime is the ROM's: the level boot
//     replays Stage::InitResources' archive lines (hal/level_boot.cpp), so a
//     level's reads find their archive resident instead of mounting and
//     unmounting a whole NARC per read, and Stage::CleanupResources' archive-7
//     release rides the level teardown there too.
//   THE ORDER OF REGISTRATION. func_0205cc80 makes the first archive it ever
//     registers the current directory. hal/fs_names.cpp registers "rom" from
//     FS_Init, which the boot runs at the ROM-order bring-up (lane S4CARD)
//     before main's loop starts any scene, so every mount appends behind it.
//     The audit below prints the list and the current directory to prove it.
//
// hal/fs.cpp keeps its own copy of each NARC for the host readers that are
// still faces (SharedFilePtr::Load, LoadFile, func_0201817c: route R3's). It
// never read this table and does not now.
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

/* ---- the two hosted DS globals ------------------------------------------ */

DSSTATE_BEGIN
extern "C" {

/* 0x0208ecf4, thirteen entries of 0x14 bytes: the whole 0x104-byte span to the
   next dsd symbol at 0x0208edf8. */
struct PortCardMount {
    void *f0;           /* the mounted archive object; 0 means not mounted */
    void *f4;           /* the heap the mount came off, written by LoadArchive */
    u16 f8;             /* first interior file id */
    u16 fa;             /* one past the last */
    const char *fc;     /* three-character short name (hosted ROM string) */
    const char *f10;    /* NARC path (hosted ROM string) */
};
PortCardMount data_0208ecf4[13];

/* 0x0209d3b4, one bss word: the archive src/func_02018598.c pairs with a file
   id to make the FSFileID that opens a loose file; src/func_020189f0.c writes
   it while ov000's entry builds the handle table. Four bytes by the delta to
   data_0209d3b8, and its ROM value is zero because it is bss. */
int data_0209d3b4;

}  /* extern "C" */
DSSTATE_END

/* NitroSDK's archive list head and current directory (hal/scene_boot.cpp),
   read by the audit only. An FSArchive's first word is its packed name and its
   second the next archive (src/func_0205cc80.c). */
extern "C" void *data_020a8048;
extern "C" void *data_020a804c[3];

/* The root heap (Heap::SetupRootHeap's data_020a0e9c) and the game heap
   (data_020a0eac), for the audit's free-space pair: both are ExpandingHeaps,
   whose allocator is the word at +0x14 (include/Heap.h), the same read the
   boot's own "[heap] game heap" line makes. */
extern "C" void *data_020a0e9c;
extern "C" void *data_020a0eac;
extern "C" unsigned _ZN22ExpandingHeapAllocator10MemoryLeftEv(void *self);

static unsigned heap_left(void *heap)
{
    void *alloc = heap ? *(void **)((char *)heap + 0x14) : 0;
    return alloc ? _ZN22ExpandingHeapAllocator10MemoryLeftEv(alloc) : 0;
}

static int g_audit_registered;

static void arc_name(const void *arc, char out[4])
{
    u32 packed = arc ? *(const u32 *)arc : 0;
    int k;
    for (k = 0; k < 3; ++k) {
        char c = (char)((packed >> (8 * k)) & 0xff);
        out[k] = (c >= 0x21 && c < 0x7f) ? c : '.';
    }
    out[3] = 0;
}

/* ONE LINE OF STATE: which entries are resident, each with the heap word the
   ROM's mount wrote, and NitroSDK's archive list in order with the current
   directory. Printed by the audit at exit and by the level boot's replay
   points (hal/level_boot.cpp) when SM64DS_CARDFS is set. */
extern "C" void port_card_mount_snapshot(const char *where)
{
    char line[1024];
    int n = 0, i, resident = 0, heaped = 0, named = 0, guard = 0;
    const void *a;
    char nm[4];

    for (i = 0; i < 13; ++i) {
        const PortCardMount *e = &data_0208ecf4[i];
        if (e->f0) {
            resident++;
            if (e->f4)
                heaped++;
            n += snprintf(line + n, sizeof line - n, " %d:%s@%p/h%p", i,
                          e->fc ? e->fc : "?", e->f0, e->f4);
        }
        if (e->fc && e->f10 && e->fc[3] == 0 && e->f10[0] == '/')
            named++;
    }
    n += snprintf(line + n, sizeof line - n, " | fs archives:");
    for (a = data_020a8048; a && guard < 32; a = *((void *const *)a + 1), ++guard) {
        arc_name(a, nm);
        n += snprintf(line + n, sizeof line - n, " %s", nm);
    }
    arc_name(data_020a804c[0], nm);
    fprintf(stderr, "[cardfs] %s: %d/13 resident, %d of them with a ROM-written "
            "heap word, %d/13 carry both ROM strings;%s | cwd %s | free: root "
            "%u game %u\n",
            where, resident, heaped, named, n ? line : "", nm,
            heap_left(data_020a0e9c), heap_left(data_020a0eac));
    fflush(stderr);
}

static void port_card_mount_audit(void)
{
    port_card_mount_snapshot("exit audit");
}

/* THE ROM'S .data VALUES, written before any game code runs -- the host's
   stand-in for the image load that puts them there on the cartridge. The ids
   and the string pointers are generated const data (romdata.c); the two
   runtime words stay zero. Nothing is mounted here: the first mount is the
   ROM's, on the ROM's heap, when a scene or the level boot asks for one. */
static struct PortCardMountSeed {
    PortCardMountSeed()
    {
        int i;
        for (i = 0; i < 13; ++i) {
            data_0208ecf4[i].f0 = 0;
            data_0208ecf4[i].f4 = 0;
            data_0208ecf4[i].f8 = port_archive_map[i].base;
            data_0208ecf4[i].fa = port_archive_map[i].end;
            data_0208ecf4[i].fc = port_archive_name[i];
            data_0208ecf4[i].f10 = port_archive_path[i];
        }
        if (getenv("SM64DS_CARDFS") && !g_audit_registered) {
            g_audit_registered = 1;
            atexit(port_card_mount_audit);
        }
    }
} g_card_mount_seed;
