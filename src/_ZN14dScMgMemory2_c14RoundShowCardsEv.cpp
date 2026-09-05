//cpp
// @symbol _ZN14dScMgMemory2_c14RoundShowCardsEv
#include "decl_common.h"
#include "dScMgMemory2_c.h"
/* dScMgMemory2_c::RoundShowCards.
 *
 * End of the preview round: walk the eight preview slots, clear each one, and
 * for every card still active whose state is 1 (face up, waiting) push it to
 * state 3 and count it. The count picks one of three jingles. When no slot held
 * a card at all the round is over, so the substate advances and the preview
 * timer is rearmed from the difficulty.
 *
 * WAS A C99 FILE spelling its own mangled name; this is the same body as a real
 * method, byte-identical at 2004/b56.
 *
 * THE LOOP KEEPS RAW OFFSETS OFF `this`, and that is measured, not laziness.
 * Three member substitutions were tried against the byte gate and each one cost
 * instructions, so each was backed out:
 *   &mPreviewCards[i]      instead of (c + i) + 0x53fd   -> 0x108, +12 bytes
 *   mCards[id].active      instead of e + 0x51bb         -> 0x114, +24 bytes
 *   &mCards[id].state      instead of e + 0x51bc         -> 0x114, +24 bytes
 * The ROM forms the slot address as a pre-indexed load with 0x53fd held in a
 * pool register (`ldrb sb, [r0, r6]!`) and reaches the card through
 * `mla sl, sb, ip, r5` + `add lr, sl, #0x5000`, i.e. base `this` with the card
 * offset split 0x5000 + 0x1bb. Indexing the real arrays folds those bases
 * differently and the shape drifts. The tail members DO reproduce and are named.
 * Same disclosure pattern as the sibling methods in this class -- see
 * src/_ZN14dScMgMemory2_c6RenderEv.cpp. */

namespace Sound { void PlayBank2_2D(unsigned int); }

void dScMgMemory2_c::RoundShowCards()
{
    char *c = (char *)this;
    unsigned char *p;
    unsigned char id;
    char *e;
    unsigned char *q;
    int cntA;
    int cntB;
    int i;

    cntA = 0;
    cntB = 0;
    for (i = 0; i < 8; i++) {
        p = (unsigned char *)(c + i) + 0x53fd;   /* &mPreviewCards[i] */
        id = *p;
        if (id != 0xff) {
            e = c + id * 0x18;                   /* &mCards[id], base this */
            if (*(unsigned char *)(e + 0x51bb) != 0) {   /* .active */
                q = (unsigned char *)(e + 0x51bc);       /* &.state  */
                if (*q == 1) {
                    *q = 3;
                    *p = 0xff;
                    cntB++;
                }
            } else {
                *p = 0xff;
            }
            cntA++;
        }
    }
    if (cntB == 1) Sound::PlayBank2_2D(0x148);
    else if (cntB == 2) Sound::PlayBank2_2D(0x145);
    else if (cntB >= 3) Sound::PlayBank2_2D(0x146);
    if (cntA != 0) return;
    mSubstate = 6;
    mPreviewTimer = (u16)(mDifficulty * 0x14 + 0x50);
}
