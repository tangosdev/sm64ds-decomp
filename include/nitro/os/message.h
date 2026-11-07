#ifndef NITRO_OS_MESSAGE_H
#define NITRO_OS_MESSAGE_H

/*
 * The message queue of the Nitro SDK's os_message.c, this SDK's layout. The
 * bodies are in src/engine/os/OS_Message.cpp.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OS_MESSAGE_NOBLOCK 0
#define OS_MESSAGE_BLOCK 1

typedef void *OSMessage;

typedef struct OSMessageQueue {
    u16 queueSend;       /* 0x00 -- OSThreadQueue: senders waiting for room */
    u16 queueReceive;    /* 0x02 -- receivers waiting for a message */
    OSMessage *msgArray; /* 0x04 */
    s32 msgCount;        /* 0x08 */
    s32 firstIndex;      /* 0x0c */
    s32 usedCount;       /* 0x10 */
} OSMessageQueue;

#ifndef SM64DS_PLATFORM_PC
typedef char OSMessageQueue_size_must_be_0x14[sizeof(OSMessageQueue) == 0x14 ? 1 : -1];
#endif

void OS_InitMessageQueue(OSMessageQueue *, OSMessage *, s32);
int OS_SendMessage(OSMessageQueue *, OSMessage, s32);
int OS_ReceiveMessage(OSMessageQueue *, OSMessage *, s32);

#ifdef __cplusplus
}
#endif

#endif
