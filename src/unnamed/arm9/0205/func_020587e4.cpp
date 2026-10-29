//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

struct MsgQueue {
    u16 queueSend;      /* 0x00 */
    u16 queueReceive;   /* 0x02 */
    u32 *msgArray;      /* 0x04 */
    int msgCount;       /* 0x08 */
    int firstIndex;     /* 0x0c */
    int usedCount;      /* 0x10 */
};

void OS_SleepThread(u16 *self);
void OS_WakeupThread(u16 *self);

int func_020587e4(struct MsgQueue *self, u32 *msg, int flags) {
    u32 enabled = IRQ::Disable();

    while (self->usedCount == 0) {
        if ((flags & 1) == 0) {
            IRQ::Restore(enabled);
            return 0;
        }
        OS_SleepThread(&self->queueReceive);
    }

    if (msg != 0) {
        *msg = self->msgArray[self->firstIndex];
    }
    self->firstIndex = (self->firstIndex + 1) % self->msgCount;
    *(int *)(&self->usedCount) -= 1;
    OS_WakeupThread(&self->queueSend);

    IRQ::Restore(enabled);
    return 1;
}
}
