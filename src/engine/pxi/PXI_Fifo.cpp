//cpp
/* PXI FIFO (SDK pxi_fifo.c), .text 0x0205b864..0x0205bbdc.
 * The ARM9 side of the ARM7 command FIFO. A FIFO word packs a 5-bit channel
 * tag, a reply flag and 26 bits of data. Each channel has one callback in
 * data_020a7fc8, and a set bit in the shared system-work word at 0x027fff88
 * tells the ARM7 that the ARM9 listens on that channel.
 *
 * mwccarm emits .text in reverse source order, so the file reads from the
 * top of the run down: init, set callback, ready test, send, receive IRQ.
 *
 * leftovers: the callback table and the init flag (data_020a7fc4) are not
 * claimed here; the four C-linkage members keep their address names.
 */
#include "IRQ.h"

namespace IRQ {

u32 Disable(void);
u32 Restore(u32 saved);
void IPCRxFifoNotEmptyHandler(void);

}

typedef void (*PxiCallback)(u32 tag, u32 data, u32 reply);

extern "C" {
extern u16 data_020a7fc4;           /* set once the FIFO is initialised */
extern PxiCallback data_020a7fc8[]; /* one callback per channel tag */
}

/* The OS system-work block shared with the ARM7 (main RAM 0x027ffc00). */
struct SystemWork {
    u8 pad[0x388];
    u32 pxiHandleChecker[2]; /* [0] ARM9, [1] ARM7: listening tags */
};
#define SYSTEM_WORK ((SystemWork *)0x027ffc00)

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

#define IRQ_RECV_FIFO_NOT_EMPTY 0x40000

extern "C" {

/* PXI_InitFifo: clear the callback table, hook the receive IRQ, enable the
 * FIFO, then handshake with the ARM7 through the SUBPINTF nibble. */
// @symbol func_0205bad8
void func_0205bad8(void) {
    u32 state;
    int i;
    int r5;
    int lr;
    SystemWork *work;

    volatile u16 *reg;
    int ip;

    state = IRQ::Disable();
    if (data_020a7fc4 == 0) {
        data_020a7fc4 = 1;
        work = SYSTEM_WORK;
        work->pxiHandleChecker[0] = 0;
        for (i = 0; i < 0x20; i++) {
            data_020a7fc8[i] = 0;
        }
        IRQ::SetIRQHandler(IRQ_RECV_FIFO_NOT_EMPTY, IRQ::IPCRxFifoNotEmptyHandler);
        IRQ::EnableIRQs(IRQ_RECV_FIFO_NOT_EMPTY);
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
    IRQ::Restore(state);
}

/* PXI_SetFifoRecvCallback. Every caller passes the callback as a void *. */
// @symbol func_0205ba64
void func_0205ba64(int tag, void *callback) {
    u32 saved = IRQ::Disable();
    SystemWork *work = SYSTEM_WORK;
    data_020a7fc8[tag] = (PxiCallback)callback;
    if (callback != 0) {
        work->pxiHandleChecker[0] |= (1u << tag);
    } else {
        work->pxiHandleChecker[0] &= ~(1u << tag);
    }
    IRQ::Restore(saved);
}

/* PXI_IsCallbackReady: is `tag` registered on processor `proc`? */
// @symbol func_0205ba3c
int func_0205ba3c(int tag, int proc) {
    SystemWork *work = SYSTEM_WORK;
    return (((1 << tag) & work->pxiHandleChecker[proc]) != 0);
}

/* PXI_SendWordByFifo: 0 on success, -1 on a FIFO error, -2 if full. */
// @symbol IPCSend
int IPCSend(unsigned int tag, unsigned int data, unsigned int reply) {
    PxiFifoWord word;
    u32 saved;

    word.f.tag = tag;
    word.f.reply = reply;
    word.f.data = data;

    if (REG_SUBP_FIFO_CNT & FIFO_CNT_ERROR) {
        REG_SUBP_FIFO_CNT |= FIFO_CNT_ERROR_CLEAR;
        return -1;
    }

    saved = IRQ::Disable();

    if (REG_SUBP_FIFO_CNT & FIFO_CNT_SEND_FULL) {
        IRQ::Restore(saved);
        return -2;
    }

    REG_SEND_FIFO = word.raw;
    IRQ::Restore(saved);
    return 0;
}
}

namespace IRQ {

/* PXIi_HandlerRecvFifoNotEmpty: drain the receive FIFO, dispatching each
 * word to its channel callback. An unhandled command (not a reply) is
 * echoed back to the ARM7 with bit 5 set. */
// @symbol _ZN3IRQ24IPCRxFifoNotEmptyHandlerEv
void IPCRxFifoNotEmptyHandler(void) {
    PxiFifoWord word;
    int code;
    u32 saved;
    for (;;) {
        if (REG_SUBP_FIFO_CNT & FIFO_CNT_ERROR) {
            REG_SUBP_FIFO_CNT = REG_SUBP_FIFO_CNT | FIFO_CNT_ERROR_CLEAR;
            code = -3;
        } else {
            saved = Disable();
            if (REG_SUBP_FIFO_CNT & FIFO_CNT_RECV_EMPTY) {
                Restore(saved);
                code = -4;
            } else {
                word.raw = REG_RECV_FIFO;
                Restore(saved);
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
            saved = Disable();
            if (REG_SUBP_FIFO_CNT & FIFO_CNT_SEND_FULL) {
                Restore(saved);
                continue;
            }
            REG_SEND_FIFO = word.raw;
            Restore(saved);
        }
    }
}

}
