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
#include "IRQ.h"
#include "nitro/ctrdg/ctrdg_common.h"
#include "nitro/os/spinLock.h"

extern "C" {

// local extern: no header declares these yet.
extern void CpuSet(void *src, void *dst, u32 ctrl); // SVC_CpuClear
extern int PXI_SendWordByFifo(u32 tag, u32 data, u32 reply);
extern void WaitByLoop(int count); // SVC_WaitByLoop

extern CTRDGWork data_020a9de0; /* CTRDGi_Work */

#define HW_CTRDG_ROM 0x08000000
#define CTRDG_IS_ROM_CODE 0x96
/* Where a cartridge that is not ROM code keeps its module ID. */
#define CTRDG_MODULE_ID_IMAGE (*(u16 *)0x0801fffe)

void CTRDGi_InitCommon(void) {
    u32 local = 0;
    CpuSet(&local, &data_020a9de0, 0x5000001);
    data_020a9de0.lockID = (u16)OS_GetLockID();
}

int CTRDG_IsExisting(void) {
    int lockInfo[2];
    int cycle[2];
    int ret = 1;
    CTRDGModuleInfo *cip = (CTRDGModuleInfo *)0x27ffc30;
    CTRDGHeader *chp = (CTRDGHeader *)HW_CTRDG_ROM;

    if (cip->moduleID == 0xffff)
        return 0;
    if (cip->detectPullOut == 1)
        return 0;

    CTRDGi_LockByProcessor(data_020a9de0.lockID, lockInfo);
    CTRDGi_ChangeLatestAccessCycle(cycle);

    if ((chp->isRomCode == CTRDG_IS_ROM_CODE && cip->moduleID != chp->moduleID) ||
        (chp->isRomCode != CTRDG_IS_ROM_CODE && cip->moduleID != CTRDG_MODULE_ID_IMAGE) ||
        (cip->gameCode != chp->gameCode && cip->isAgbCartridge)) {
        ret = 0;
        cip->detectPullOut = 1;
    }

    CTRDGi_RestoreAccessCycle(cycle);
    CTRDGi_UnlockByProcessor(data_020a9de0.lockID, lockInfo);
    return ret;
}

void CTRDGi_ChangeLatestAccessCycle(int *cycle) {
    cycle[0] = (*(volatile u16 *)0x4000204 & 0xc) >> 2;
    cycle[1] = (*(volatile u16 *)0x4000204 & 0x10) >> 4;
    *(volatile u16 *)0x4000204 = (*(volatile u16 *)0x4000204 & ~0xc) | 0xc;
    *(volatile u16 *)0x4000204 = *(volatile u16 *)0x4000204 & ~0x10;
}

void CTRDGi_RestoreAccessCycle(int *cycle) {
    volatile u16 *reg = (volatile u16 *)0x4000204;
    *reg = (cycle[0] << 2) | (*reg & ~0xc);
    *reg = (cycle[1] << 4) | (*reg & ~0x10);
}

void CTRDGi_LockByProcessor(u16 lockID, int *info) {
    for (;;) {
        info[1] = IRQ::Disable();
        info[0] = OS_ReadOwnerOfLockCartridge() & OS_MAINP_LOCKED_FLAG;
        if (info[0] != 0)
            return;
        if (OS_TryLockCartridge(lockID) == 0)
            return;
        IRQ::Restore(info[1]);
        WaitByLoop(1);
    }
}

void CTRDGi_UnlockByProcessor(u16 lockID, int *info) {
    if (info[0] == 0)
        OS_UnLockCartridge(lockID);
    IRQ::Restore(info[1]);
}

void CTRDGi_SendtoPxi(u32 data) {
    while (PXI_SendWordByFifo(0xd, data, 0) != 0)
        WaitByLoop(1);
}
}
