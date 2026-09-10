// ---------------------------------------------------------------------------
// snd_globals.cpp -- the storage SND_Init writes, in the ROM's own layout.
//
// Run link100, lane SND1, rung R1. hal/sdat/consumer.cpp used to seed the
// sound command queue by hand and said why:
//
//     "that function is unusable on the host because it depends on three
//      symbols being ADJACENT in DS memory ... Host symbols are separate
//      objects, so running it would write past two of them."
//
// That was true of the storage, not of the function. src/func_0205b358.c is
// SND_Init and the three adjacencies it uses are the ROM's own object split
// three ways by dsd, exactly like the card driver's CARDiCommon in
// hal/globals_link100.cpp. Host them as ONE run with the three names at their
// ROM offsets and the ROM's body runs unchanged.
//
// THE RUN, and what decides each size. Deltas from config/arm9/symbols.txt;
// every one is kind:bss, so zeroed host storage reads what the DS's cleared
// BSS reads.
//
//   +0x0000  data_020a6760  0x1000   the first 0xaa-and-a-bit nodes of the
//                                    256 x 0x18 command pool. func_0205b070
//                                    cache-flushes the whole thing as
//                                    `(&data_020a6760, 0x1800)`, which is the
//                                    ROM saying in one line that the object is
//                                    0x1800 bytes and starts here.
//   +0x1000  data_020a7760  0x07e8   the rest of the pool. src/func_0205b358.c
//                                    writes `*(int *)(data_020a7760 + 0x7e8) = 0`
//                                    -- pool[255].next -- and 0x1000 + 0x7e8 is
//                                    0x17e8 = 255 * 0x18 exactly.
//   +0x17e8  data_020a7f48  0x0018   THE LAST NODE. func_0205b358 seats the
//                                    free-list TAIL as `data_020a6494 =
//                                    &data_020a7f48`, so this name has to be
//                                    &pool[255] or the tail pointer addresses a
//                                    stranger and func_0205b274's splice writes
//                                    a batch head into it.
//   +0x1800  data_020a7f60  0x0060   the 8 x 0xc channel-7 callback table
//                                    func_0205b524 clears and func_0205b470
//                                    dispatches. Nothing in the ROM's code
//                                    spans the pool/table boundary, so this
//                                    member is in the run for fidelity rather
//                                    than necessity -- and being in it is how
//                                    the check below can state the boundary
//                                    instead of assuming it.
//
// THE THREE SINGLETONS beside it, each hosted at its own ROM span:
//
//   data_020a64e0  0x280  SNDSharedWork, the block the ARM9 hands the ARM7 in
//                         command 0x19 and then reads back. func_0205b554
//                         clears it (a header, 16 x 0x24 records and a 16-entry
//                         halfword tail, 0x280 exactly, which is also the span
//                         to data_020a6760). WORD 0 is the batch progress
//                         counter func_0205b5d4 returns and WORD 1 the player
//                         bitmask func_0205b608 returns; hal/sdat/consumer.cpp
//                         publishes both INTO THIS OBJECT now, because the ROM
//                         is what tells it where the object is.
//   data_020a4d44  4      func_0204f070's once-only guard.
//   data_020a6480  4      func_0205a82c's once-only guard.
//
// WHY .dsstate AND WHY THIS SUFFIX. All of it is mutable DS state a save state
// has to roll back (hal/dsstate_seg.h). ".dsstate$yzsnd00" and up sort after
// lane WM3's ".dsstate$ywme00" -- fourth character, 'z' > 'w' -- and before the
// "$zzz" high sentinel, so the captured span grows AT ITS TAIL and not one
// existing hosted global moves a byte.
//
// ALIGNMENT. align(32) on the head so the pool is comfortably word-aligned (it
// is a linked list of pointers), align(1) on every member after it so the
// linker inserts no padding between them. Each size is a multiple of four, so
// four-byte alignment carries through the whole run on its own. That is the
// property port_snd_pool_check() reads back rather than reasons about.
// ---------------------------------------------------------------------------

#include <stdio.h>

/* One grouped run. MSVC merges same-named sections in order of the text after
   the '$', so $yzsnd00..$yzsnd03 land contiguously and in that order. This is
   the idiom hal/globals_link100.cpp's card pack and hal/wm_arm7.cpp's WMBSS
   block use. */
#define SND1_RUN(sec, name, size, algn)                           \
    __pragma(section(sec, read, write))                           \
    extern "C" __declspec(allocate(sec)) __declspec(align(algn))  \
    unsigned char name[size] = {0}

/* The 0x1800 command pool, then the callback table the ROM keeps behind it. */
SND1_RUN(".dsstate$yzsnd00", data_020a6760, 0x1000, 32);
SND1_RUN(".dsstate$yzsnd01", data_020a7760, 0x07e8, 1);
SND1_RUN(".dsstate$yzsnd02", data_020a7f48, 0x0018, 1);
SND1_RUN(".dsstate$yzsnd03", data_020a7f60, 0x0060, 1);

/* SNDSharedWork and the two once-only guards. Their neighbours are hosted
   elsewhere, so each is its own object at its own ROM span. */
SND1_RUN(".dsstate$yzsnd04", data_020a64e0, 0x0280, 32);
SND1_RUN(".dsstate$yzsnd05", data_020a4d44, 4, 4);
SND1_RUN(".dsstate$yzsnd06", data_020a6480, 4, 4);

/* ---------------------------------------------------------------------------
 * RUNGS R8 / R9 / R10: the four bss objects the ROM's own sound FRAME names,
 * and the seven MSVC spellings the decomp's C++ TUs ask for.
 *
 * Each is its own object at its own ROM span -- their neighbours are hosted
 * elsewhere and no body addresses across a boundary, which was checked rather
 * than assumed. Deltas from config/arm9/symbols.txt, all kind:bss.
 *
 *   data_020a552c  0x0c  the STRM voice list. func_0204fda4 hands it to
 *                        NestedHeapIterator::Remove, which is the 0xc-byte
 *                        iterator shape (next, prev, count) the whole sound
 *                        stack uses; span to data_020a5538 agrees.
 *   data_020a5600  0x14  the ARM9 sound thread's message queue. func_020502b8
 *                        waits on it through func_020587e4 when a stream stops.
 *                        Nothing in this build ever posts to it (rung R5 is the
 *                        thread), so the wait is never entered; the object is
 *                        here because the reference is.
 *   data_020a5bbc  0x0c  the free STRM-buffer list func_020520a4 appends to.
 *   data_020a5bc8  0x0c  the in-use STRM-buffer list func_0205212c walks.
 *
 * SEVEN NAMES THE DECOMP SPELLS IN C++, and why an alias rather than a second
 * definition. src/func_0204fda4.cpp, src/func_0205212c.cpp and
 * src/func_020132d8.cpp carry the //cpp marker, and each declares some of its
 * externs OUTSIDE its own extern "C" block -- so MSVC mangles those references
 * (?data_0209b480@@3EA and friends) while the storage is published under the C
 * spelling by hal/player_bridges.cpp, hal/cxx_aliases.cpp, hal/auto_bss.cpp,
 * hal/actor_vtables.cpp and this file. One object, two spellings, which is
 * exactly what /alternatename is for and what hal/actor_classes_bbh.cpp's
 * data_02082128 row and hal/actor_base_tables_ov002.cpp's two SharedFilePtr
 * rows already do. port/tools/alternatename_guard.py checks every one of them
 * fired (LHS and RHS at the same address) on every build.
 *
 * NestedHeapIterator::Next is the same fact about a METHOD:
 * src/func_0205212c.cpp declares it returning HeapAllocator*, the matched TU
 * _ZN18NestedHeapIterator4NextEP13HeapAllocator.cpp publishes it returning int,
 * and MSVC mangles the return type into the name. Same body, same address.
 * --------------------------------------------------------------------------- */
SND1_RUN(".dsstate$yzsnd07", data_020a552c, 0x0c, 4);
SND1_RUN(".dsstate$yzsnd08", data_020a5600, 0x14, 4);
SND1_RUN(".dsstate$yzsnd09", data_020a5bbc, 0x0c, 4);
SND1_RUN(".dsstate$yzsnd10", data_020a5bc8, 0x0c, 4);

#pragma comment(linker, "/alternatename:?data_020a552c@@3PAHA=_data_020a552c")
#pragma comment(linker, "/alternatename:?data_020a5bc8@@3UNestedHeapIterator@@A=_data_020a5bc8")
#pragma comment(linker, "/alternatename:?data_0209b480@@3EA=_data_0209b480")
#pragma comment(linker, "/alternatename:?data_0209b4a0@@3HA=_data_0209b4a0")
#pragma comment(linker, "/alternatename:?data_0209b4b0@@3HA=_data_0209b4b0")
#pragma comment(linker, "/alternatename:?data_0209b53c@@3HA=_data_0209b53c")
#pragma comment(linker, "/alternatename:?Next@NestedHeapIterator@@QAEPAUHeapAllocator@@PAU2@@Z=?Next@NestedHeapIterator@@QAEHPAUHeapAllocator@@@Z")

/* The layout the ROM's body depends on, read back rather than assumed. Returns
   non-zero if anything moved, and says which name and by how much. */
extern "C" int port_snd_pool_check(void)
{
    static const struct { const unsigned char *p; int want; const char *n; } k[] = {
        { data_020a6760, 0x0000, "data_020a6760" },
        { data_020a7760, 0x1000, "data_020a7760" },
        { data_020a7f48, 0x17e8, "data_020a7f48" },
        { data_020a7f60, 0x1800, "data_020a7f60" },
    };
    int bad = 0;
    for (int i = 0; i < 4; ++i) {
        if (k[i].p - data_020a6760 != k[i].want) {
            fprintf(stderr, "  [snd1] COMMAND POOL BROKEN: %s at +0x%x, ROM "
                    "says +0x%x\n", k[i].n,
                    (unsigned)(k[i].p - data_020a6760), k[i].want);
            bad = 1;
        }
    }
    /* The pool is a linked list of pointers and the status block is read as
       words, so both have to be word-aligned wherever the linker put them. */
    if (((unsigned)(size_t)data_020a6760 & 3) != 0) {
        fprintf(stderr, "  [snd1] COMMAND POOL BROKEN: base %p is not "
                "word-aligned\n", (void *)data_020a6760);
        bad = 1;
    }
    if (((unsigned)(size_t)data_020a64e0 & 3) != 0) {
        fprintf(stderr, "  [snd1] STATUS BLOCK BROKEN: %p is not "
                "word-aligned\n", (void *)data_020a64e0);
        bad = 1;
    }
    return bad;
}

namespace {
struct SndPoolCheck { SndPoolCheck() { port_snd_pool_check(); } };
SndPoolCheck g_snd_pool_check;
}  /* anonymous namespace */
