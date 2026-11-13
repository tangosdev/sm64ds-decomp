/* The inter-processor FIFO (the Nitro SDK's pxi_fifo.c), arm9 .text
 * 0x0205b864..0x0205bbdc, .bss 0x020a7fc4..0x020a8048.
 *
 * Each library registers one callback per PXIFifoTag; a set bit in the
 * system work's pxiHandleChecker tells the ARM7 that the ARM9 listens on
 * that tag. PXIi_HandlerRecvFifoNotEmpty drains the receive FIFO on its IRQ
 * and dispatches each word to its tag's callback; a command no one listens
 * for is echoed back with its error bit set.
 *
 * mwccarm emits the bodies in reverse, so PXI_InitFifo sits highest in the
 * ROM and PXIi_HandlerRecvFifoNotEmpty lowest. PXI_SetFifoSendCallback and
 * the empty PXIi_HandlerSendFifoEmpty were dead-stripped. This SDK is older
 * than the ntrtwl/NitroSDK copy: PXI_InitFifo installs and enables the
 * receive IRQ before it enables the FIFO, and does not reset the IRQ's
 * pending request first.
 */
#include "types.h"
#include "nitro/os/interrupt.h"
#include "nitro/os/system.h"
#include "nitro/os/systemWork.h"
#include "nitro/pxi/fifo.h"

typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define NULL 0

#define PXI_PROC_ARM PXI_PROC_ARM9

/* The ARM9 side's PXI registers (nitro/hw/ARM9/ioreg_PXI.h). */
#define reg_PXI_INTF (*(volatile u16 *)0x04000180)
#define reg_PXI_FIFO_CNT (*(volatile u16 *)0x04000184)
#define reg_PXI_SEND_FIFO (*(volatile u32 *)0x04000188)
#define reg_PXI_RECV_FIFO (*(volatile u32 *)0x04100000)

#define REG_PXI_FIFO_CNT_E_MASK 0x8000
#define REG_PXI_FIFO_CNT_ERR_MASK 0x4000
#define REG_PXI_FIFO_CNT_RECV_RI_MASK 0x0400
#define REG_PXI_FIFO_CNT_RECV_EMP_MASK 0x0100
#define REG_PXI_FIFO_CNT_SEND_CL_MASK 0x0008
#define REG_PXI_FIFO_CNT_SEND_FULL_MASK 0x0002

static u16 FifoCtrlInit = 0;
static PXIFifoCallback FifoRecvCallbackTable[PXI_MAX_FIFO_TAG];

static inline PXIFifoStatus PXIi_GetFromFifo(u32 *data_buf);
static inline PXIFifoStatus PXIi_SetToFifo(u32 data);

void PXI_InitFifo(void) {
    int i;
    OSIntrMode enabled;
    OSSystemWork *p = OS_GetSystemWork();

    enabled = OS_DisableInterrupts();

    if (!FifoCtrlInit) {
        FifoCtrlInit = TRUE;

        p->pxiHandleChecker[PXI_PROC_ARM] = 0UL;

        for (i = 0; i < PXI_MAX_FIFO_TAG; i++) {
            FifoRecvCallbackTable[i] = NULL;
        }

        OS_SetIrqFunction(OS_IE_SPFIFO_RECV, PXIi_HandlerRecvFifoNotEmpty);
        OS_EnableIrqMask(OS_IE_SPFIFO_RECV);

        reg_PXI_FIFO_CNT = (REG_PXI_FIFO_CNT_SEND_CL_MASK | REG_PXI_FIFO_CNT_RECV_RI_MASK |
                            REG_PXI_FIFO_CNT_E_MASK | REG_PXI_FIFO_CNT_ERR_MASK);

        {
            int timeout;
            s32 c;

            for (i = 0;; i++) {
                c = reg_PXI_INTF & 15;
                reg_PXI_INTF = (c << 8);

                if (c == 0 && i > 4) {
                    break;
                }

                for (timeout = 1000; (reg_PXI_INTF & 15) == c; timeout--) {
                    if (timeout == 0) {
                        i = 0;
                        break;
                    }
                }
            }
        }
    }
    OS_RestoreInterrupts(enabled);
}

void PXI_SetFifoRecvCallback(int fifotag, PXIFifoCallback callback) {
    OSIntrMode enabled;
    OSSystemWork *p = OS_GetSystemWork();

    enabled = OS_DisableInterrupts();

    FifoRecvCallbackTable[fifotag] = callback;
    if (callback) {
        p->pxiHandleChecker[PXI_PROC_ARM] |= (1UL << fifotag);
    } else {
        p->pxiHandleChecker[PXI_PROC_ARM] &= ~(1UL << fifotag);
    }
    OS_RestoreInterrupts(enabled);
}

BOOL PXI_IsCallbackReady(int fifotag, PXIProc proc) {
    OSSystemWork *p = OS_GetSystemWork();
    return (p->pxiHandleChecker[proc] & (1UL << fifotag)) ? TRUE : FALSE;
}

int PXI_SendWordByFifo(int fifotag, u32 data, BOOL err) {
    PXIFifoMessage fifomsg;

    fifomsg.e.tag = fifotag;
    fifomsg.e.err = err;
    fifomsg.e.data = data;

    return PXIi_SetToFifo(fifomsg.raw);
}

static inline PXIFifoStatus PXIi_SetToFifo(u32 data) {
    OSIntrMode enabled;

    if (reg_PXI_FIFO_CNT & REG_PXI_FIFO_CNT_ERR_MASK) {
        reg_PXI_FIFO_CNT |= (REG_PXI_FIFO_CNT_E_MASK | REG_PXI_FIFO_CNT_ERR_MASK);
        return PXI_FIFO_FAIL_SEND_ERR;
    }

    enabled = OS_DisableInterrupts();
    if (reg_PXI_FIFO_CNT & REG_PXI_FIFO_CNT_SEND_FULL_MASK) {
        OS_RestoreInterrupts(enabled);
        return PXI_FIFO_FAIL_SEND_FULL;
    }

    reg_PXI_SEND_FIFO = data;
    OS_RestoreInterrupts(enabled);
    return PXI_FIFO_SUCCESS;
}

static inline PXIFifoStatus PXIi_GetFromFifo(u32 *data_buf) {
    OSIntrMode enabled;

    if (reg_PXI_FIFO_CNT & REG_PXI_FIFO_CNT_ERR_MASK) {
        reg_PXI_FIFO_CNT |= (REG_PXI_FIFO_CNT_E_MASK | REG_PXI_FIFO_CNT_ERR_MASK);
        return PXI_FIFO_FAIL_RECV_ERR;
    }

    enabled = OS_DisableInterrupts();
    if (reg_PXI_FIFO_CNT & REG_PXI_FIFO_CNT_RECV_EMP_MASK) {
        OS_RestoreInterrupts(enabled);
        return PXI_FIFO_FAIL_RECV_EMPTY;
    }

    *data_buf = reg_PXI_RECV_FIFO;
    OS_RestoreInterrupts(enabled);

    return PXI_FIFO_SUCCESS;
}

void PXIi_HandlerRecvFifoNotEmpty(void) {
    PXIFifoMessage fifomsg;
    PXIFifoStatus ret_code;
    PXIFifoTag tag;

    while (1) {
        ret_code = PXIi_GetFromFifo(&fifomsg.raw);

        if (ret_code == PXI_FIFO_FAIL_RECV_EMPTY)
            break;

        if (ret_code == PXI_FIFO_FAIL_RECV_ERR)
            continue;

        tag = fifomsg.e.tag;

        if (tag) {
            if (FifoRecvCallbackTable[tag]) {
                (FifoRecvCallbackTable[tag])(tag, fifomsg.e.data, fifomsg.e.err);
            } else {
                if (fifomsg.e.err) {
                } else {
                    fifomsg.e.err = TRUE;
                    PXIi_SetToFifo(fifomsg.raw);
                }
            }
        }
    }
}
