//cpp
/* Message queues (the Nitro SDK's os_message.c), arm9 .text
 * 0x020587e4..0x02058960.
 *
 * pret/pokediamond's NitroSDK OS_message.c has, in this order,
 * OS_InitMessageQueue, OS_SendMessage, OS_ReceiveMessage, OS_JamMessage and
 * OS_ReadMessage. mwccarm defers the bodies and emits them in reverse, so the
 * ROM runs bottom-up: OS_ReceiveMessage at 0x020587e4, OS_InitMessageQueue at
 * 0x02058940. Nothing calls OS_JamMessage or OS_ReadMessage, so the linker
 * dropped them.
 *
 * This SDK's thread queue is a 16-bit mask of thread ids (nitro/os/thread.h).
 */
#include "nitro/os/system.h"
#include "nitro/os/thread.h"
#include "nitro/os/message.h"

void OS_InitMessageQueue(OSMessageQueue *mq, OSMessage *msgArray, s32 msgCount) {
    mq->queueSend = 0;
    mq->queueReceive = 0;
    mq->msgArray = msgArray;
    mq->msgCount = msgCount;
    mq->firstIndex = 0;
    mq->usedCount = 0;
}

int OS_SendMessage(OSMessageQueue *mq, OSMessage msg, s32 flags) {
    OSIntrMode enabled = OS_DisableInterrupts();
    while (mq->msgCount <= mq->usedCount) {
        if (!(flags & OS_MESSAGE_BLOCK)) {
            OS_RestoreInterrupts(enabled);
            return 0;
        }
        OS_SleepThread(&mq->queueSend);
    }
    {
        s32 lastIndex = (mq->firstIndex + mq->usedCount) % mq->msgCount;
        mq->msgArray[lastIndex] = msg;
        mq->usedCount++;
        OS_WakeupThread(&mq->queueReceive);
        OS_RestoreInterrupts(enabled);
        return 1;
    }
}

int OS_ReceiveMessage(OSMessageQueue *mq, OSMessage *msg, s32 flags) {
    OSIntrMode enabled = OS_DisableInterrupts();
    while (mq->usedCount == 0) {
        if (!(flags & OS_MESSAGE_BLOCK)) {
            OS_RestoreInterrupts(enabled);
            return 0;
        }
        OS_SleepThread(&mq->queueReceive);
    }
    if (msg != 0)
        *msg = mq->msgArray[mq->firstIndex];
    mq->firstIndex = (mq->firstIndex + 1) % mq->msgCount;
    mq->usedCount--;
    OS_WakeupThread(&mq->queueSend);
    OS_RestoreInterrupts(enabled);
    return 1;
}
