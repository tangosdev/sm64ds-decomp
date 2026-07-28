/* func_020690b0 @ 0x020690b0 (arm9, size 0x868)   [mwccarm 1.2/sp2p3, 2004/b56]
 * Wireless (WM) message-dispatch state machine. data_020a9db4 is the work-buffer
 * base; g+0x1f00 is a 16-entry table of 0x180-byte records, each holding a
 * WMBssDesc copy at +0xc0 (so +0xca/+0xcc/+0xce are the BSSID halfwords, +0xf6
 * gameInfoLength and +0xf8 the 0x80-byte gameInfo).
 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern char *data_020a9db4;
extern char *data_020a9db8;
extern u16 data_020a9db0;
extern char data_020a9dc0[];

extern int func_020621b8(void *fn);
extern void func_0206879c(int a, int b);
extern int func_02061c20(void *p);
extern int func_02063574(void *fn, int a, int b, int c);
extern int func_02061ee4(void *fn, void *p);
extern int func_02061e38(void *fn, int a, int b);
extern int func_02061c88(int a, void *fn, int c);
extern int func_020625fc(void *fn, int a, u16 b, int c, u16 d, u16 e, int f,
                         int g, int h, int i, int j);
extern int func_0206a230(void);
extern int func_0206a29c(void *p);
extern int func_02068a74(void);
extern void func_02069918(void);
extern void _ZN4CP1519InvalidateDataCacheEjj(u32 addr, u32 n);
extern void DMASyncHalfTransfer(u32 ch, void *src, void *dst, u32 n);
extern void MultiCopyHalf(void *src, void *dst, u32 n);

void func_020690b0(char *arg0);

typedef struct {
    u16 w[0x40];
} GameInfo;

typedef struct {
    u8 unk00[0xc0];
    u8 bss[0xa];
    u16 m0;
    u16 m1;
    u16 m2;
    u8 unkd0[0x26];
    u16 gameInfoLength;
    GameInfo gameInfo;
    u8 unk178[8];
} Entry;

#define DISPATCH(a, b) (*(void (**)(int, void *))(g + 0x1000 + 0xe1c))((a), (b))

void func_020690b0(char *arg0)
{
    char *const g = data_020a9db4;

    if (*(u16 *)(g + 0x1e00 + 0x26) != 0) {
        if (*(u16 *)arg0 == 1) {
            if (*(u16 *)(arg0 + 2) == 0) {
                func_0206879c(2, func_020621b8((void *)func_020690b0));
                return;
            }
            *(u16 *)(g + 0x1e00 + 0x26) = 0;
            DISPATCH(0x100, arg0);
            return;
        }
        if (*(u16 *)arg0 != 2) return;
        if (*(u16 *)(arg0 + 2) == 0) {
            *(u8 *)(g + 0x1000 + 0x411) = 0;
            *(u16 *)(data_020a9db8 + 0x1300 + 0x16) = 0;
            DISPATCH(0x11, arg0);
            return;
        }
        *(u16 *)(g + 0x1e00 + 0x26) = 0;
        DISPATCH(0x100, arg0);
        return;
    }

    switch (*(u16 *)arg0) {
    case 0:
        if (*(u16 *)(arg0 + 2) == 0) {
            DISPATCH(0x15, arg0);
            func_0206879c(0x83, func_02061c20(g + 0x1418));
            func_0206879c(0x1d, func_02063574((void *)func_020690b0, 0xffff, 0x28, 5));
            return;
        }
        DISPATCH(0x100, arg0);
        return;

    case 0x1d:
        if (*(u16 *)(arg0 + 2) == 0) {
            *(char **)(data_020a9dc0) = g + 0x1340;
            if (*(u16 *)(data_020a9dc0 + 4) == 0) *(u16 *)(data_020a9dc0 + 4) = 1;
            if (*(u16 *)(data_020a9dc0 + 6) == 0) *(u16 *)(data_020a9dc0 + 6) = 0xc8;
            *(u16 *)(data_020a9dc0 + 8) = 0xffff;
            *(u16 *)(data_020a9dc0 + 0xa) = 0xffff;
            *(u16 *)(data_020a9dc0 + 0xc) = 0xffff;
            *(int *)(g + 0x1000 + 0xee4) = 1;
            *(int *)(g + 0x1000 + 0xee8) = 1;
            func_0206879c(0xa, func_02061ee4((void *)func_020690b0, data_020a9dc0));
            return;
        }
        DISPATCH(0x100, arg0);
        return;

    case 0xa:
        if (*(u16 *)(arg0 + 2) == 0) {
            switch (*(u16 *)(arg0 + 8)) {
            case 3:
                return;
            case 5:
                {
                    Entry *tbl = (Entry *)(g + 0x1f00);
                    int i;
                    for (i = 0; i < *(u16 *)(g + 0x1e00 + 0xe0); i++) {
                        if (*(u16 *)(arg0 + 0xa) == tbl[i].m0 &&
                            *(u16 *)(arg0 + 0xc) == tbl[i].m1 &&
                            *(u16 *)(arg0 + 0xe) == tbl[i].m2) {
                            tbl[i].gameInfoLength = *(u16 *)(arg0 + 0x36);
                            *(GameInfo *)((char *)tbl + i * 0x180 + 0xf8) = *(GameInfo *)(arg0 + 0x38);
                            _ZN4CP1519InvalidateDataCacheEjj((u32)(g + 0x1f00 + i * 0x180), 0xc0);
                            DMASyncHalfTransfer(data_020a9db0, g + 0x1340, g + 0x1f00 + i * 0x180, 0xc0);
                            *(int *)(g + 0x1000 + 0xeec) = i;
                            goto done;
                        }
                    }
                    if (i < 0x10) {
                        *(u16 *)(g + 0x1e00 + 0xe0) = i + 1;
                        MultiCopyHalf(arg0, (char *)tbl + i * 0x180 + 0xc0, 0xb8);
                        _ZN4CP1519InvalidateDataCacheEjj((u32)(g + 0x1f00 + i * 0x180), 0xc0);
                        DMASyncHalfTransfer(data_020a9db0, g + 0x1340, g + 0x1f00 + i * 0x180, 0xc0);
                        *(int *)(g + 0x1000 + 0xeec) = i;
                    }
                }
            done:
                DISPATCH(4, arg0);
                if (*(int *)(g + 0x1000 + 0xee4) == 0) return;
                if (*(int *)(g + 0x1000 + 0xee8) != 0 && func_0206a29c(data_020a9dc0) == 0)
                    func_0206879c(2, func_02068a74());
                func_0206879c(0xa, func_02061ee4((void *)func_020690b0, data_020a9dc0));
                return;
            case 4:
                DISPATCH(5, arg0);
                if (*(int *)(g + 0x1000 + 0xee4) == 0) return;
                if (*(int *)(g + 0x1000 + 0xee8) != 0 && func_0206a29c(data_020a9dc0) == 0)
                    func_0206879c(2, func_02068a74());
                func_0206879c(0xa, func_02061ee4((void *)func_020690b0, data_020a9dc0));
                return;
            default:
                DISPATCH(0x100, arg0);
                return;
            }
        } else {
            DISPATCH(0x100, arg0);
        }
        return;

    case 0xb:
        if (*(u16 *)(arg0 + 2) == 0) {
            func_0206879c(0xc, func_02061e38((void *)func_020690b0,
                                             *(int *)(g + 0x1000 + 0xe20), 0));
            return;
        }
        DISPATCH(0x100, arg0);
        return;

    case 0xc:
        if (*(u16 *)(arg0 + 2) == 0) {
            switch (*(u16 *)(arg0 + 8)) {
            case 6:
                return;
            case 7:
                *(u16 *)(g + 0x1e00 + 0xe2) = *(u16 *)(arg0 + 0xa);
                DISPATCH(6, arg0);
                *(u16 *)(g + 0x1e00 + 0x2a) = 1;
                if (func_02061c88(1, (void *)func_02069918, 0) != 0) return;
                func_0206879c(0xe, func_020625fc((void *)func_020690b0,
                                                 *(int *)(g + 0x1000 + 0x408),
                                                 *(u16 *)(g + 0x1e00 + 0x1a),
                                                 *(int *)(g + 0x1000 + 0x404),
                                                 *(u16 *)(g + 0x1e00 + 0x18),
                                                 (u16)(*(u16 *)(g + 0x1e00 + 0x2c) == 0),
                                                 0, 0, 0, 1, 1));
                return;
            case 9:
                DISPATCH(0xa, arg0);
                *(u16 *)(g + 0x1e00 + 0x2a) = 0;
                return;
            default:
                DISPATCH(0x100, arg0);
                return;
            }
        } else {
            *(u16 *)(g + 0x1e00 + 0xe0) = 0;
            DISPATCH(0xb, arg0);
        }
        return;

    case 0xe:
        switch (*(u16 *)(arg0 + 4)) {
        case 0xa:
            *(u16 *)(g + 0x1e00 + 0x28) = 1;
            if (func_0206a230() == 0) return;
            DISPATCH(0x19, 0);
            return;
        case 0xc:
            return;
        case 0xd:
            return;
        default:
            DISPATCH(0x100, arg0);
            return;
        }

    case 0xf:
        *(u8 *)(g + 0x1000 + 0x410) = 0;
        if (*(u16 *)(arg0 + 2) == 0)
            DISPATCH(8, arg0);
        else if (*(u16 *)(arg0 + 2) == 9)
            DISPATCH(0x29, arg0);
        else
            DISPATCH(0x12, arg0);
        DISPATCH(0x19, 0);
        return;

    case 0x15:
        if (func_0206a230() == 0) return;
        DISPATCH(0x19, 0);
        return;

    case 0x80:
        return;

    default:
        DISPATCH(0x100, arg0);
        return;
    }
}
