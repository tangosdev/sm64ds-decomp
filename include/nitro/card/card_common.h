#ifndef NITRO_CARD_COMMON_H
#define NITRO_CARD_COMMON_H
#ifndef __cplusplus
#define CARDi_EndTask _Z13CARDi_EndTaskP11CARDiCommon
#define CARDi_Request _Z13CARDi_RequestP11CARDiCommonii
#define CARDi_TaskThread _Z16CARDi_TaskThreadPv
#define CARD_UnlockBackup _Z17CARD_UnlockBackupt
#define CARD_LockBackup _Z15CARD_LockBackupt
#define CARD_UnlockRom _Z14CARD_UnlockRomt
#define CARD_LockRom _Z12CARD_LockRomt
#define CARDi_WaitAsync _Z15CARDi_WaitAsyncv
#define CARDi_InitCommon _Z16CARDi_InitCommonv
#define CARDi_SetTask _Z13CARDi_SetTaskPFvP11CARDiCommonE
#endif

/*
 * The card access common routines, under the NitroSDK's own names. The
 * bodies are card_common.c, in src/engine/card/CARD_Common.cpp, and
 * card_request.c, whose three functions are still separate files.
 *
 * CARDi_SetTask takes the task the card thread runs. CARDi_LockResource and
 * CARDi_UnlockResource are static in pret's card_common.c and only that file
 * calls them, so they are not declared here. Here they are global:
 * config/arm9/symbols.txt binds both globally, and a static definition fails
 * the intact-TU link check.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

#include "types.h"
#include "nitro/os/thread.h"
#include "nitro/pxi/fifo.h"

#ifdef __cplusplus
#ifdef _MSC_VER
extern "C" {
#endif
#endif
/* NitroSDK MIDmaCallback: run with its argument when a transfer ends. */
typedef void (*MIDmaCallback)(void *);

#define CARD_RESULT_SUCCESS 0
#define CARD_RESULT_TIMEOUT 4

/* CARD_STAT_* bits of CARDiCommon.flag */
#define CARD_STAT_INIT 0x01
#define CARD_STAT_INIT_CMD 0x02
#define CARD_STAT_BUSY 0x04
#define CARD_STAT_TASK 0x08
#define CARD_STAT_RECV 0x10
#define CARD_STAT_REQ 0x20

/* CARD_REQ_*: the request types sent to ARM7 */
#define CARD_REQ_INIT 0
#define CARD_REQ_IDENTIFY 2
#define CARD_REQ_READ_BACKUP 6
#define CARD_REQ_PROGRAM_BACKUP 8
#define CARD_REQ_VERIFY_BACKUP 9

/* NitroSDK CARDiCommandArg (cardi_arg, data_020a8160), this SDK's layout:
 * the request block ARM7 reads and answers in. pret's revision appends the
 * backup device's spec. */
typedef struct CARDiCommandArg {
    s32 result;   /* 0x00 -- CARD_RESULT_* */
    s32 type;     /* 0x04 -- CARDBackupType */
    u32 id;       /* 0x08 */
    u32 src;      /* 0x0c */
    u32 dest;     /* 0x10 */
    u32 len;      /* 0x14 */
    u8 pad_18[8]; /* 0x18 */
} CARDiCommandArg;

struct CARDiCommon;
/* A card-thread task: CARDi_SetTask hands it cardi_common. */
typedef void (*CARDiTask)(struct CARDiCommon *);

/* NitroSDK CARDiCommon (cardi_common, data_020a8180), this SDK's layout.
 * pret's revision adds request-type, retry and mode fields. The card
 * thread's 0x400-byte stack follows it, at 0x020a8360. */
typedef struct CARDiCommon {
    CARDiCommandArg *cmd;            /* 0x00 */
    s32 command;                     /* 0x04 -- the last CARD_REQ_* sent */
    volatile int lock_owner;         /* 0x08 -- OS_LOCK_ID_ERROR when free */
    volatile int lock_ref;           /* 0x0c */
    u16 lock_queue;                  /* 0x10 -- OSThreadQueue */
    u16 pad12;                       /* 0x12 */
    int lock_target;                 /* 0x14 -- CARD_TARGET_* */
    u32 src;                         /* 0x18 */
    u32 dst;                         /* 0x1c */
    u32 len;                         /* 0x20 */
    u32 dma;                         /* 0x24 -- the DMA channel; ~0 for none */
    MIDmaCallback callback;          /* 0x28 */
    void *callback_arg;              /* 0x2c */
    CARDiTask task_func;             /* 0x30 */
    volatile u32 flag;               /* 0x34 -- CARD_STAT_* */
    int priority;                    /* 0x38 */
    OSThread thread;                 /* 0x3c -- the card task thread */
    OSThread *cur_th;                /* 0xd0 -- the thread the request runs on */
    u16 busy_q;                      /* 0xd4 -- OSThreadQueue */
    u16 pad_d6;                      /* 0xd6 */
    u8 dummy[8];                     /* 0xd8 */
    u8 backup_cache_page_buf[0x100]; /* 0xe0 */
} CARDiCommon;

extern CARDiCommon cardi_common;

void CARDi_SetTask(CARDiTask);
void CARDi_InitCommon(void);
int CARDi_WaitAsync(void);
void CARD_LockRom(u16);
void CARD_UnlockRom(u16);
void CARD_LockBackup(u16);
void CARD_UnlockBackup(u16);

#ifdef __cplusplus
extern "C"
#endif
    /* The backup type the game selected: word 0 of its backup spec (ntrtwl/NitroSDK
 * nitro/card/common.h). The SDK returns a CARDBackupType; the
 * definition, src/CARD_GetCurrentBackupType.c, spells it int. */
    int
    CARD_GetCurrentBackupType(void);

#ifdef __cplusplus
extern "C"
#endif
    /* card_request.c */
    void
    CARDi_OnFifoRecv(PXIFifoTag, u32, int);
void CARDi_TaskThread(void *);
int CARDi_Request(CARDiCommon *, s32, s32);

/* pret's CARDi_EndTask(p, TRUE), out of line in this SDK: clear the busy
 * and task bits, wake the waiters, then run the callback. */
void CARDi_EndTask(CARDiCommon *);
#ifdef __cplusplus
#ifdef _MSC_VER
}
#endif
#endif

#endif /* NITRO_CARD_COMMON_H */
