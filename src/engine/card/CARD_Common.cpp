//cpp
/* Card access common code (the Nitro SDK's card_common.c),
 * arm9 .text 0x0205ff70..0x020602bc.
 *
 * pret/pokediamond's NitroSDK CARD_common.c has these nine functions in this
 * order: CARDi_SetTask, CARDi_LockResource, CARDi_UnlockResource,
 * CARDi_InitCommon, CARDi_WaitAsync, CARD_LockRom, CARD_UnlockRom,
 * CARD_LockBackup, CARD_UnlockBackup. The rest of that file (CARD_IsEnabled,
 * CARD_CheckEnabled, CARD_Enable, CARDi_TryWaitAsync, CARD_GetResultCode) is
 * not in this ROM.
 *
 * mwccarm defers C bodies and emits them in reverse, so the ROM runs
 * bottom-up: CARD_UnlockBackup at 0x0205ff70, CARDi_SetTask at 0x02060228.
 *
 * This SDK is older than pret's. CARDiCommon is smaller (no request type,
 * retry or mode fields), CARDi_InitCommon has no multiboot-child checks and
 * never calls CARD_Enable, and CARDi_SetTask resolves the relative priorities
 * 0x20 / 0x21 against the calling thread.
 */
#include "types.h"
#include "nitro/os/system.h"
#include "nitro/os/cache.h"
#include "nitro/card/card_common.h"
#include "nitro/os/spinLock.h"
#include "nitro/mi/memory.h"
#include "nitro/pxi/fifo.h"

extern "C" {

/* local extern: no header declares this yet. */

#define OS_LOCK_ID_ERROR (-3)
#define CARD_TARGET_NONE 0
#define CARD_TARGET_ROM 1
#define CARD_TARGET_BACKUP 2
#define CARD_THREAD_PRIORITY_DEFAULT 4
/* Relative priorities: one step above / below the calling thread. */
#define CARD_THREAD_PRIORITY_HIGHER 0x20
#define CARD_THREAD_PRIORITY_LOWER 0x21

extern CARDiCommandArg cardi_arg;
/* The word just past cardi_thread_stack (0x400 bytes): its address is the
 * stack's top. */
extern int *data_020a8760;

void CARDi_SetTask(void (*task)(CARDiCommon *)) {
    CARDiCommon *const p = &cardi_common;
    u32 priority = p->priority;
    switch (priority) {
    case CARD_THREAD_PRIORITY_HIGHER:
        priority = OS_GetThreadPriority(OSi_ThreadInfo.current);
        if (priority != 0)
            priority -= 1;
        break;
    case CARD_THREAD_PRIORITY_LOWER:
        priority = OS_GetThreadPriority(OSi_ThreadInfo.current);
        if (priority < 0x1f)
            priority += 1;
        break;
    }
    OS_SetThreadPriority(&p->thread, priority);
    p->cur_th = &p->thread;
    p->task_func = task;
    p->flag |= CARD_STAT_TASK;
    OS_WakeupThreadDirect(&p->thread);
}

void CARDi_LockResource(int owner, int target) {
    CARDiCommon *const p = &cardi_common;
    OSIntrMode saved = OS_DisableInterrupts();
    if (p->lock_owner == owner) {
        if (p->lock_target != target)
            OS_Terminate();
    } else {
        while (p->lock_owner != OS_LOCK_ID_ERROR)
            OS_SleepThread(&p->lock_queue);
        p->lock_owner = owner;
        p->lock_target = target;
    }
    ++p->lock_ref;
    p->cmd->result = CARD_RESULT_SUCCESS;
    OS_RestoreInterrupts(saved);
}

void CARDi_UnlockResource(int owner, int target) {
    CARDiCommon *p = &cardi_common;
    OSIntrMode saved = OS_DisableInterrupts();
    if (p->lock_owner != owner || !p->lock_ref) {
        OS_Terminate();
    } else {
        if (p->lock_target != target)
            OS_Terminate();
        if (!--p->lock_ref) {
            p->lock_owner = OS_LOCK_ID_ERROR;
            p->lock_target = CARD_TARGET_NONE;
            OS_WakeupThread(&p->lock_queue);
        }
    }
    p->cmd->result = CARD_RESULT_SUCCESS;
    OS_RestoreInterrupts(saved);
}

void CARDi_InitCommon(void) {
    CARDiCommon *p = &cardi_common;
    p->lock_owner = OS_LOCK_ID_ERROR;
    p->lock_ref = 0;
    p->lock_target = CARD_TARGET_NONE;
    p->cmd = &cardi_arg;
    {
        /* MI_CpuFillFast's value: the ROM passes it through a stack slot. */
        volatile u32 fill = 0;
        MI_CpuFillFast(&cardi_arg, fill, sizeof(cardi_arg));
    }
    DC_FlushRange(&cardi_arg, sizeof(cardi_arg));
    p->lock_queue = 0;
    p->busy_q = 0;
    p->priority = CARD_THREAD_PRIORITY_DEFAULT;
    OS_CreateThread(&p->thread, CARDi_TaskThread, 0, &data_020a8760, 0x400, p->priority);
    OS_WakeupThreadDirect(&p->thread);
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_FS, CARDi_OnFifoRecv);
}

int CARDi_WaitAsync(void) {
    CARDiCommon *const p = &cardi_common;
    OSIntrMode saved = OS_DisableInterrupts();
    while (p->flag & CARD_STAT_BUSY)
        OS_SleepThread(&p->busy_q);
    OS_RestoreInterrupts(saved);
    return p->cmd->result == CARD_RESULT_SUCCESS;
}

void CARD_LockRom(u16 lockID) {
    CARDi_LockResource(lockID, CARD_TARGET_ROM);
    OS_TryLockCard(lockID);
}

void CARD_UnlockRom(u16 lockID) {
    OS_UnlockCard(lockID);
    CARDi_UnlockResource(lockID, CARD_TARGET_ROM);
}

void CARD_LockBackup(u16 lockID) {
    CARDi_LockResource(lockID, CARD_TARGET_BACKUP);
}

void CARD_UnlockBackup(u16 lockID) {
    CARDi_UnlockResource(lockID, CARD_TARGET_BACKUP);
}
}
