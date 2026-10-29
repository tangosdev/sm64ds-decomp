//cpp
#include "IRQ.h"

extern "C" {

extern void OS_SleepThread(void *queue);
extern void OS_WakeupThread(void *p);

/* NitroSDK OS_SendMessage(mq, msg, flags): wait (OS_MESSAGE_BLOCK, bit 0) on
 * queueSend while the queue is full, store msg after the used entries, and
 * wake queueReceive. OSMessageQueue, with the field names func_020587e4
 * (OS_ReceiveMessage) uses; the tag stays Q, as SoundEngine.cpp declares it. */
struct Q {
    unsigned short queueSend;    /* 0x00 */
    unsigned short queueReceive; /* 0x02 */
    int *msgArray;               /* 0x04 */
    int msgCount;                /* 0x08 */
    int firstIndex;              /* 0x0c */
    int usedCount;               /* 0x10 */
};

int func_02058894(struct Q *q, int val, int flag) {
    unsigned int saved = IRQ::Disable();
    while (q->msgCount <= q->usedCount) {
        if ((flag & 1) == 0) {
            IRQ::Restore(saved);
            return 0;
        }
        OS_SleepThread(&q->queueSend);
    }
    {
        int idx = (q->firstIndex + q->usedCount) % q->msgCount;
        q->msgArray[idx] = val;
        q->usedCount++;
        OS_WakeupThread(&q->queueReceive);
        IRQ::Restore(saved);
        return 1;
    }
}
}
