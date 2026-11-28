/* Card backup access (the Nitro SDK's card_backup.c), arm9 .text
 * 0x02060364..0x02060890.
 *
 * ntrtwl/NitroSDK libraries/card/src/card_backup.c carries the backup spec
 * table and its selection, CARD_IdentifyBackup, the spec accessors and the
 * backup read/write/verify requests and their card-thread tasks. This SDK
 * is older: one entry point per request where pret's has
 * CARDi_RequestStreamCommand, and CARDi_EndTask -- which every task here
 * ends through -- is out of line at the end of the file.
 *
 * mwccarm emits the bodies in reverse, so the ROM runs bottom-up:
 * func_02060364 at 0x02060364, CARDi_EndTask at 0x0206081c.
 */
#include "types.h"
#include "nitro/os/system.h"
#include "nitro/os/cache.h"
#include "nitro/card/card_common.h"
#include "nitro/mi/memory.h"
#include "private/card_backup_spec.h"

/* card_backup.c's selected-spec state: the table entry CARD_IdentifyBackup
   selected and its sector-size halfword. The remaining 0x18 bytes of the
   range have no reference in the ROM; one trailing array keeps the exact
   tiling through 0x020a8780, where card_rom.c's rom_stat starts. */
struct CardBackupSpec *data_020a8760;
unsigned int data_020a8764;
unsigned int data_020a8768[6];

void func_0206062c(CARDiCommon *p);
void func_0206071c(CARDiCommon *p);
void func_020607e8(CARDiCommon *p);
struct CardBackupSpec *func_02060398(int arg);
void func_02060364(int arg);
extern const struct CardBackupSpec data_020867bc[];

// @symbol CARDi_EndTask
/* NitroSDK CARDi_EndTask: take the callback, clear the busy and task bits,
 * wake every caller waiting on busy_q and, if the card thread is waiting
 * for work (CARD_STAT_RECV), the card thread itself; then, interrupts
 * restored, run the callback. */
void CARDi_EndTask(CARDiCommon *p) {
    MIDmaCallback callback = p->callback;
    void *arg = p->callback_arg;
    OSIntrMode mask = OS_DisableInterrupts();
    p->flag &= ~(CARD_STAT_BUSY | CARD_STAT_TASK);
    OS_WakeupThread(&p->busy_q);
    if (p->flag & CARD_STAT_RECV) {
        OS_WakeupThreadDirect(&p->thread);
    }
    OS_RestoreInterrupts(mask);
    if (callback != 0) {
        callback(arg);
    }
}

// @symbol func_020607e8
/* The backup identify task CARD_IdentifyBackup runs: put the backup type
 * (CARD_GetCurrentBackupType, the first word of the spec func_02060364
 * selected) in the request's type field, send CARD_REQ_IDENTIFY, then end
 * the task. */
void func_020607e8(CARDiCommon *p) {
    p->cmd->type = CARD_GetCurrentBackupType();
    CARDi_Request(p, CARD_REQ_IDENTIFY, 1);
    CARDi_EndTask(p);
}

// @symbol func_0206071c
/* The backup read task func_02060558 runs (the receive mode of pret's
 * CARDi_RequestStreamCommandCore): request up to a page from backup address
 * src into backup_cache_page_buf (CARD_REQ_READ_BACKUP), copy it out to dst,
 * and advance until len is 0 or a request fails; then end the task. */
void func_0206071c(CARDiCommon *p) {
    u32 len;

    do {
        len = p->len;
        if (len > sizeof(p->backup_cache_page_buf))
            len = sizeof(p->backup_cache_page_buf);
        DC_InvalidateRange(p->backup_cache_page_buf, sizeof(p->backup_cache_page_buf));
        p->cmd->src = p->src;
        p->cmd->dest = (u32)p->backup_cache_page_buf;
        p->cmd->len = len;
        if (!CARDi_Request(p, CARD_REQ_READ_BACKUP, 1))
            break;
        MI_CpuCopy8(p->backup_cache_page_buf, (void *)p->dst, len);
        p->src += len;
        p->dst += len;
        p->len -= len;
    } while (p->len != 0);

    CARDi_EndTask(p);
}

// @symbol func_0206062c
/* The backup write task func_02060484 runs (the send-and-verify mode of
 * pret's CARDi_RequestStreamCommandCore): copy up to a page of the source
 * into backup_cache_page_buf, flush it, program it (CARD_REQ_PROGRAM_BACKUP,
 * 10 retries) and verify it, and advance until len is 0 or a request fails;
 * then end the task. */
void func_0206062c(CARDiCommon *p) {
    do {
        u32 len = p->len;
        if (len > sizeof(p->backup_cache_page_buf))
            len = sizeof(p->backup_cache_page_buf);
        MI_CpuCopy8((const void *)p->src, p->backup_cache_page_buf, len);
        DC_FlushRange(p->backup_cache_page_buf, len);
        DC_WaitWriteBufferEmpty();
        p->cmd->src = (u32)p->backup_cache_page_buf;
        p->cmd->dest = p->dst;
        p->cmd->len = len;
        if (!CARDi_Request(p, CARD_REQ_PROGRAM_BACKUP, 10))
            break;
        if (!CARDi_Request(p, CARD_REQ_VERIFY_BACKUP, 1))
            break;
        p->src += len;
        p->dst += len;
        p->len -= len;
    } while (p->len != 0);
    CARDi_EndTask(p);
}

// @symbol func_02060558
/* The backup read request (NitroSDK card_backup.c; this older SDK has one
 * entry point per request where pret's has CARDi_RequestStreamCommand):
 * wait for the card to be free and claim it, record the transfer, then run
 * the read task func_0206071c, on the card thread if is_async. Synchronous
 * calls return whether the request succeeded. */
int func_02060558(u32 src, void *dst, u32 len, MIDmaCallback callback, void *arg, int is_async) {
    CARDiCommon *const p = &cardi_common;

    {
        OSIntrMode mask = OS_DisableInterrupts();
        while (p->flag & CARD_STAT_BUSY)
            OS_SleepThread(&p->busy_q);
        p->flag |= CARD_STAT_BUSY;
        OS_RestoreInterrupts(mask);
    }

    p->src = src;
    p->dst = (u32)dst;
    p->len = len;
    p->callback = callback;
    p->callback_arg = arg;
    if (is_async) {
        CARDi_SetTask(func_0206071c);
        return 1;
    }
    cardi_common.cur_th = OSi_ThreadInfo.current;
    func_0206071c(p);
    return p->cmd->result == CARD_RESULT_SUCCESS;
}

// @symbol func_02060484
/* The backup write request (NitroSDK card_backup.c; this older SDK has one
 * entry point per request where pret's has CARDi_RequestStreamCommand):
 * write len bytes from RAM src to backup address dst. Wait for the card to
 * be free and claim it, record the transfer, then run the write task
 * func_0206062c, on the card thread if is_async. Synchronous calls return
 * whether the request succeeded. */
int func_02060484(u32 dst, const void *src, u32 len, MIDmaCallback callback, void *arg,
                  int is_async) {
    CARDiCommon *const p = &cardi_common;

    {
        OSIntrMode mask = OS_DisableInterrupts();
        while (p->flag & CARD_STAT_BUSY)
            OS_SleepThread(&p->busy_q);
        p->flag |= CARD_STAT_BUSY;
        OS_RestoreInterrupts(mask);
    }

    p->src = (u32)src;
    p->dst = dst;
    p->len = len;
    p->callback = callback;
    p->callback_arg = arg;
    if (is_async) {
        CARDi_SetTask(func_0206062c);
        return 1;
    }
    cardi_common.cur_th = OSi_ThreadInfo.current;
    func_0206062c(p);
    return p->cmd->result == CARD_RESULT_SUCCESS;
}

// @symbol CARD_GetCurrentBackupType
/* NitroSDK CARD_GetCurrentBackupType: word 0 of the selected backup spec
 * (data_020a8760; see CARD_GetBackupTotalSize). */
int CARD_GetCurrentBackupType(void) {
    return data_020a8760->type;
}

// @symbol CARD_GetBackupTotalSize
/* NitroSDK CARD_GetBackupTotalSize: data_020a8760 is the backup spec
 * CARD_IdentifyBackup selected from the table at 0x020867bc ({type,
 * total_size, sect_size, ...}). */
u32 CARD_GetBackupTotalSize(void) {
    return data_020a8760->totalSize;
}

// @symbol CARD_IdentifyBackup
/* NitroSDK CARD_backup.c CARD_IdentifyBackup, an older revision than pret's
 * (no OS_Terminate or CARD_CheckEnabled, no read-back request, and no
 * result): wait for the card to be free and claim it, select the backup
 * spec for the type (func_02060364), clear the callback, then run the
 * identify task func_020607e8 on this thread. */
void CARD_IdentifyBackup(int type) {
    CARDiCommon *const p = &cardi_common;

    {
        OSIntrMode mask = OS_DisableInterrupts();
        while (p->flag & CARD_STAT_BUSY)
            OS_SleepThread(&p->busy_q);
        p->flag |= CARD_STAT_BUSY;
        OS_RestoreInterrupts(mask);
    }

    func_02060364(type);
    p->callback = 0;
    p->callback_arg = 0;
    cardi_common.cur_th = OSi_ThreadInfo.current;
    func_020607e8(p);
}

// @symbol func_02060398
struct CardBackupSpec *func_02060398(int arg) {
    const struct CardBackupSpec *p = data_020867bc;
    for (;;) {
        if (p->type == arg) {
            return (struct CardBackupSpec *)p;
        }
        if (p->type == 0) {
            return 0;
        }
        p++;
    }
}

// @symbol func_02060364
void func_02060364(int arg) {
    struct CardBackupSpec *p = func_02060398(arg);
    data_020a8760 = p;
    data_020a8764 = p->h0e;
}

/* The backup specs, one per CARD backup type this cartridge supports; the
   zero-type entry ends the table. */
const struct CardBackupSpec data_020867bc[] = {
    {0x00000901, 0x00000200, {0x00, 0x02, 0x00, 0x00, 0x10, 0x00}, 0x0001, {0x4f}},
    {0x00000d01, 0x00002000, {0x00, 0x20, 0x00, 0x00, 0x20, 0x00}, 0x0002, {0x4f}},
    {0x00001202,
     0x00040000,
     {0x00, 0x00, 0x01, 0x00, 0x00, 0x01},
     0x0003,
     {0x4f, 0,    0,    0,    0x8a, 0x01, 0,    0,    0x70, 0x12, 0,
      0,    0x98, 0x34, 0x01, 0,    0x98, 0x34, 0x01, 0,    0x70, 0x12}},
    {0},
};
