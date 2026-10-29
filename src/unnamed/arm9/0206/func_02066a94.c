#include "types.h"
#include "private/arm9_a9db8_mb_parent.h"
extern char *data_020a9db8;

/* Typed through a cast: the other declarations of data_020a9db8 spell it as
 * a byte pointer. */
#define MB_WORK ((MbParentWork *)data_020a9db8)

/* File record i, reached the way the ROM builds its address: the work
 * pointer stepped a whole MbParentFile per index, then mFiles[0] from there
 * (the same stepped form as func_0206655c's MB_FILE). */
#define MB_FILE(i) (((MbParentWork *)(data_020a9db8 + (i) * (int)sizeof(MbParentFile)))->mFiles)

/* Leading fields of NitroSDK WMStartParentCallback, the argument the WM
 * parent callback (func_02069994) passes for CONNECTED (code 0),
 * DISCONNECTED (code 1) and BEACON_SENT (code 0x1c). Codes 0xff (a failed
 * WM call, from func_0206879c: {apiid, errcode}) and 0x100 (any other WM
 * result) read only the WMCallback header the SDK callbacks all share. */
typedef struct WmStartParentCallback {
    u16 apiid;        /* 0x00 */
    u16 errcode;      /* 0x02 */
    u16 wlCmdID;      /* 0x04 */
    u16 wlResult;     /* 0x06 */
    u16 state;        /* 0x08 */
    u8 macAddress[6]; /* 0x0a */
    u16 aid;          /* 0x10 */
} WmStartParentCallback;

extern int func_02066fec(int a, int b, int c);
extern void func_0205a588(void *dst, int val, u32 size);
extern int func_0206881c(void);
extern void func_020664b4(int a);
extern void func_02065f78(void);
extern void func_0206853c(char *self, void *src, u32 flags, int s);
extern void func_02068410(void);
extern void func_02065eb8(void *a, short s);
extern void MultiStore32Bytes(u32 val, void *dst, int len);

void func_02066a94(int arg0, char *arg1)
{
    WmStartParentCallback *cb = (WmStartParentCallback *)arg1;

    switch (arg0) {
    case 0x15:
        func_02066fec(0, 1, (int)arg1);
        break;
    case 0x11:
        func_02066fec(0, 0xc, 0);
        break;
    case 0: {
        u16 v = cb->aid;
        if (v != 0 && v < 0x10) {
            func_02066fec(v, 2, (int)arg1);
        }
        break;
    }
    case 1: {
        u16 v = cb->aid;
        if (v != 0 && v < 0x10) {
            MB_WORK->mChildVersion[v - 1] = 0;
            func_0205a588(&MB_WORK->mChildGameId[cb->aid - 1], 0, 4);
            func_0205a588(MB_WORK->mChildUser[cb->aid - 1], 0, 0x16);
            {
                int cur = func_0206881c();
                func_0205a588(&MB_WORK->mChildRequest[cb->aid - 1], 0,
                              (cur - 2) * 0x1e / (func_0206881c() - 2));
            }
            MB_WORK->mChildRequestPieces[cb->aid - 1] = 0;
            {
                u16 id = cb->aid;
                s8 slot = MB_WORK->mChildFile[id - 1];
                if (slot != -1) {
                    u32 off = (u8)slot * sizeof(MbParentFile);
                    u32 mask = ~(1 << id);
                    ((MbParentWork *)(data_020a9db8 + off))->mFiles->mEntryBitmap &= mask;
                    ((MbParentWork *)(data_020a9db8 + off))->mFiles->mEntryChanged |= (1 << id);
                    MB_WORK->mChildFile[id - 1] = -1;
                    ((MbParentWork *)(data_020a9db8 + off))->mFiles->mChildBitmap &= mask;
                }
            }
            if (MB_WORK->mChildEntryBitmap & (1 << cb->aid)) {
                MB_WORK->mChildCount -= 1;
                MB_WORK->mChildEntryBitmap &= ~(1 << cb->aid);
            }
            {
                u16 id2 = cb->aid;
                if (MB_WORK->mChildState[id2 - 1] == 8) {
                    func_02066fec(id2, 9, 0);
                }
            }
            func_02066fec(cb->aid, 3, (int)arg1);
            MB_WORK->mChildState[cb->aid - 1] = 0;
        }
        break;
    }
    case 3:
        func_020664b4((int)arg1);
        break;
    case 0x19:
        func_02065f78();
        break;
    case 0x1c: {
        u8 i;
        for (i = 0; i < 0x10; i++) {
            MbParentWork *e = (MbParentWork *)(data_020a9db8 + i * (int)sizeof(MbParentFile));
            if (e->mFiles->mActive != 0) {
                u16 f48 = e->mFiles->mEntryChanged;
                if (f48 != 0) {
                    /* the game info record func_0206853c refreshes sits after
                     * the 0xe4 bytes of mHeader that func_020662c0 sends */
                    func_0206853c((char *)&MB_WORK->mFiles[i].mHeader[0xe4], MB_WORK->mChildUser,
                                  e->mFiles->mEntryBitmap, f48);
                    MB_FILE(i)->mEntryChanged = 0;
                }
            }
        }
        func_02068410();
        break;
    }
    case 0xff: {
        u16 v = cb->errcode;
        switch (v) {
        case 1:
        case 4:
        case 5:
        case 6:
        case 8:
        case 9:
            func_02065eb8(0, 9);
            break;
        default:
        case 0:
        case 2:
        case 3:
        case 7:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            func_02065eb8(0, 8);
            break;
        }
        break;
    }
    case 0x100: {
        u16 v = cb->apiid;
        switch (v) {
        case 0:
        case 7:
        case 8:
        case 13:
        case 14:
        case 15:
        case 17:
        case 18:
        case 21:
        case 25:
        case 29:
            func_02065eb8(0, 9);
            break;
        default:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 9:
        case 10:
        case 11:
        case 12:
        case 16:
        case 19:
        case 20:
        case 22:
        case 23:
        case 24:
        case 26:
        case 27:
        case 28:
            func_02065eb8(0, 8);
            break;
        }
        break;
    }
    }

    if (arg0 != 0x11) {
        return;
    }
    {
        volatile int sp0 = 0;
        MultiStore32Bytes(sp0, data_020a9db8, 0x73e0);
    }
    data_020a9db8 = 0;
}
