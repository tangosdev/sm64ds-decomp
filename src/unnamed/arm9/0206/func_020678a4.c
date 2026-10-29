#include "types.h"
extern void func_020676e0(void *in, void *tbl, void *dst, void *recvCursor);
extern void CpuCopy8(void *src, void *dst, u32 size);
extern int data_0208685c[];

/* NitroSDK MbSegmentInfo, as func_020676e0 fills it (same names as
 * func_02067604's view). */
typedef struct MbSegmentInfo {
    u32 recv_addr;  /* 0x00 */
    u32 load_addr;  /* 0x04 */
    u32 size;       /* 0x08 */
    u32 target : 1; /* 0x0c -- set for the ARM7 segment */
    u32 rsv : 31;
} MbSegmentInfo;

/* NitroSDK MB_DownloadFileInfo, the start of MbParentFile.mHeader: the
 * MbDownloadFileInfoHeader fields (as func_02067604's view names them), then
 * the 0x88-byte authentication code. */
typedef struct MB_DownloadFileInfo {
    u32 arm9EntryAddr;    /* 0x00 */
    u32 arm7EntryAddr;    /* 0x04 */
    u32 padding;          /* 0x08 */
    MbSegmentInfo seg[3]; /* 0x0c */
    u8 auth_code[0x88];   /* 0x3c */
} MB_DownloadFileInfo;

/* The ROM header fields read here: the two entry points. */
typedef struct RomHeaderView {
    u8 pad_00[0x24];
    u32 arm9EntryAddr; /* 0x24 */
    u8 pad_28[0x34 - 0x28];
    u32 arm7EntryAddr; /* 0x34 */
} RomHeaderView;

/* NitroSDK MBi_MakeDownloadFileInfo: fill a registered file's download
 * header from the ROM header at the start of its image, one segment row per
 * data_0208685c entry (ARM9 static, ARM7 static, ROM header), with ARM7
 * segments that cannot be received in place staged upward from 0x022c0000,
 * then copy in the authentication code that follows the image. */
void func_020678a4(int *out, int *in, void *authCode) {
    int recvCursor;
    MbSegmentInfo *seg;
    int *t;
    int i;
    MB_DownloadFileInfo *info = (MB_DownloadFileInfo *)out;
    RomHeaderView *rom = (RomHeaderView *)in;
    recvCursor = 0x22c0000;
    info->arm9EntryAddr = rom->arm9EntryAddr;
    info->arm7EntryAddr = rom->arm7EntryAddr;
    seg = info->seg;
    t = data_0208685c;
    for (i = 0; i < 3; i++) {
        func_020676e0(in, t, seg, &recvCursor);
        seg++;
        t += 1;
    }
    CpuCopy8(authCode, info->auth_code, 0x88);
}
