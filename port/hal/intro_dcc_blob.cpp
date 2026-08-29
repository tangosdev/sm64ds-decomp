/* data_02089dcc, hosted rather than byte-copied, with its ONE code
 * pointer seated to a trap.
 *
 * port/tools/ptr_audit.py flagged this one and it was RIGHT: 788 bytes,
 * one relocation, and that relocation points at CODE --
 *     from:0x02089f4c to:0x020c020c (function)
 * i.e. func_ov007_020c020c, which is NOT hosted and lives in ov007,
 * territory this lane is barred from touching. Byte-copying it through
 * romdata.py would put a raw DS address in a word that gets CALLED, which
 * is precisely the data_020876e4 crash that file's header banks.
 *
 * This lane classified it as a value word on the reasoning that its target
 * had no symbol; that reasoning was WRONG and ptr_audit has better type
 * information. The audit's other five value-word verdicts it CONFIRMS:
 * data_02089af8 (the third opening script) reports 6 relocations and ZERO
 * code pointers, which is exactly the record-decode argument.
 *
 * THE POINTER IS SEATED TO A TRAP, not to a DS address and not to a
 * silently wrong host function. data_02089dcc is NOT in the opening's
 * four-script chain (020890a0 -> 02088a94 -> 02089af8 -> 020876d4) -- the
 * same sinit patches it for a different cutscene -- so the opening never
 * reaches this word, and anything that does now says what it needs.
 */
#include <cstdio>
#include <cstdlib>
#include "dsstate_seg.h"

extern "C" {

static void intro_dcc_trap(void)
{
    std::fprintf(stderr,
                 "UNHOSTED: data_02089dcc+0x180 calls func_ov007_020c020c, which is not hosted. It is ov007 territory and outside the intro-cutscene lane's grant; the opening does not reach this word. See hal/intro_dcc_blob.cpp.\n", 384);
    std::abort();
}

DSSTATE_BEGIN
__declspec(align(8)) unsigned char data_02089dcc[788] = {
    9,4,0,0,255,255,3,244,9,10,7,0,0,0,0,73,
    0,0,0,8,46,88,2,88,2,21,7,15,4,0,0,0,
    0,6,0,0,0,0,0,0,0,0,13,4,0,0,255,255,
    1,187,2,244,6,17,5,15,4,244,1,255,255,6,0,0,
    0,0,0,0,0,0,15,0,0,0,0,0,0,0,0,160,
    3,92,3,0,128,15,1,0,0,0,0,0,140,0,160,3,
    92,3,0,128,15,2,0,0,0,0,0,240,0,160,3,32,
    3,0,160,15,3,0,0,0,0,0,116,255,160,3,188,2,
    0,64,15,19,0,0,0,0,14,0,0,160,3,190,2,0,
    0,8,10,0,0,30,0,18,0,10,8,0,0,0,0,14,
    1,0,0,8,10,30,0,100,0,19,0,10,8,60,0,60,
    0,15,1,0,0,8,10,150,0,200,0,20,0,10,8,150,
    0,150,0,17,1,0,0,8,0,0,0,0,0,10,5,8,
    1,0,0,0,0,10,5,8,2,0,0,0,0,10,5,8,
    3,0,0,0,0,10,5,8,19,120,0,120,0,12,10,9,
    3,199,0,229,0,11,0,128,8,3,230,0,230,0,10,5,
    8,19,239,0,239,0,12,11,15,19,239,0,239,0,14,0,
    0,176,3,113,2,0,128,15,19,240,0,86,2,15,226,255,
    232,3,224,252,0,64,15,0,24,1,86,2,1,60,0,232,
    3,224,252,0,9,15,1,44,1,86,2,1,60,0,232,3,
    224,252,0,9,9,2,74,1,83,1,11,0,128,15,2,84,
    1,86,2,1,60,0,232,3,224,252,0,9,8,3,94,1,
    94,1,10,14,8,10,204,1,244,1,12,0,10,8,204,1,
    204,1,18,1,0,0,8,3,204,1,204,1,10,15,7,3,
    12,2,12,2,12,15,3,12,2,86,2,1,116,255,232,3,
    224,252,0,16,7,1,87,2,87,2,13,7,2,87,2,87,
    2,13,7,3,87,2,87,2,13,7,19,87,2,87,2,19,
    20,26,88,2,88,2,10,156,255,156,255,68,253,0,0,0,
    0,0,241,25,20,27,88,2,88,2,10,6,255,255,0,56,
    255,0,0,0,0,0,236,25,20,28,88,2,88,2,10,156,
    255,155,0,168,253,0,0,0,0,0,251,35,20,29,88,2,
    88,2,10,250,0,200,0,80,251,0,0,0,0,0,249,25,
    20,30,88,2,88,2,10,6,255,255,0,68,253,0,0,0,
    0,0,0,25,20,31,96,2,96,2,10,156,255,156,255,68,
    253,0,0,0,0,0,241,25,20,32,96,2,96,2,10,6,
    255,255,0,56,255,0,0,0,0,0,236,25,20,33,96,2,
    96,2,10,156,255,155,0,168,253,0,0,0,0,0,251,35,
    20,34,96,2,96,2,10,250,0,200,0,80,251,0,0,0,
    0,0,249,25,8,26,88,2,255,255,11,1,8,27,88,2,
    255,255,11,1,8,28,88,2,255,255,11,1,8,29,88,2,
    255,255,11,1,8,30,88,2,255,255,11,1,8,31,96,2,
    255,255,11,1,8,32,96,2,255,255,11,1,8,33,96,2,
    255,255,11,1,8,34,96,2,255,255,11,1,8,35,96,2,
    255,255,11,1,8,36,112,2,255,255,11,1,8,37,112,2,
    255,255,11,1,8,38,112,2,255,255,11,1,8,39,112,2,
    255,255,11,1,8,40,112,2,255,255,11,1,8,41,198,2,
    255,255,11,1,8,42,198,2,255,255,11,1,8,43,198,2,
    255,255,11,1,8,44,198,2,255,255,11,1,8,45,198,2,
    255,255,11,1,13,11,32,3,32,3,6,0,1,0,0,0,
    0,0,0,0,
};
DSSTATE_END

/* The one seated word. Done at runtime rather than in the initialiser
   because the trap's address is not a compile-time constant an array
   initialiser can take at file scope in C. Runs before anything can read
   it: hal/level_boot.cpp's intro seam calls it alongside the sinit. */
void port_intro_seat_dcc(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    unsigned *w = (unsigned *)(data_02089dcc + 384);
    if (*w != 0x020c020cu) {
        std::fprintf(stderr, "FATAL: data_02089dcc+0x180 holds %08x, the ROM says 020c020c -- WRONG BYTES\n", *w);
        std::abort();
    }
    *w = (unsigned)(size_t)&intro_dcc_trap;
}

}  /* extern "C" */
