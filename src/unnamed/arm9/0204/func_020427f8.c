/* func_020427f8 @ 0x20427f8 (arm9) -- tail-call veneer to OS_InitTick (0x2059788).
 * ldr ip, [pc]; bx ip; .word 0x2059788
 */
#include "nitro/os/tick.h"

void func_020427f8(void) {
    OS_InitTick();
}
