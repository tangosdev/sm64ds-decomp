#include "types.h"
extern char data_020a2400[];
extern char data_020a1fc0[];
extern void func_02041224(char *thiz, int n);
extern int func_020656d4(u16 arg0, const void *arg1, s32 arg2, u32 arg3);
extern void func_02042160(void);

/* The FSFile being read, with the names func_02040f30.cpp gives it: this
 * ROM's FSFile has its prop union at 0x1c and its arg union at 0x2c. */
typedef struct FSFile {
    char pad_00[0x28];
    u32 pos; /* 0x28 -- prop.file.pos */
    char pad_2c[8];
    u32 len; /* 0x34 -- arg.readfile.len */
} FSFile;

/* The fields of the file-share work block at data_020a1fc0 used here, with
 * include/private/arm9_a1fc0_file_share.h's names. */
typedef struct FileShareWork {
    char pad_00[0x1c];
    int mReadRequest; /* 0x1c -- child read in flight, an FSFile */
    char pad_20[8];
    int mParentPacketSize; /* 0x28 -- parent MP send size */
} FileShareWork;

/* The open request (kind 0) a child sends the parent. func_020412f0 reads it
 * back: it compares the two packet-size bytes with its own mParentPacketSize,
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
void func_02041a00(char *thiz)
{
  Msg m;
  int packetSize;
  int len;
  int pos;
  FSFile *file = (FSFile *)thiz;
  func_02041224(data_020a2400, file->len);
  ((FileShareWork *)data_020a1fc0)->mReadRequest = (int)thiz;
  len = file->len;
  pos = file->pos;
  m.kind = 0;
  m.length = len;
  packetSize = ((FileShareWork *)data_020a1fc0)->mParentPacketSize;
  m.offset = pos;
  m.packetSizeHigh = (packetSize >> 8) & 0xff;
  m.packetSizeLow = (u8)packetSize;
  func_020656d4(1, &m, 9, (u32)func_02042160);
}
