//cpp
#include "IRQ.h"

extern "C" {

/* Switch the sound-capture effect to kind (data_020a4c64; func_02049ba8
   runs it): leaving kind 1 sends SNDi_SetSurroundDecay(0); then clear the
   effect state at data_020a4c70 and store the kind under the IRQ lock, and
   for kind 1 send SNDi_SetSurroundDecay(0x3000) as func_02049cd8 does. */

extern void MultiStore16(unsigned short val, void *dest, int size);
extern void SNDi_SetSurroundDecay(int val);

extern int data_020a4c64;       // linked to 0x020a4c64
extern unsigned short data_020a4c70[];  // linked to 0x020a4c70

void func_02049c48(int kind) {
    int current = data_020a4c64;
    volatile unsigned short tmp;
    unsigned int savedIRQ;
    if (kind == current)
        return;
    if (current == 1) {
        SNDi_SetSurroundDecay(0);
    }
    savedIRQ = IRQ::Disable();
    tmp = 0;
    MultiStore16(tmp, data_020a4c70, 0xc0);
    data_020a4c64 = kind;
    IRQ::Restore(savedIRQ);
    if (kind == 1) {
        SNDi_SetSurroundDecay(0x3000);
    }
}
}
