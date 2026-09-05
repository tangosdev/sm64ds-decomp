// ---------------------------------------------------------------------------
// globals_link100.cpp -- the DS card driver's control block, hosted as the ONE
// object the ROM has instead of the three dsd calls it a name apiece.
//
// WHY THIS FILE EXISTS. hal/boot_os.cpp refuses func_02042f68, the game-card
// bring-up src/func_0201a054.c calls, and the refusal it wrote down is the
// right bug with the wrong boundary:
//
//     "symbols.txt gives data_020a81bc a span of 1444 bytes ending exactly at
//      data_020a8760, so the thread record and its 1KB stack are one
//      contiguous bss object. Two separate host globals do not reproduce
//      that... Hosting data_020a81bc..data_020a8760 as ONE span."
//
// The span is right at the TOP and short at the BOTTOM. Measured off
// extracted/arm9_dec.bin at base 0x02004000, not off the decompilation:
//
//   02060034  ldr  r3, =0x020a8180        func_0206002c's own base register
//   02060080  strh r2, [r0, #0xd4]        r0 = 0x020a8180 -> writes 0x020a8254
//
// 0x020a8254 is data_020a81bc + 0x98. The head of the block and the name below
// it are the SAME ROM object: NitroSDK's CARDiCommon, with the card thread's
// OSThread record embedded in it at +0x3c and the thread's 1 KB stack as its
// last member. dsd split it at every offset some function LDR'd, which is the
// same thing it did to the OS thread-info record (see the link100 GLOBALS
// block in hal/cxx_aliases.cpp), and the same reason a plain resize is not the
// fix.
//
// THE THREE NAMES AND WHAT DECIDES EACH SIZE. Deltas from
// config/arm9/symbols.txt; every one is .bss, so zeroed host storage reads
// what the DS's cleared BSS reads.
//
//   data_020a8180   0x3c = 60      the CARDiCommon head. func_0206002c writes
//                                  +0x00 (-> data_020a8160), +0x08, +0x0c,
//                                  +0x10, +0x14, +0x38 and +0xd4, and only the
//                                  first six of those are inside its own 60.
//   data_020a81bc   0x5a4 = 1444   the card thread's OSThread record (0x94 --
//                                  func_02058200's last store in the object it
//                                  is handed is at +0x90) and, 1 KB below the
//                                  top of the run, that thread's stack.
//   data_020a8760   0x04           the stack TOP, and this is the one
//                                  hal/boot_os.cpp asked to have checked. It
//                                  has to be IN the group. func_0206002c calls
//
//                                    func_02058200(&data_020a81bc, func_020602bc,
//                                                  0, &data_020a8760, 0x400, 4)
//
//                                  and func_02058200 takes the fourth argument
//                                  as the top of a DOWNWARD stack:
//
//                                    02058270  sub r5, r6, r0     base = end - 0x400
//                                    0205828c  str r1, [r0, #-4]  magic at end-4
//                                    02058298  str r2, [r1]       magic at base
//                                    (then MultiStore_Int over base+4 .. end-8)
//
//                                  so the 1 KB it fills is [end - 0x400, end).
//                                  That is inside data_020a81bc's own 1444
//                                  bytes if and ONLY IF &data_020a8760 is
//                                  &data_020a81bc + 1444. Host it as a
//                                  standalone global anywhere else in the
//                                  image and func_02058200 writes a kilobyte
//                                  wherever the linker happened to put it.
//
// WHAT IS NOT IN THE GROUP, and it was checked rather than assumed.
// data_020a8764 is the neighbour above the top. func_02060364 is the only body
// that touches the pair and it writes them as two independent words --
//
//   02060370  ldr r2, =0x020a8760
//   02060374  ldr r1, =0x020a8764
//   02060378  str r0, [r2]          4 bytes at 0x020a8760
//   02060380  str r0, [r1]          4 bytes at 0x020a8764
//
// -- so nothing spans the boundary and data_020a8764 stays whatever hosts it.
// data_020a8160 (span 0x20) is likewise standalone: func_0206002c parks its
// address in the head and MultiStore32Bytes fills exactly its 32 bytes.
//
// NOTHING IN THE PORT REFERENCES THESE THREE YET. func_02042f68 and
// func_02060890 are still refused in hal/boot_os.cpp, on a reason that is not
// storage (the PXI seam and the ROM scheduler under func_02058048), and this
// file does not seat them. What it does is retire the storage half of that
// refusal, with the boundary the ROM actually has, so the lane that models the
// PXI side is not left re-deriving it -- and so the write at CARDiCommon +0xd4
// cannot land on a stranger the day someone does seat it.
//
// The group goes in .dsstate like every other hosted DS global: it is mutable
// DS state and the save state has to roll it back. align(1) keeps the ROM's
// spacing (align(4) on the members would pad 60 to 60 and 1444 to 1444, which
// happens to be harmless here, but the checked property is the OFFSET and it
// is checked rather than reasoned about).
// ---------------------------------------------------------------------------

#include <stdio.h>

/* One grouped run. MSVC merges same-named sections in order of the text after
   the '$', so $card00..$card02 land contiguously and in that order; $c sorts
   between the .dsstate$aaa and .dsstate$zzz sentinels hal/dsstate_seg.cpp
   defines, so the save state's section copy takes them. This is the idiom
   hal/scene_boot.cpp's L2 pack and hal/cxx_aliases.cpp's GX bank block use. */
#define LINK100_CARD(sec, name, size)                             \
    __pragma(section(sec, read, write))                           \
    extern "C" __declspec(allocate(sec)) __declspec(align(1))     \
    unsigned char name[size] = {0}

/* +0x0000  CARDiCommon head; its own writes reach +0xd4 */
LINK100_CARD(".dsstate$card00", data_020a8180, 60);
/* +0x003c  the card thread's OSThread record, then its 1 KB stack */
LINK100_CARD(".dsstate$card01", data_020a81bc, 1444);
/* +0x05e0  the stack TOP func_02058200 grows down from */
LINK100_CARD(".dsstate$card02", data_020a8760, 4);

extern "C" int port_link100_card_check(void)
{
    static const struct { const unsigned char *p; int want; const char *n; } k[] = {
        { data_020a8180,      0, "data_020a8180" },
        { data_020a81bc,   0x3c, "data_020a81bc" },
        { data_020a8760,  0x5e0, "data_020a8760" },
    };
    int bad = 0;
    for (int i = 0; i < 3; ++i)
        if (k[i].p - data_020a8180 != k[i].want) {
            fprintf(stderr, "  [link100] CARD PACK BROKEN: %s at +0x%x, ROM "
                    "says +0x%x\n", k[i].n,
                    (unsigned)(k[i].p - data_020a8180), k[i].want);
            bad = 1;
        }
    return bad;
}

namespace {
struct Link100CardCheck { Link100CardCheck() { port_link100_card_check(); } };
Link100CardCheck g_link100_card_check;
}  /* anonymous namespace */
