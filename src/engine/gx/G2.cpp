//cpp
/* G2 / G2S -- the 2D engines' BG base-address getters (Nitro SDK
 * namespaces, not classes). Owns arm9 .text 0x02054d38..0x020551f0: for
 * each of BG0..BG3, the screen-base and character-base pointer of the main
 * engine (G2, registers 0x04000000/0x04000008..0x0400000e, VRAM 0x06000000)
 * and of the sub engine (G2S, 0x04001000/0x04001008..0x0400100e, VRAM
 * 0x06200000). Each G2S getter sits directly below its G2 twin, so the file
 * is written ROM-descending (mwccarm emits .text in reverse source order):
 * G2 then G2S for BG0 screen through BG3 character.
 *
 * Only the main engine's getters need the display-control word's screen and
 * character offsets (bits 27-29 and 24-26 of 0x04000000); the sub engine
 * has fixed bases. BG2 and BG3 are mode-dependent: where the layer is
 * extended-affine (BG2 in mode 5, BG3 in modes 3-5), bit 7 of its control
 * word selects a bitmap, whose screen base counts in 16 KiB units and which
 * has no character base. BG3 has no layer at all in modes 6 and 7.
 *
 * Three of the main engine's character getters (BG0, BG1, BG3) keep their
 * address names under C linkage: their callers name them that way.
 *
 * G2 and G2S have no header, and most callers spell these symbols by their
 * mangled names. For G2 that is forced in any TU that includes
 * decl_common.h, whose `extern int G2[];` (a ROM data symbol) hides a
 * `namespace G2`. This file does not include it.
 */

namespace G2 {

// @symbol _ZN2G212GetBG0ScrPtrEv
void *GetBG0ScrPtr(void) {
    int off = ((*(volatile unsigned short *)0x4000008) & 0x1f00) >> 8;
    unsigned int sbase = (*(volatile unsigned int *)0x4000000) & 0x38000000;
    return (void *)(0x6000000 + ((sbase >> 0x1b) << 0x10) + (off << 11));
}

} // namespace G2

namespace G2S {

// @symbol _ZN3G2S12GetBG0ScrPtrEv
unsigned int GetBG0ScrPtr() {
    int v = *(volatile unsigned short *)0x4001008;
    return (((v & 0x1f00) >> 8) << 0xb) + 0x6200000;
}

} // namespace G2S

namespace G2 {

// @symbol _ZN2G212GetBG1ScrPtrEv
void *GetBG1ScrPtr(void) {
    int off = ((*(volatile unsigned short *)0x400000a) & 0x1f00) >> 8;
    unsigned int sbase = (*(volatile unsigned int *)0x4000000) & 0x38000000;
    return (void *)(0x6000000 + ((sbase >> 0x1b) << 0x10) + (off << 11));
}

} // namespace G2

namespace G2S {

// @symbol _ZN3G2S12GetBG1ScrPtrEv
unsigned int GetBG1ScrPtr() {
    int v = *(volatile unsigned short *)0x400100a;
    return (((v & 0x1f00) >> 8) << 0xb) + 0x6200000;
}

} // namespace G2S

namespace G2 {

// @symbol _ZN2G212GetBG2ScrPtrEv
/* Mode 6 (large bitmap) returns the engine's VRAM base itself; mode 7 and
 * anything else returns null. */
void *GetBG2ScrPtr(void) {
    unsigned int mode = *(volatile unsigned int *)0x4000000 & 7;
    unsigned short bg2cnt = *(volatile unsigned short *)0x400000c;
    unsigned int sbase = ((*(volatile unsigned int *)0x4000000 & 0x38000000) >> 0x1b) << 0x10;
    unsigned int off = ((unsigned int)(bg2cnt & 0x1f00)) >> 8;
    switch (mode) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        return (void *)(0x6000000 + sbase + (off << 11));
    case 5:
        if (bg2cnt & 0x80)
            return (void *)(0x6000000 + (off << 0xe));
        return (void *)(0x6000000 + sbase + (off << 11));
    case 6:
        return (void *)0x6000000;
    default:
        return (void *)0;
    }
}

} // namespace G2

namespace G2S {

// @symbol _ZN3G2S12GetBG2ScrPtrEv
unsigned int GetBG2ScrPtr() {
    int m = *(volatile int *)0x4001000 & 7;
    unsigned int v = *(volatile unsigned short *)0x400100c;
    unsigned int r1 = (v & 0x1f00) >> 8;
    switch (m) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        return (r1 << 0xb) + 0x6200000;
    case 5:
        if (v & 0x80)
            return (r1 << 0xe) + 0x6200000;
        return (r1 << 0xb) + 0x6200000;
    case 6:
        return 0;
    default:
        return 0;
    }
}

} // namespace G2S

namespace G2 {

// @symbol _ZN2G212GetBG3ScrPtrEv
void *GetBG3ScrPtr() {
    unsigned int mode = *(volatile unsigned int *)0x4000000 & 7;
    unsigned short bg3cnt = *(volatile unsigned short *)0x400000e;
    unsigned int sbase = ((*(volatile unsigned int *)0x4000000 & 0x38000000) >> 0x1b) << 0x10;
    unsigned int off = ((unsigned int)(bg3cnt & 0x1f00)) >> 8;
    switch (mode) {
    case 0:
    case 1:
    case 2:
        return (void *)(0x6000000 + sbase + (off << 11));
    case 3:
    case 4:
    case 5:
        if (bg3cnt & 0x80)
            return (void *)(0x6000000 + (off << 0xe));
        return (void *)(0x6000000 + sbase + (off << 11));
    case 6:
        return (void *)0;
    default:
        return (void *)0;
    }
}

} // namespace G2

namespace G2S {

// @symbol _ZN3G2S12GetBG3ScrPtrEv
unsigned int GetBG3ScrPtr() {
    int m = *(volatile int *)0x4001000 & 7;
    unsigned int v = *(volatile unsigned short *)0x400100e;
    unsigned int r1 = (v & 0x1f00) >> 8;
    switch (m) {
    case 0:
    case 1:
    case 2:
        return (r1 << 0xb) + 0x6200000;
    case 3:
    case 4:
    case 5:
        if (v & 0x80)
            return (r1 << 0xe) + 0x6200000;
        return (r1 << 0xb) + 0x6200000;
    case 6:
        return 0;
    default:
        return 0;
    }
}

} // namespace G2S

// @symbol func_02054efc
/* G2's BG0 character base. */
extern "C" void *func_02054efc(void) {
    int off = ((*(volatile unsigned short *)0x4000008) & 0x3c) >> 2;
    unsigned int sbase = (*(volatile unsigned int *)0x4000000) & 0x7000000;
    return (void *)(0x6000000 + ((sbase >> 0x18) << 0x10) + (off << 14));
}

namespace G2S {

// @symbol _ZN3G2S13GetBG0CharPtrEv
unsigned int GetBG0CharPtr() {
    int v = *(volatile unsigned short *)0x4001008;
    return (((v & 0x3c) >> 2) << 0xe) + 0x6200000;
}

} // namespace G2S

// @symbol func_02054ea8
/* G2's BG1 character base. */
extern "C" void *func_02054ea8(void) {
    int off = ((*(volatile unsigned short *)0x400000a) & 0x3c) >> 2;
    unsigned int sbase = (*(volatile unsigned int *)0x4000000) & 0x7000000;
    return (void *)(0x6000000 + ((sbase >> 0x18) << 0x10) + (off << 14));
}

namespace G2S {

// @symbol _ZN3G2S13GetBG1CharPtrEv
unsigned int GetBG1CharPtr() {
    int v = *(volatile unsigned short *)0x400100a;
    return (((v & 0x3c) >> 2) << 0xe) + 0x6200000;
}

} // namespace G2S

namespace G2 {

// @symbol _ZN2G213GetBG2CharPtrEv
void *GetBG2CharPtr(void) {
    int mode = *(volatile unsigned int *)0x4000000 & 7;
    unsigned int bg2cnt = *(volatile unsigned short *)0x400000c;
    if (mode < 5 || !(bg2cnt & 0x80))
        return (void *)(0x6000000 +
                        (((*(volatile unsigned int *)0x4000000 & 0x7000000) >> 0x18) << 0x10) +
                        (((bg2cnt & 0x3c) >> 2) << 14));
    return (void *)0;
}

} // namespace G2

namespace G2S {

// @symbol _ZN3G2S13GetBG2CharPtrEv
unsigned GetBG2CharPtr() {
    int v1 = *(volatile int *)0x4001000;
    unsigned short v2 = *(volatile unsigned short *)0x400100c;
    if (!((v1 & 7) >= 5 && (v2 & 0x80)))
        return ((unsigned)(v2 & 0x3c) >> 2 << 0xe) + 0x6200000;
    return 0;
}

} // namespace G2S

// @symbol func_02054d88
/* G2's BG3 character base. */
extern "C" int func_02054d88(void) {
    unsigned int mode = *(volatile unsigned int *)0x4000000 & 7;
    unsigned short h = *(volatile unsigned short *)0x400000e;
    if ((int)mode < 3 || ((int)mode < 6 && !(h & 0x80))) {
        unsigned int v = *(volatile unsigned int *)0x4000000;
        return 0x6000000 + (((v & 0x7000000) >> 0x18) << 0x10) +
               (((unsigned int)(h & 0x3c) >> 2) << 14);
    }
    return 0;
}

namespace G2S {

// @symbol _ZN3G2S13GetBG3CharPtrEv
unsigned int GetBG3CharPtr() {
    int cfg = *(volatile int *)0x4001000;
    unsigned int v = *(volatile unsigned short *)0x400100e;
    int m = cfg & 7;
    if (m < 3 || (m < 6 && !(v & 0x80)))
        return (((v & 0x3c) >> 2) << 0xe) + 0x6200000;
    return 0;
}

} // namespace G2S
