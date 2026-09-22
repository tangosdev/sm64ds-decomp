// @symbol func_ov006_02126b4c
/* recovered: minigame BG2 tile-column writer.
 *
 * Copies one 16-row column of 2x2 BG tiles into the BG2 screen of both the
 * main and the sub engine. Each cell index comes from the u16 grid at
 * self+0x4f38 (row stride 0x5c0, column `col`); its four tiles are the four
 * u16 words of data_ov006_0212f3bc[idx], written with MultiStore16 (two
 * halfwords each) at screen offset ((col & 0xf) << 7) + 2*j, +2, +0x40,
 * +0x42. flag == 1 targets the first half of the screen block, anything else
 * the half at +0x800.
 *
 * Three spellings are load-bearing for the second loop's register colouring
 * (notes/mwccarm-codegen.md 6cd and the 6cu addendum): the two dead
 * assignments at the head of the first loop (`tile`, `row`), the dead
 * initialiser on `i`, and `off` declared as long rather than int. The sixteen
 * volatile u16 locals are the ROM's real stack traffic (each tile is stored
 * and reloaded around the screen-pointer call).
 */
#include "types.h"

#pragma opt_strength_reduction off

extern char *_ZN2G212GetBG2ScrPtrEv(void);
extern char *_ZN3G2S12GetBG2ScrPtrEv(void);
extern void MultiStore16(int val, char *dst, int n);
extern u16 data_ov006_0212f3bc[];

void func_ov006_02126b4c(char *c, int col, int flag)
{
    volatile u16 v0, v1, v2, v3, v4, v5, v6, v7;
    volatile u16 w0, w1, w2, w3, w4, w5, w6, w7;
    int n;
    int i = 0;
    u16 tile;
    char *row;
    int j;
    long off;
    int k842;
    int masked;
    u32 idx;
    volatile u16 *scr;

    if (flag == 1) {
        masked = col & 0xf;
        tile = *(u16 *)((char *)data_ov006_0212f3bc + 6);
        row = c;
        i = 0;
        j = i;
        row = c + (col << 1);
        off = masked << 7;
        n = 2;
        do {
            idx = *(u16 *)(row + 0x4f38);
            tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3));
            scr = (volatile u16 *)(_ZN2G212GetBG2ScrPtrEv() + off);
            MultiStore16(v0 = tile, (char *)(scr + j), n);

            idx = *(u16 *)(row + 0x4f38);
            tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 2);
            scr = (volatile u16 *)(_ZN2G212GetBG2ScrPtrEv() + off);
            MultiStore16(v1 = tile, (char *)(scr + j + 1), n);

            idx = *(u16 *)(row + 0x4f38);
            tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 4);
            scr = (volatile u16 *)(_ZN2G212GetBG2ScrPtrEv() + off);
            MultiStore16(v2 = tile, (char *)(scr + j + 0x20), n);

            idx = *(u16 *)(row + 0x4f38);
            tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 6);
            scr = (volatile u16 *)(_ZN2G212GetBG2ScrPtrEv() + off);
            MultiStore16(v3 = tile, (char *)(scr + j + 0x21), n);

            idx = *(u16 *)(row + 0x4f38);
            tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3));
            scr = (volatile u16 *)(_ZN3G2S12GetBG2ScrPtrEv() + off);
            MultiStore16(v4 = tile, (char *)(scr + j), n);

            idx = *(u16 *)(row + 0x4f38);
            tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 2);
            scr = (volatile u16 *)(_ZN3G2S12GetBG2ScrPtrEv() + off);
            MultiStore16(v5 = tile, (char *)(scr + j + 1), n);

            idx = *(u16 *)(row + 0x4f38);
            tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 4);
            scr = (volatile u16 *)(_ZN3G2S12GetBG2ScrPtrEv() + off);
            MultiStore16(v6 = tile, (char *)(scr + j + 0x20), n);

            idx = *(u16 *)(row + 0x4f38);
            tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 6);
            scr = (volatile u16 *)(_ZN3G2S12GetBG2ScrPtrEv() + off);
            MultiStore16(v7 = tile, (char *)(scr + j + 0x21), n);

            row += 0x5c0;
            j += 2;
            i += 1;
        } while (i < 0x10);
        return;
    }

    masked = col & 0xf;
    i = 0;
    j = i;
    row = c + (col << 1);
    off = masked << 7;
    n = 2;
    k842 = 0x842;
    do {
        idx = *(u16 *)(row + 0x4f38);
        tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3));
        scr = (volatile u16 *)(_ZN2G212GetBG2ScrPtrEv() + 0x800 + off);
        MultiStore16(w0 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + 0x4f38);
        tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 2);
        scr = (volatile u16 *)(_ZN2G212GetBG2ScrPtrEv() + 0x802 + off);
        MultiStore16(w1 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + 0x4f38);
        tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 4);
        scr = (volatile u16 *)(_ZN2G212GetBG2ScrPtrEv() + 0x840 + off);
        MultiStore16(w2 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + 0x4f38);
        tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 6);
        scr = (volatile u16 *)(_ZN2G212GetBG2ScrPtrEv() + k842 + off);
        MultiStore16(w3 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + 0x4f38);
        tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3));
        scr = (volatile u16 *)(_ZN3G2S12GetBG2ScrPtrEv() + 0x800 + off);
        MultiStore16(w4 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + 0x4f38);
        tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 2);
        scr = (volatile u16 *)(_ZN3G2S12GetBG2ScrPtrEv() + 0x802 + off);
        MultiStore16(w5 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + 0x4f38);
        tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 4);
        scr = (volatile u16 *)(_ZN3G2S12GetBG2ScrPtrEv() + 0x840 + off);
        MultiStore16(w6 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + 0x4f38);
        tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 6);
        scr = (volatile u16 *)(_ZN3G2S12GetBG2ScrPtrEv() + k842 + off);
        MultiStore16(w7 = tile, (char *)(scr + j), n);

        row += 0x5c0;
        j += 2;
        i += 1;
    } while (i < 0x10);
}
