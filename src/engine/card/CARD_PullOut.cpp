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
#include "IRQ.h"

namespace IRQ {

u32 Disable(void);
u32 Restore(u32 saved);
void GameCardIREQMCHandler(void);

}

typedef int (*CardCallback)(void);

extern "C" {
extern int data_020a89a0;          /* set once the card has been pulled */
extern CardCallback data_020a89a4; /* nonzero return: terminate */
int IPCSend(unsigned int tag, unsigned int data, unsigned int reply);
}

#define IRQ_CARD_IREQ_MC 0x100000

extern "C" {

// @symbol func_02061138
// Install the pull-out handler on the card interrupt.
void func_02061138(void) {
    u32 saved = IRQ::Disable();
    IRQ::SetIRQHandler(IRQ_CARD_IREQ_MC, IRQ::GameCardIREQMCHandler);
    IRQ::EnableIRQs(IRQ_CARD_IREQ_MC);
    IRQ::Restore(saved);
}

// @symbol func_02061128
// Register the game's pull-out callback.
void func_02061128(CardCallback callback) {
    data_020a89a4 = callback;
}

// @symbol func_020610fc
// Tell the ARM7 the card is gone, then stop for good.
void func_020610fc(void) {
    int tag = 0xd; /* PXI channel */
    int data = 2;
    int reply = 0;
    while (IPCSend(tag, data, reply) != 0)
        ;
    for (;;)
        ;
}
}

namespace IRQ {

// @symbol _ZN3IRQ21GameCardIREQMCHandlerEv
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
