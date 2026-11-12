//cpp
/* Cartridge-slot common code (the Nitro SDK's ctrdg_common.c),
 * arm9 .text 0x0206a318..0x0206a600.
 *
 * pret/pokediamond's NitroSDK CTRDG_common.c has these seven functions in this
 * order, with the same bodies: CTRDGi_InitCommon, CTRDG_IsExisting,
 * CTRDGi_ChangeLatestAccessCycle, CTRDGi_RestoreAccessCycle,
 * CTRDGi_LockByProcessor, CTRDGi_UnlockByProcessor, CTRDGi_SendtoPxi. The
 * rest of that file (the CTRDG_IsAgbCartridge family, CTRDG_IsPulledOut, the
 * CTRDG_CpuCopy and CTRDG_Read routines, CTRDG_Enable) is not in this ROM.
 *
 * mwccarm defers C bodies and emits them in reverse, so the ROM runs
 * bottom-up: CTRDGi_SendtoPxi at 0x0206a318, CTRDGi_InitCommon at 0x0206a5c0.
 *
 * The lock-info and access-cycle records are int arrays; see
 * nitro/ctrdg/ctrdg_common.h.
 */
#include "types.h"
#include "nitro/os/system.h"
#include "nitro/ctrdg/ctrdg_common.h"
#include "nitro/os/spinLock.h"
#include "nitro/hw/registers.h"
#include "nitro/pxi/fifo.h"

#define FALSE 0

extern "C" {

// local extern: no header declares these yet.
extern void SVC_CpuSet(void *src, void *dst, u32 ctrl); // SVC_CpuClear
extern void SVC_WaitByLoop(int count);

extern CTRDGWork CTRDGi_Work;


void CTRDGi_InitCommon(void) {
    u32 local = 0;
    SVC_CpuSet(&local, &CTRDGi_Work, 0x5000001);
    CTRDGi_Work.lockID = (u16)OS_GetLockID();
}

int CTRDG_IsExisting(void) {
    int lockInfo[2];
    int cycle[2];
    int ret = 1;
    CTRDGModuleInfo *cip = CTRDGi_GetModuleInfoAddr();
    CTRDGHeader *chp = (CTRDGHeader *)HW_CTRDG_ROM;

    if (cip->moduleID == 0xffff)
        return 0;
    if (cip->detectPullOut == 1)
        return 0;

    CTRDGi_LockByProcessor(CTRDGi_Work.lockID, lockInfo);
    CTRDGi_ChangeLatestAccessCycle(cycle);

    if ((chp->isRomCode == CTRDG_IS_ROM_CODE && cip->moduleID != chp->moduleID) ||
        (chp->isRomCode != CTRDG_IS_ROM_CODE && cip->moduleID != *CTRDGi_GetModuleIDImageAddr()) ||
        (cip->gameCode != chp->gameCode && cip->isAgbCartridge)) {
        ret = 0;
        cip->detectPullOut = 1;
    }

    CTRDGi_RestoreAccessCycle(cycle);
    CTRDGi_UnlockByProcessor(CTRDGi_Work.lockID, lockInfo);
    return ret;
}

void CTRDGi_ChangeLatestAccessCycle(int *cycle) {
    cycle[0] = (REG_EXMEMCNT & 0xc) >> 2;
    cycle[1] = (REG_EXMEMCNT & 0x10) >> 4;
    REG_EXMEMCNT = (REG_EXMEMCNT & ~0xc) | 0xc;
    REG_EXMEMCNT = REG_EXMEMCNT & ~0x10;
}

void CTRDGi_RestoreAccessCycle(int *cycle) {
    REG_EXMEMCNT = (cycle[0] << 2) | (REG_EXMEMCNT & ~0xc);
    REG_EXMEMCNT = (cycle[1] << 4) | (REG_EXMEMCNT & ~0x10);
}

void CTRDGi_LockByProcessor(u16 lockID, int *info) {
    for (;;) {
        info[1] = OS_DisableInterrupts();
        info[0] = OS_ReadOwnerOfLockCartridge() & OS_MAINP_LOCKED_FLAG;
        if (info[0] != 0)
            return;
        if (OS_TryLockCartridge(lockID) == 0)
            return;
        OS_RestoreInterrupts((OSIntrMode)info[1]);
        SVC_WaitByLoop(1);
    }
}

void CTRDGi_UnlockByProcessor(u16 lockID, int *info) {
    if (info[0] == 0)
        OS_UnLockCartridge(lockID);
    OS_RestoreInterrupts((OSIntrMode)info[1]);
}

void CTRDGi_SendtoPxi(u32 data) {
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_CTRDG, data, FALSE) != PXI_FIFO_SUCCESS) {
        SVC_WaitByLoop(1);
    }
}
}
