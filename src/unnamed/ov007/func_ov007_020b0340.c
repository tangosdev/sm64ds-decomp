#include "Ov007MenuState.h"
#include "Ov007Work.h"

/* Clears the pending choice; func_ov007_020aed98 calls it for its case 6. */
void func_ov007_020b0340(void) {
    gMenuWork->pendingChoice = 0;
}
