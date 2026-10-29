//cpp
/* func_020412f0 @ 0x020412f0 (arm9, size 0x46c)   [mwccarm 2004/b56]
 * Lever (notes 6y.1 refinement): the byte-triple store into data_020a2409 needs the
 * pooled base in r3 and the derived pointer (base + r6) in r2; every plain spelling
 * inverts them and stalls at 5 divergences. Three things must hold at once: the base
 * is a NAMED local, d9 is derived from that local rather than from the array symbol
 * again, and the zero-instruction self-select booster sits on the BASE (the shorter-
 * lived web), after the volatile temps and immediately before the first store.
 * Boosting the DERIVED pointer is inert at all placements. Here the booster raises a
 * web toward the HIGHER-numbered scratch register, opposite to the callee-saved cases
 * in 6y. Do not drop `b9b = b9b ? b9b : b9b;` or fold b9b back to the array symbol.
 * Also improves 1.2/base|sp2|sp2p3 from 12 divergences to 7, so it is a better source
 * shape rather than a b56 accident. Link-verified: VERIFIED, 0 diffs, 0 blind.
 */
#include "private/arm9_a1fc0_file_share.h"
#include "private/arm9_wbt.h"
#include "IRQ.h"

extern "C" {

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;

extern int func_02065a4c(int x);
extern void *func_02041ce0(void *base, int a, int b);
extern void *func_02041c64(void *base, int a, int b);
extern void *func_02041c2c(void *base, int key);
extern void func_020657fc(int key);
extern void func_02041ae8(void *base, void *node);
extern void func_020423c8(void);
extern void func_0204175c(void);
extern int func_0205d368(void *o, int a, int b);
extern void func_0205d3e4(void *o, int a, int b);

extern char data_020a2400[];
extern char data_020a2404[];
extern u8 data_020a2408[];
extern u8 data_020a2409[];
extern char data_020a1fc0[];
extern int data_020a0f8c;

typedef struct {
    u32 f0 : 4;
    u32 f4 : 4;
    u32 f8 : 24;
    u32 w1;
    u8 b8;
    u8 b9;
    u8 ba;
    u8 bb;
    s16 hc;
} Msg;

void func_020412f0(char *thiz)
{
    int sb;
    int type;
    u32 r6;
    FileShareNode *p;
    WBTPrepareSendDataCallback *n;
    WBTCommand *cmd = (WBTCommand *)thiz;

    sb = func_02065a4c(cmd->peer_bmp);
    if (cmd->command != 0xc)
        return;

    type = cmd->event;

    switch (type) {
    case 2:
        break;

    case 8:
        break;

    case 0xa: {
        r6 = sb * 0xc;
        Msg *m = (Msg *)cmd->u.user_data.data;
        char *base = data_020a2400;
        FileShareParentArea *area = (FileShareParentArea *)base;
        int kind;

        *(int *)(base + r6) = *(int *)((Msg *)((int)m));
        *(int *)(data_020a2404 + r6) = m->w1;
        data_020a2408[r6] = m->b8;
        {
            u8 *b9b = data_020a2409;
            u8 *d9 = b9b + r6;
            u8 b9v = ((volatile Msg *)m)->b9;
            u8 bav = ((volatile Msg *)m)->ba;
            b9b = b9b ? b9b : b9b;
            b9b[r6] = b9v;
            d9[1] = bav;
            d9[2] = m->bb;
        }

        kind = ((Msg *)((int)m))->f0;

        switch (kind) {
        case 0: {
            u32 nib1 = m->f8;
            u32 unk18v = m->w1;
            int val = (m->f4 << 8) | m->b8;

            if (area->mResizePending != 0 || val != FILE_SHARE_WORK->mParentPacketSize) {
                area->mResizePending = 1;
                area->mResizeChildren |= (1 << sb);
                area->mOpenChildren |= (1 << sb);
            } else {
                FileShareNode *p = (FileShareNode *)func_02041ce0(base, unk18v, nib1);

                if (p != 0) {
                    area->mReplyPending |= (1 << sb);
                    *(int *)(base + r6 + 4) = p->mFileId;
                    p->mRefCount += 1;
                } else {
                    FileShareNode *q = (FileShareNode *)func_02041c64(base, unk18v, nib1);
                    q->mWaitingChildren |= (1 << sb);
                }

                area->mOpenChildren |= (1 << sb);
            }
            break;
        }
        case 2: {
            u32 saved = IRQ::Disable();
            FileShareNode *p;

            area->mReplyPending |= (1 << sb);
            p = (FileShareNode *)func_02041c2c(base, m->w1);

            if (p != 0) {
                p->mRefCount -= 1;
                if (p->mRefCount <= 0) {
                    p->mWaitingChildren = 0;
                    func_020657fc(m->w1);
                    func_02041ae8(base, p);
                    func_020423c8();
                }
            }

            IRQ::Restore(saved);
            break;
        }
        }

        break;
    }

    case 0xd: {
        char *base = data_020a2400;
        int key;

        n = &cmd->u.prepare_send_data;
        key = n->block_id;
        int shortVal, p94, unk18v;
        int i;

        data_020a0f8c += 1;
        if (data_020a0f8c >= 0x3c) {
            func_0204175c();
            data_020a0f8c = 0;
        }

        n->data_ptr = 0;
        p = (FileShareNode *)func_02041c2c(base, key);
        if (p == 0)
            break;

        shortVal = n->own_packet_size;
        p94 = p->mPrevSeqNo;
        unk18v = n->block_seq_no;

        if (p->mLoadingPage != 3) {
            /* mFile's FSFile stat, bit 0: its read is still in flight */
            int t = (*(int *)(p->mFile + 0xc) & 1) ? 1 : 0;
            if (t == 0)
                p->mLoadingPage = 3;
        }

        {
            int prod = shortVal * p94;
            int cnt2;

            p->mPrevSeqNo = unk18v;
            cnt2 = p->mLoadingPage;

            for (i = 0; i < 3; i++) {
                if (i != cnt2) {
                    int rem = prod - p->mPageOffset[i];
                    if (rem >= 0 && rem + shortVal <= 0x400) {
                        n->block_seq_no = p94;
                        n->data_ptr = p->mPages[i] + rem;
                        data_020a0f8c = 0;
                        break;
                    }
                }
            }
        }

        if (p->mLoadingPage != 3)
            break;

        {
            int j;
            int prod2 = shortVal * unk18v;

            for (j = 0; j < 3; j++) {
                int rem = prod2 - p->mPageOffset[j];

                if (rem >= 0 && rem + shortVal <= 0x400)
                    break;
            }

            if (j < 3)
                break;

            {
                int nxt = p->mLastRefillPage;
                int masked;

                do {
                    nxt++;
                    if (nxt >= 3)
                        nxt = 0;
                } while (nxt == i);

                p->mLastRefillPage = nxt;
                p->mLoadingPage = nxt;
                masked = prod2 & -0x200;
                p->mPageOffset[nxt] = masked;
                func_0205d368(p->mFile, masked, 0);
                func_0205d3e4(p->mFile, (int)p->mPages[nxt], 0x400);
            }
        }

        break;
    }
    }
}
}
