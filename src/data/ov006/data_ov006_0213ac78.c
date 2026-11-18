/*
 * One object for pmf_MascotWalker_StateWalk_2, pmf_Dealer_StateWin,
 * pmf_MascotWalker_StateWalk_3, pmf_Dealer_StateIdle_3,
 * pmf_Dealer_StateLose, pmf_Dealer_StateSwap, pmf_Dealer_StateReveal,
 * pmf_Dealer_StateDeal_2, pmf_Dealer_StateReveal_2 (72 bytes): the linker
 * starts every input section on a 4-byte boundary, so these tables cannot be
 * separate.
 */
#include "types.h"

#pragma define_section obj ".data.gap43" ".data.gap43" ".data.gap43" far_data
#pragma section obj begin
extern void _ZN6Dealer9StateSwapEv(void *);
extern void _ZN6Dealer9StateLoseEv(void *);
extern void _ZN6Dealer9StateIdleEv(void *);
extern void _ZN6Dealer9StateDealEv(void *);
extern void _ZN6Dealer8StateWinEv(void *);
extern void _ZN6Dealer11StateRevealEv(void *);
extern void _ZN12MascotWalker9StateWalkEv(void *);
int pmf_MascotWalker_StateWalk_2[18] = {
    (int)&_ZN12MascotWalker9StateWalkEv, 0x00000000, (int)&_ZN6Dealer8StateWinEv, 0x00000000, (int)&_ZN12MascotWalker9StateWalkEv, 0x00000000, (int)&_ZN6Dealer9StateIdleEv, 0x00000000,
    (int)&_ZN6Dealer9StateLoseEv, 0x00000000, (int)&_ZN6Dealer9StateSwapEv, 0x00000000, (int)&_ZN6Dealer11StateRevealEv, 0x00000000, (int)&_ZN6Dealer9StateDealEv, 0x00000000,
    (int)&_ZN6Dealer11StateRevealEv, 0x00000000,
};
#pragma section obj end
