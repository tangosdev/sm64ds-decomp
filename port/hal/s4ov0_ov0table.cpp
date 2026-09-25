// ---------------------------------------------------------------------------
// s4ov0_ov0table.cpp -- the two words the cartridge's ov0 handle table leaves
// behind, and a report of what the ROM built into them.
//
// Run linkfull, lane S4OV0. The game asks for most files by an ov0 HANDLE and
// src/func_02018a24.c turns a handle into a FAT file id:
//
//     return i < 0x8000 ? data_0209d3b8[i] : i;
//
// data_0209d3b8 is a pointer to a u16 table on the heap and data_0209d3b0 is
// its entry count. Both are written once, at boot, by overlay 0's entry
// (src/func_ov000_020aa420.c through src/func_02018a58.cpp): 0x80a entries,
// 4116 bytes off the default heap, each one the file id of one of ov000's 2058
// path strings. hal/boot_os.cpp's port_boot_rom_worker() runs that entry.
//
// THEY ARE DS STATE, so they are inside the .dsstate bracket a save state
// captures: the heap block the pointer names is in the arena the save state
// captures too, and a restore that brought back one without the other would
// hand every handle lookup a table from another boot. Each is one word, which
// is its whole ROM span (config/arm9/symbols.txt: data_0209d3b0, data_0209d3b4,
// data_0209d3b8, data_0209d3bc are four consecutive words). data_0209d3b4 is
// hal/card_mount.cpp's and data_0209d3bc is hal/fs.cpp's.
// ---------------------------------------------------------------------------

#include <chrono>
#include <cstdio>
#include <cstdlib>

#include "dsstate_seg.h"

extern "C" {

DSSTATE_BEGIN
unsigned short *data_0209d3b8;   /* 0x0209d3b8: the handle -> file id table */
int data_0209d3b0;               /* 0x0209d3b0: its entry count, 0x80a       */
DSSTATE_END

/* The default heap the table came out of: Memory::Allocate with no heap uses
   this word (src/_ZN6Memory8AllocateEjiP4Heap.cpp), and at the ROM's boot
   worker it is still the root heap Heap::SetupRootHeap installed. */
extern void *data_020a0ea0;

/* A wall clock for the one line below; nothing the game reads. */
double port_ov0_table_clock_ms(void)
{
    using namespace std::chrono;
    return duration<double, std::milli>(
               steady_clock::now().time_since_epoch()).count();
}

/* One line on every boot, so a captured run says what the ROM built and how
   long it took. SM64DS_OV0_TABLE_DUMP=<path> also writes the whole table, one
   "handle<TAB>file id" row per entry, for a comparison against the catalog
   (build/assets/handles.tsv) that the host face used to answer from. */
void port_ov0_table_report(double ms)
{
    int nonzero = 0;
    const unsigned short *t = data_0209d3b8;
    for (int i = 0; t && i < data_0209d3b0; ++i)
        if (t[i])
            ++nonzero;
    std::fprintf(stderr,
                 "[boot] ov0 handle table: %d entries at %p (%d non-zero) off "
                 "the default heap %p, built in %.1f ms\n",
                 data_0209d3b0, (const void *)t, nonzero, data_020a0ea0, ms);

    const char *path = std::getenv("SM64DS_OV0_TABLE_DUMP");
    if (!path || !*path || !t)
        return;
    std::FILE *f = std::fopen(path, "w");
    if (!f) {
        std::fprintf(stderr, "[boot] SM64DS_OV0_TABLE_DUMP: cannot open %s\n",
                     path);
        return;
    }
    std::fprintf(f, "handle\tfile_id\n");
    for (int i = 0; i < data_0209d3b0; ++i)
        std::fprintf(f, "%d\t%u\n", i, (unsigned)t[i]);
    std::fclose(f);
    std::fprintf(stderr, "[boot] SM64DS_OV0_TABLE_DUMP: %d rows written\n",
                 data_0209d3b0);
}

}  // extern "C"
