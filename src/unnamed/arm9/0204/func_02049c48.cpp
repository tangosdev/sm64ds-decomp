//cpp
#include "nitro/os/system.h"
#include "CaptureEffect.h"
#include "nitro/mi/memory.h"
#include "nitro/snd/interface.h"

/* Switch the sound-capture effect to kind (data_020a4c64; func_02049ba8
   runs it): leaving kind 1 sends SNDi_SetSurroundDecay(0); then clear the
   effect's work (data_020a4c70) and store the kind under the IRQ lock, and
   for kind 1 send SNDi_SetSurroundDecay(0x3000) as func_02049cd8 does. */

void func_02049c48(int kind) {
    int current = data_020a4c64.kind;
    volatile unsigned short tmp;
    OSIntrMode savedIRQ;
    if (kind == current)
        return;
    if (current == 1) {
        SNDi_SetSurroundDecay(0);
    }
    savedIRQ = OS_DisableInterrupts();
    tmp = 0;
    MI_CpuFill16(&data_020a4c70, tmp, sizeof(data_020a4c70));
    data_020a4c64.kind = kind;
    OS_RestoreInterrupts(savedIRQ);
    if (kind == 1) {
        SNDi_SetSurroundDecay(0x3000);
    }
}
