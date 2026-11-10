#include "types.h"
#include "private/rom_header_view.h"
#include "private/arm9_a9db8_mb_parent.h"
extern void MBi_SetSegmentInfo(void *in, void *tbl, void *dst, void *recvCursor);
extern void CpuCopy8(const void *src, void *dst, u32 size);
extern u32 MBi_defaultLoadSegList[];

/* NitroSDK MB_DownloadFileInfo, the start of MbParentFile.mHeader: the
 * MbDownloadFileInfoHeader fields (as that type names them), then
 * the 0x88-byte authentication code. */
typedef struct MB_DownloadFileInfo {
    u32 arm9EntryAddr;    /* 0x00 */
    u32 arm7EntryAddr;    /* 0x04 */
    u32 padding;          /* 0x08 */
    MbSegmentInfo seg[3]; /* 0x0c */
    u8 auth_code[0x88];   /* 0x3c */
} MB_DownloadFileInfo;

/* NitroSDK MBi_MakeDownloadFileInfo: fill a registered file's download
 * header from the ROM header at the start of its image, one segment row per
 * MBi_defaultLoadSegList entry (ARM9 static, ARM7 static, ROM header), with ARM7
 * segments that cannot be received in place staged upward from 0x022c0000,
 * then copy in the authentication code that follows the image. */
void MBi_MakeDownloadFileInfo(int *out, int *in, void *authCode) {
    int recvCursor;
    MbSegmentInfo *seg;
    u32 *t;
    int i;
    MB_DownloadFileInfo *info = (MB_DownloadFileInfo *)out;
    RomHeaderView *rom = (RomHeaderView *)in;
    recvCursor = 0x22c0000;
    info->arm9EntryAddr = rom->main_entry_address;
    info->arm7EntryAddr = rom->sub_entry_address;
    seg = info->seg;
    t = MBi_defaultLoadSegList;
    for (i = 0; i < 3; i++) {
        MBi_SetSegmentInfo(in, t, seg, &recvCursor);
        seg++;
        t += 1;
    }
    CpuCopy8(authCode, info->auth_code, 0x88);
}
