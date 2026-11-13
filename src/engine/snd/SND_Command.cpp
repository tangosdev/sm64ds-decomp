//cpp
/* The ARM9 sound command queue (the Nitro SDK's SND_command.c), arm9 .text
 * 0x0205adf8..0x0205b470.
 *
 * The 256 SNDCommand records start on the free list. SND_AllocCommand takes
 * one, SND_PushCommand appends it to the reserve list, and
 * SND_FlushCommand hands the whole reserve list to the ARM7 over the sound
 * PXI FIFO and parks it in the waiting queue. SND_RecvCommandReply takes
 * back each list the ARM7 has finished, as counted by the shared work's
 * finished-command tag, and returns its records to the free list.
 *
 * pret's pokediamond SND_command.c defines these fifteen functions in this
 * order. mwccarm emits the bodies in reverse, so the ROM runs from the
 * static AllocCommand at 0x0205adf8 up to SND_CommandInit at 0x0205b358.
 * This SDK is older than pret's. Nothing disables interrupts around the
 * lists, nothing spins with OS_SpinWait while the FIFO is full, and
 * SND_FlushCommand has no SND_CMD_FLAG_IMMEDIATE. There is no
 * IsCommandAvailable: SND_AllocCommand and InitPXI give up when
 * OS_IsRunOnEmulator says so.
 */
#include "types.h"
#include "nitro/os/cache.h"
#include "nitro/os/system.h"
#include "nitro/os/emulator.h"
#include "nitro/snd/command.h"
#include "nitro/pxi/fifo.h"

typedef int BOOL;
#define FALSE 0

extern "C" {

#define SND_CMD_COUNT 256
#define SND_CMD_WAIT_QUEUE_COUNT 8
#define SND_CMD_FLAG_NOBLOCK 0x0
#define SND_CMD_FLAG_BLOCK 0x1
/* This SDK's number for pret's SND_CMD_SET_SHARED_WORK (0x1d). */
#define SND_CMD_SET_SHARED_WORK 0x19

struct SNDSharedWork;

extern SNDCommand *data_020a6484;    /* sFreeList */
extern u32 data_020a6488;            /* sFinishedTag */
extern SNDCommand *data_020a648c;    /* sReserveList */
extern SNDCommand *data_020a6490;    /* sReserveListEnd */
extern SNDCommand *data_020a6494;    /* sFreeListEnd */
extern s32 data_020a6498;            /* sWaitingCommandListQueueRead */
extern s32 data_020a649c;            /* sWaitingCommandListQueueWrite */
extern s32 data_020a64a0;            /* sWaitingCommandListCount */
extern u32 data_020a64a4;            /* sCurrentTag */
extern SNDCommand *data_020a64a8[];  /* sWaitingCommandListQueue */
extern SNDSharedWork data_020a64e0;  /* sSharedWork */
extern SNDCommand data_020a6760[];   /* sCommandArray */
extern SNDCommand data_020a7f48;     /* sCommandArray[SND_CMD_COUNT - 1] */
extern SNDSharedWork *data_020a7fc0; /* SNDi_SharedWork */

/* The ROM reaches sCommandArray[SND_CMD_COUNT - 1].next through this
 * label, 0x1000 bytes into the array. */
extern unsigned char data_020a7760[];

void SNDi_CallAlarmHandler(s32 idx);
void SNDi_InitSharedWork(SNDSharedWork *sw);
u32 SNDi_GetFinishedCommandTag(void);

SNDCommand *SND_AllocCommand(u32 flags);
void SND_PushCommand(SNDCommand *cmd);
int SND_FlushCommand(u32 flags);
const SNDCommand *SND_RecvCommandReply(u32 flags);
int SND_IsFinishedCommandTag(u32 tag);
s32 SND_CountFreeCommand(void);
s32 SND_CountReservedCommand(void);
s32 SND_CountWaitingCommand(void);

void InitPXI(void);
void RequestCommandProc(void);
SNDCommand *AllocCommand(void);

void SND_CommandInit(void) {
    int i;
    SNDCommand *cmd;

    InitPXI();

    data_020a6484 = data_020a6760;
    for (i = 0; i < SND_CMD_COUNT - 1; i++)
        data_020a6760[i].next = &data_020a6760[i + 1];
    *(SNDCommand **)(data_020a7760 + 0x7e8) = 0;
    data_020a6494 = &data_020a7f48;

    data_020a648c = 0;
    data_020a6490 = 0;
    data_020a64a0 = 0;
    data_020a6498 = 0;
    data_020a649c = 0;
    data_020a64a4 = 1;
    data_020a6488 = 0;

    data_020a7fc0 = &data_020a64e0;
    SNDi_InitSharedWork(data_020a7fc0);

    cmd = SND_AllocCommand(SND_CMD_FLAG_BLOCK);
    if (cmd == 0)
        return;

    cmd->id = SND_CMD_SET_SHARED_WORK;
    cmd->arg[0] = (u32)data_020a7fc0;
    SND_PushCommand(cmd);
    SND_FlushCommand(SND_CMD_FLAG_BLOCK);
}

const SNDCommand *SND_RecvCommandReply(u32 flags) {
    SNDCommand *queueRead;
    SNDCommand *cur;
    s32 read;

    if (flags == SND_CMD_FLAG_NOBLOCK) {
        if (SNDi_GetFinishedCommandTag() == data_020a6488)
            return 0;
    } else {
        while (SNDi_GetFinishedCommandTag() == data_020a6488) {
        }
    }

    read = data_020a6498;
    queueRead = data_020a64a8[read];
    data_020a6498 = ++read;
    if (read > SND_CMD_WAIT_QUEUE_COUNT)
        data_020a6498 = 0;

    cur = queueRead;
    while (cur->next != 0)
        cur = cur->next;

    if (data_020a6494 != 0)
        data_020a6494->next = queueRead;
    else
        data_020a6484 = queueRead;

    data_020a6494 = cur;
    data_020a64a0--;
    data_020a6488++;
    return queueRead;
}

SNDCommand *SND_AllocCommand(u32 flags) {
    SNDCommand *cmd;

    if (OS_IsRunOnEmulator())
        return 0;

    cmd = AllocCommand();
    if (cmd != 0)
        return cmd;

    if (flags == SND_CMD_FLAG_NOBLOCK)
        return 0;

    if (SND_CountWaitingCommand() > 0) {
        while (SND_RecvCommandReply(SND_CMD_FLAG_NOBLOCK) != 0) {
        }

        cmd = AllocCommand();
        if (cmd != 0)
            return cmd;
    } else {
        SND_FlushCommand(SND_CMD_FLAG_BLOCK);
    }

    RequestCommandProc();

    do {
        SND_RecvCommandReply(SND_CMD_FLAG_BLOCK);
        cmd = AllocCommand();
    } while (cmd == 0);
    return cmd;
}

void SND_PushCommand(SNDCommand *cmd) {
    if (data_020a6490 == 0) {
        data_020a6490 = cmd;
        data_020a648c = cmd;
    } else {
        data_020a6490->next = cmd;
        data_020a6490 = cmd;
    }

    cmd->next = 0;
}

int SND_FlushCommand(u32 flags) {
    if (data_020a648c == 0)
        return 1;

    if (data_020a64a0 >= SND_CMD_WAIT_QUEUE_COUNT) {
        if (flags == SND_CMD_FLAG_NOBLOCK)
            return 0;

        do {
            SND_RecvCommandReply(SND_CMD_FLAG_BLOCK);
        } while (data_020a64a0 >= SND_CMD_WAIT_QUEUE_COUNT);
    }

    DC_FlushRange(data_020a6760, SND_CMD_COUNT * sizeof(SNDCommand));

    if (PXI_SendWordByFifo(PXI_FIFO_TAG_SOUND, (u32)data_020a648c, FALSE) < 0) {
        if (flags == SND_CMD_FLAG_NOBLOCK)
            return 0;

        while (PXI_SendWordByFifo(PXI_FIFO_TAG_SOUND, (u32)data_020a648c, FALSE) < 0) {
        }
    }

    data_020a64a8[data_020a649c] = data_020a648c;
    if (++data_020a649c > SND_CMD_WAIT_QUEUE_COUNT)
        data_020a649c = 0;

    data_020a648c = 0;
    data_020a6490 = 0;
    data_020a64a0++;
    data_020a64a4++;
    return 1;
}

void SND_WaitForCommandProc(u32 tag) {
    if (SND_IsFinishedCommandTag(tag))
        return;

    while (SND_RecvCommandReply(SND_CMD_FLAG_NOBLOCK) != 0) {
    }

    if (SND_IsFinishedCommandTag(tag))
        return;

    RequestCommandProc();

    if (SND_IsFinishedCommandTag(tag))
        return;

    do {
        SND_RecvCommandReply(SND_CMD_FLAG_BLOCK);
    } while (SND_IsFinishedCommandTag(tag) == 0);
}

u32 SND_GetCurrentCommandTag(void) {
    u32 retval;
    if (data_020a648c == 0)
        retval = data_020a6488;
    else
        retval = data_020a64a4;
    return retval;
}

int SND_IsFinishedCommandTag(u32 tag) {
    if (tag > data_020a6488)
        return tag - data_020a6488 >= 0x80000000;
    else
        return data_020a6488 - tag < 0x80000000;
}

s32 SND_CountFreeCommand(void) {
    s32 count = 0;
    for (SNDCommand *cmd = data_020a6484; cmd != 0; cmd = cmd->next)
        count++;
    return count;
}

s32 SND_CountReservedCommand(void) {
    s32 count = 0;
    for (SNDCommand *cmd = data_020a648c; cmd != 0; cmd = cmd->next)
        count++;
    return count;
}

s32 SND_CountWaitingCommand(void) {
    return SND_CMD_COUNT - SND_CountFreeCommand() - SND_CountReservedCommand();
}

void PxiFifoCallback(PXIFifoTag tag, u32 data, BOOL err) {
    OSIntrMode saved = OS_DisableInterrupts();
    SNDi_CallAlarmHandler(data);
    OS_RestoreInterrupts(saved);
}

void InitPXI(void) {
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_SOUND, PxiFifoCallback);

    if (OS_IsRunOnEmulator())
        return;

    if (PXI_IsCallbackReady(PXI_FIFO_TAG_SOUND, PXI_PROC_ARM7))
        return;

    do {
        OS_SpinWait(100);
    } while (!PXI_IsCallbackReady(PXI_FIFO_TAG_SOUND, PXI_PROC_ARM7));
}

void RequestCommandProc(void) {
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_SOUND, 0, FALSE) < 0) {
    }
}

SNDCommand *AllocCommand(void) {
    SNDCommand *retval = data_020a6484;
    if (retval == 0)
        return 0;

    data_020a6484 = retval->next;
    if (data_020a6484 == 0)
        data_020a6494 = 0;
    return retval;
}
}
