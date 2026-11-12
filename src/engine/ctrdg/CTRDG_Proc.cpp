//cpp
/* The cartridge library's ARM7 handshake (the Nitro SDK's ctrdg_proc.c),
 * arm9 .text 0x0206a600..0x0206a928.
 *
 * CTRDG_Init brings up the CTRDG PXI channel, then CTRDGi_InitModuleInfo reads the
 * cartridge's module information with CTRDGi_CallbackForInitModuleInfo as
 * the FIFO callback, and CTRDGi_PulledOutCallback is left in place to catch the ARM7's
 * "cartridge pulled out" message.
 *
 * File and order: pret's pokediamond, built against a later SDK, has this
 * file as CTRDG_proc.s with CTRDGi_CallbackForSetPhi,
 * CTRDG_TerminateForPulledOut, CTRDGi_PulledOutCallback,
 * CTRDGi_CallbackForInitModuleInfo, CTRDGi_InitModuleInfo and CTRDG_Init in
 * that address order. This run has the same functions in the same order,
 * without CTRDGi_CallbackForSetPhi. Its .bss is pret's in pret's order with
 * the two words only the newer code uses (CallbackForSetPhi's flag and
 * PulledOutCallback's isOnTerminate) taken out: the InitModuleInfo flag
 * (data_020a9de4), the CTRDG_Init flag (data_020a9de8), the pulled-out hook
 * (data_020a9dec), then the header copy (data_020a9e00). mwccarm emits a
 * file's bodies in reverse, so the source below runs from CTRDG_Init down.
 *
 * Names: CTRDGi_CallbackForInitModuleInfo and CTRDG_Init are pret's
 * statement for statement. The other three are this SDK's older versions of
 * the functions at the same places in pret's order; an SDK function keeps
 * its name across versions, and each comment says what differs.
 */
#include "nitro/os/cache.h"
#include "nitro/os/interrupt.h"
#include "nitro/ctrdg/ctrdg_common.h"
#include "nitro/mi/memory.h"
#include "nitro/os/system.h"
#include "nitro/hw/registers.h"

/* NitroSDK's inline IME helpers (os/common/interrupt.h): each returns the
 * previous master enable. File-local because an inline body in a shared
 * header renumbers every includer's @NNN symbols. */
static inline int OS_EnableIrq(void) {
    u16 prep = REG_IME;
    REG_IME = 1;
    return prep;
}

static inline int OS_RestoreIrq(int enable) {
    u16 prep = REG_IME;
    REG_IME = (u16)enable;
    return prep;
}

#define PXI_FIFO_TAG_CTRDG 13
#define PXI_PROC_ARM7 1

extern "C" {

extern void PXI_Init(void);
/* PXI_IsCallbackReady, PXI_SetFifoRecvCallback and PXI_SendWordByFifo
 * (src/engine/pxi/PXI_Fifo.cpp). */
extern int PXI_IsCallbackReady(int tag, int proc);
extern void PXI_SetFifoRecvCallback(int tag, void *callback);
extern int PXI_SendWordByFifo(u32 tag, u32 data, u32 reply);
extern void MI_DmaCopy16(unsigned int channel, void *src, void *dst, unsigned int numHalfs);
extern void SVC_WaitByLoop(int n);

extern CTRDGWork CTRDGi_Work;
extern int data_020a9de4;         /* CTRDGi_InitModuleInfo's done flag */
extern int data_020a9de8;         /* CTRDG_Init's done flag */
extern int (*data_020a9dec)(int); /* the hook CTRDGi_PulledOutCallback asks before stopping */
/* data_020a9e00 is a 0xc0-byte copy of the CTRDGHeader at 0x08000000; the
 * DMA in CTRDGi_InitModuleInfo fills its last 0x40 bytes (+0x80) from the cartridge,
 * and the BIOS logo goes into nintendoLogo (data_020a9e04). */
extern int data_020a9e00;
extern int data_020a9e04;

#define CTRDG_MODULE_INFO ((volatile CTRDGModuleInfo *)0x27ffc30)
#define CTRDG_HEADER (*(CTRDGHeader *)&data_020a9e00)

void CTRDGi_InitModuleInfo(void);
void CTRDGi_CallbackForInitModuleInfo(int tag, int data);
void CTRDGi_PulledOutCallback(int tag, int data);
void CTRDG_TerminateForPulledOut(void);

void CTRDG_Init(void) {
    if (data_020a9de8)
        return;
    data_020a9de8 = 1;
    CTRDGi_InitCommon();
    PXI_Init();
    while (PXI_IsCallbackReady(PXI_FIFO_TAG_CTRDG, PXI_PROC_ARM7) == 0)
        ;
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_CTRDG, (void *)CTRDGi_CallbackForInitModuleInfo);
    CTRDGi_InitModuleInfo();
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_CTRDG, 0);
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_CTRDG, (void *)CTRDGi_PulledOutCallback);
    data_020a9dec = 0;
}

/* Older than pret's CTRDGi_InitModuleInfo: copy the cartridge's module ID, the
 * three extended LSI IDs, the maker and game codes into the shared module
 * information at 0x027ffc30 and hand the header copy to the ARM7. It reads
 * the header's last 0x40 bytes with MI_DmaCopy16, and has none of
 * pret's checks of the 0x027fff9b flag or of CTRDG_IsExisting. */
void CTRDGi_InitModuleInfo(void) {
    int sp[4];
    OSIrqMask lastIrq;
    int flag;
    int lastIme;
    int i;
    u32 data;

    if (data_020a9de4 != 0)
        return;
    data_020a9de4 = 1;
    if ((*(volatile u16 *)0x4000300 & 1) == 0)
        return;
    lastIrq = OS_SetIrqMask(OS_IE_SPFIFO_RECV);
    lastIme = OS_EnableIrq();
    CTRDGi_LockByProcessor(CTRDGi_Work.lockID, sp);
    flag = ((int)REG_EXMEMCNT & 0x8000) >> 15;
    CTRDGi_ChangeLatestAccessCycle(sp + 2);
    REG_EXMEMCNT &= ~0x8000;
    MI_DmaCopy16(0, (void *)0x8000080, (void *)((char *)&CTRDG_HEADER + 0x80), 0x40);
    REG_EXMEMCNT = (REG_EXMEMCNT & ~0x8000) | (flag << 15);
    CTRDGi_RestoreAccessCycle(sp + 2);
    CTRDGi_UnlockByProcessor(CTRDGi_Work.lockID, sp);
    {
        volatile CTRDGModuleInfo *cip = CTRDG_MODULE_INFO;
        CTRDGHeader *chd = &CTRDG_HEADER;
        cip->moduleID = chd->moduleID;
        for (i = 0; i < 3; i++)
            cip->exLsiID[i] = chd->exLsiID[i];
        cip->makerCode = chd->makerCode;
        cip->gameCode = chd->gameCode;
    }
    MIi_CpuCopy32((int *)0xffff0020, &data_020a9e04, 0x9c);
    DC_FlushAll();
    data = (((unsigned int)&data_020a9e00 + 0xfe000000) >> 5) << 6 | 1;
    CTRDGi_SendtoPxi(data);
    if (CTRDGi_Work.subpState != 1) {
        do {
            SVC_WaitByLoop(1);
        } while (CTRDGi_Work.subpState != 1);
    }
    (void)OS_RestoreIrq(lastIme);
    (void)OS_SetIrqMask(lastIrq);
}

void CTRDGi_CallbackForInitModuleInfo(int a0, int a1) {
    if ((a1 & 0x3f) == 1) {
        CTRDGi_Work.subpState = 1;
        return;
    }
    OS_Terminate();
}

/* Older than pret's CTRDGi_PulledOutCallback: on the ARM7's pulled-out message
 * (command 0x11) ask the hook data_020a9dec, and stop if it returns nonzero.
 * It has no isOnTerminate flag, so it does not remember having asked. */
void CTRDGi_PulledOutCallback(int unused, int arg) {
    if ((arg & 0x3f) == 0x11) {
        int r = 0;
        if (data_020a9dec)
            r = data_020a9dec(0);
        if (r == 0)
            return;
        CTRDG_TerminateForPulledOut();
    } else {
        OS_Terminate();
    }
}

/* Older than pret's CTRDG_TerminateForPulledOut: send command 2 on the CTRDG
 * FIFO and stop. The send is inlined here (retried until it succeeds)
 * where pret's calls CTRDGi_SendtoPxi. */
void CTRDG_TerminateForPulledOut(void) {
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_CTRDG, 2, 0) != 0)
        ;
    OS_Terminate();
}
}
