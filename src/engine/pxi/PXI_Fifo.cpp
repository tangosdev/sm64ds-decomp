//cpp
/* PXI FIFO (SDK pxi_fifo.c), .text 0x0205b864..0x0205bbdc.
 * The ARM9 side of the ARM7 command FIFO. A FIFO word packs a 5-bit channel
 * tag, a reply flag and 26 bits of data. Each channel has one callback in
 * data_020a7fc8, and a set bit in the system work area's pxiHandleChecker
 * word tells the ARM7 that the ARM9 listens on that channel.
 *
 * mwccarm emits .text in reverse source order, so the file reads from the
 * top of the run down: init, set callback, ready test, send, receive IRQ.
 *
 * leftovers: the callback table and the init flag (data_020a7fc4) are not
 * claimed here.
 */
#include "nitro/os/system.h"
#include "nitro/os/interrupt.h"
#include "nitro/os/systemWork.h"

typedef void (*PxiCallback)(u32 tag, u32 data, u32 reply);

extern "C" {
extern u16 data_020a7fc4;           /* set once the FIFO is initialised */
extern PxiCallback data_020a7fc8[]; /* one callback per channel tag */
}

typedef union {
    u32 raw;
    struct {
        u32 tag : 5;   /* callback slot; 0 is unused */
        u32 reply : 1; /* set on reply words */
        u32 data : 26;
    } f;
} PxiFifoWord;

#define REG_SUBPINTF (*(volatile u16 *)0x04000180)
#define REG_SUBP_FIFO_CNT (*(volatile u16 *)0x04000184)
#define REG_SEND_FIFO (*(volatile u32 *)0x04000188)
#define REG_RECV_FIFO (*(volatile u32 *)0x04100000)

#define FIFO_CNT_SEND_FULL 0x0002
#define FIFO_CNT_RECV_EMPTY 0x0100
#define FIFO_CNT_ERROR 0x4000       /* recv-empty read or send-full write */
#define FIFO_CNT_ERROR_CLEAR 0xc000 /* clears the error, flushes send */

extern "C" {

void PXIi_HandlerRecvFifoNotEmpty(void);

/* PXI_InitFifo: clear the callback table, hook the receive IRQ, enable the
 * FIFO, then handshake with the ARM7 through the SUBPINTF nibble. */
void PXI_InitFifo(void) {
    OSIntrMode state;
    int i;
    int r5;
    int lr;
    OSSystemWork *p = OS_GetSystemWork();

    volatile u16 *reg;
    int ip;

    state = OS_DisableInterrupts();
    if (data_020a7fc4 == 0) {
        data_020a7fc4 = 1;
        p->pxiHandleChecker[0] = 0;
        for (i = 0; i < 0x20; i++) {
            data_020a7fc8[i] = 0;
        }
        OS_SetIrqFunction(OS_IE_SPFIFO_RECV, PXIi_HandlerRecvFifoNotEmpty);
        OS_EnableIrqMask(OS_IE_SPFIFO_RECV);
        REG_SUBP_FIFO_CNT = 0xc408;

        ip = 0;
        reg = &REG_SUBPINTF;
        for (;;) {
            lr = *reg & 0xf;
            *reg = (u16)(lr << 8);
            if (lr == 0) {
                if (ip > 4)
                    break;
            }
            r5 = 0x3e8;
            if ((*reg & 0xf) == lr) {
                for (;;) {
                    if (r5 <= 0) {
                        ip = 0;
                        break;
                    }
                    r5--;
                    if ((*reg & 0xf) != lr)
                        break;
                }
            }
            ip++;
        }
    }
    OS_RestoreInterrupts(state);
}

/* PXI_SetFifoRecvCallback. Every caller passes the callback as a void *. */
void PXI_SetFifoRecvCallback(int tag, void *callback) {
    OSIntrMode saved = OS_DisableInterrupts();
    OSSystemWork *p = OS_GetSystemWork();
    data_020a7fc8[tag] = (PxiCallback)callback;
    if (callback != 0) {
        p->pxiHandleChecker[0] |= (1u << tag);
    } else {
        p->pxiHandleChecker[0] &= ~(1u << tag);
    }
    OS_RestoreInterrupts(saved);
}

/* PXI_IsCallbackReady: is `tag` registered on processor `proc`? */
int PXI_IsCallbackReady(int tag, int proc) {
    OSSystemWork *p = OS_GetSystemWork();
    return (((1 << tag) & p->pxiHandleChecker[proc]) != 0);
}

/* PXI_SendWordByFifo: 0 on success, -1 on a FIFO error, -2 if full. */
int PXI_SendWordByFifo(unsigned int tag, unsigned int data, unsigned int reply) {
    PxiFifoWord word;
    OSIntrMode saved;

    word.f.tag = tag;
    word.f.reply = reply;
    word.f.data = data;

    if (REG_SUBP_FIFO_CNT & FIFO_CNT_ERROR) {
        REG_SUBP_FIFO_CNT |= FIFO_CNT_ERROR_CLEAR;
        return -1;
    }

    saved = OS_DisableInterrupts();

    if (REG_SUBP_FIFO_CNT & FIFO_CNT_SEND_FULL) {
        OS_RestoreInterrupts(saved);
        return -2;
    }

    REG_SEND_FIFO = word.raw;
    OS_RestoreInterrupts(saved);
    return 0;
}

/* PXIi_HandlerRecvFifoNotEmpty: drain the receive FIFO, dispatching each
 * word to its channel callback. An unhandled command (not a reply) is
 * echoed back to the ARM7 with bit 5 set. */
void PXIi_HandlerRecvFifoNotEmpty(void) {
    PxiFifoWord word;
    int code;
    OSIntrMode saved;
    for (;;) {
        if (REG_SUBP_FIFO_CNT & FIFO_CNT_ERROR) {
            REG_SUBP_FIFO_CNT = REG_SUBP_FIFO_CNT | FIFO_CNT_ERROR_CLEAR;
            code = -3;
        } else {
            saved = OS_DisableInterrupts();
            if (REG_SUBP_FIFO_CNT & FIFO_CNT_RECV_EMPTY) {
                OS_RestoreInterrupts(saved);
                code = -4;
            } else {
                word.raw = REG_RECV_FIFO;
                OS_RestoreInterrupts(saved);
                code = 0;
            }
        }
        if (code == -4)
            return;
        if (code == -3)
            continue;
        {
            PxiCallback fn;
            if (word.f.tag == 0)
                continue;
            fn = data_020a7fc8[word.f.tag];
            if (fn != 0) {
                fn(word.f.tag, word.f.data, word.f.reply);
                continue;
            }
            if (word.f.reply)
                continue;
            word.raw = word.raw | 0x20;
            if (REG_SUBP_FIFO_CNT & FIFO_CNT_ERROR) {
                REG_SUBP_FIFO_CNT = REG_SUBP_FIFO_CNT | FIFO_CNT_ERROR_CLEAR;
                continue;
            }
            saved = OS_DisableInterrupts();
            if (REG_SUBP_FIFO_CNT & FIFO_CNT_SEND_FULL) {
                OS_RestoreInterrupts(saved);
                continue;
            }
            REG_SEND_FIFO = word.raw;
            OS_RestoreInterrupts(saved);
        }
    }
}

}
