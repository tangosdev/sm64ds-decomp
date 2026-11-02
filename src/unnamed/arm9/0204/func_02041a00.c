#include "types.h"
#include "private/arm9_a1fc0_file_share.h"
#include "private/fs_views.h"
extern char data_020a2400[];
extern char data_020a1fc0[];
extern void func_02041224(char *thiz, int n);
extern int func_020656d4(u16 arg0, const void *arg1, s32 arg2, u32 arg3);
extern void func_02042160(void);

typedef struct FSFile FSFile;

/* The open request (kind 0) a child sends the parent: FileShareMsg's layout
 * (arm9_a1fc0_file_share.h), with the region's length and offset where the
 * replies carry mFlag and mFileId. func_020412f0 reads it back: it compares the two packet-size bytes with its own mParentPacketSize,
 * and looks the region up by (offset, length) with func_02041ce0 and
 * func_02041c64. */
typedef struct {
    u32 kind : 4;
    u32 packetSizeHigh : 4; /* bits 8-11 of the packet size */
    u32 length : 24;
    u32 offset;
    u8 packetSizeLow;
} Msg;

/* Child: ask the parent to open the region the FSFile read covers. The
 * receive work is grown to the read's length first, and the file is kept in
 * mReadRequest until func_02040f30 completes it. */
void func_02041a00(FSFile *file) {
    Msg m;
    int packetSize;
    int len;
    int pos;
    func_02041224(data_020a2400, file->arg.readfile.len);
    FILE_SHARE_WORK->mReadRequest = (int)file;
    len = file->arg.readfile.len;
    pos = file->prop.file.pos;
    m.kind = 0;
    m.length = len;
    packetSize = FILE_SHARE_WORK->mParentPacketSize;
    m.offset = pos;
    m.packetSizeHigh = (packetSize >> 8) & 0xff;
    m.packetSizeLow = (u8)packetSize;
    func_020656d4(1, &m, 9, (u32)func_02042160);
}
