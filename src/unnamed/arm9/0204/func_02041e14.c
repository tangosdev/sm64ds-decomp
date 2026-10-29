#include "types.h"
#include "private/arm9_a1fc0_file_share.h"

/* One reply in FileShareParentArea::mReplies (0xc bytes): the message kind
 * (an open request 0 becomes reply 1, a release 2 becomes reply 3), the file
 * id in word4, and the parent packet size split across chan and byte8. */
typedef struct {
    u32 kind : 4;
    u32 chan : 4;
    u32 flag : 24;
    u32 word4;
    u8 byte8;
} Cmd;

/* data_020a1fc0 under FileShareWork's names (arm9_a1fc0_file_share.h). Kept
 * as a local object type: the packet size reads through
 * ((FileShareWork *)&data_020a1fc0) take one more register. */
typedef struct {
    int mIsParent;         /* 0x00 */
    int pad04[9];          /* 0x04..0x27 */
    int mParentPacketSize; /* 0x28 */
    int mChildPacketSize;  /* 0x2c */
    char pad30[0x410];     /* 0x30..0x43f */
    char mArea[0x2730];    /* 0x440 -- FileShareParentArea */
} Globals;

extern int func_020659a0(int a, int b);
extern int func_020656d4(u16 arg0, const void *arg1, int arg2, u32 arg3);
extern void func_02042160();
extern Globals data_020a1fc0;

void func_02041e14(void)
{
    FileShareParentArea *s = (FileShareParentArea *)((FileShareWork *)&data_020a1fc0)->mArea;
    Cmd cmd;
    Cmd cmd2;

    if (data_020a1fc0.mIsParent == 0) {
        return;
    }
    if (s->mReplySending != 0) {
        return;
    }

    s->mReplyPending &= s->mConnectedBitmap;
    s->mReplyGroup &= s->mConnectedBitmap;
    s->mOpenChildren &= s->mConnectedBitmap;
    s->mResizeChildren &= s->mConnectedBitmap;

    if (s->mResizePending != 0 && s->mBusyCount == 0) {
        int v = s->mNewPacketSize;
        int a1 = data_020a1fc0.mChildPacketSize;
        int snd;

        s->mResizePending = 0;
        data_020a1fc0.mParentPacketSize = v;
        func_020659a0(v, a1);

        snd = s->mResizeChildren;
        if (snd == 0) {
            return;
        }
        cmd.kind = 1;
        cmd.flag = 0;
        cmd.word4 = 0;
        cmd.chan = (u8)(data_020a1fc0.mParentPacketSize >> 8);
        cmd.byte8 = (u8)data_020a1fc0.mParentPacketSize;
        func_020656d4((u16)snd, &cmd, 9, (u32)func_02042160);
        s->mReplySending = 1;
        s->mResizeChildren = 0;
    } else {
        int mask;
        Cmd *found;
        int cond;
        int i;
        int pri;
        int bit;
        Cmd *p;
        u8 k;
        int c;
        int w4;

        mask = s->mReplyPending;
        if (mask == 0) {
            return;
        }
        pri = s->mReplyGroup;
        found = 0;
        cond = (pri != 0 && pri == (pri & mask));
        if (cond) {
            mask = pri;
        } else {
            mask &= ~pri;
        }

        p = (Cmd *)s->mReplies;
        i = 0;
        for (;;) {
            bit = 1 << i;
            if (bit > mask) {
                break;
            }
            if (bit & mask) {
                if (found == 0) {
                    found = p;
                } else if (found->kind != p->kind || found->word4 != p->word4) {
                    mask &= ~bit;
                }
            }
            p++;
            i++;
        }

        if (cond && mask != pri) {
            s->mReplyGroup = 0;
        }
        if (found == 0) {
            return;
        }

        switch (found->kind) {
        case 0:
            found->kind = 1;
            break;
        case 2:
            found->kind = 3;
            s->mOpenChildren &= ~mask;
            break;
        }

        k = found->kind;
        w4 = found->word4;
        c = data_020a1fc0.mParentPacketSize;
        cmd2.kind = k;
        cmd2.flag = 1;
        cmd2.word4 = w4;
        cmd2.chan = (u8)(c >> 8);
        cmd2.byte8 = (u8)c;
        func_020656d4((u16)mask, &cmd2, 9, (u32)func_02042160);
        s->mReplySending = 1;
        s->mReplyPending &= ~mask;
    }
}
