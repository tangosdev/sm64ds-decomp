//cpp
/* Game-card pull-out detection (SDK card module), .text 0x020610ac..0x0206116c.
 * The card slot raises IE bit 20 (IREQ_MC) when the card is pulled. The
 * handler records the event, asks the game's callback whether to carry on,
 * and otherwise tells the ARM7 over PXI and stops.
 *
 * mwccarm emits .text in reverse source order, so the file reads from the
 * top of the run down: init, set callback, terminate, IRQ handler.
 *
 * leftovers: the pulled-out flag and the callback word (data_020a89a0,
 * data_020a89a4) are not claimed here; the three C-linkage members keep their
 * address names.
 */
#include "nitro/os/system.h"
#include "nitro/os/interrupt.h"

namespace IRQ {

void GameCardIREQMCHandler(void);

}

typedef int (*CardCallback)(void);

extern "C" {
extern int data_020a89a0;          /* set once the card has been pulled */
extern CardCallback data_020a89a4; /* nonzero return: terminate */
int PXI_SendWordByFifo(unsigned int tag, unsigned int data, unsigned int reply);
}

extern "C" {

// Install the pull-out handler on the card interrupt.
void func_02061138(void) {
    OSIntrMode saved = OS_DisableInterrupts();
    OS_SetIrqFunction(OS_IE_CARD_IREQ, IRQ::GameCardIREQMCHandler);
    OS_EnableIrqMask(OS_IE_CARD_IREQ);
    OS_RestoreInterrupts(saved);
}

// Register the game's pull-out callback.
void func_02061128(CardCallback callback) {
    data_020a89a4 = callback;
}

// Tell the ARM7 the card is gone, then stop for good.
void func_020610fc(void) {
    int tag = 0xd; /* PXI channel */
    int data = 2;
    int reply = 0;
    while (PXI_SendWordByFifo(tag, data, reply) != 0)
        ;
    for (;;)
        ;
}
}

namespace IRQ {

// The card interrupt: flag it, then terminate unless the callback says no.
void GameCardIREQMCHandler() {
    CardCallback callback = data_020a89a4;
    int result = 1;
    data_020a89a0 = result;
    if (callback)
        result = callback();
    if (result == 0)
        return;
    func_020610fc();
}

}
