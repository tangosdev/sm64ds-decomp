//cpp
#include "types.h"
#include "private/mb_views.h"
#include "nitro/mi/memory.h"
#include "private/player_input_record.h"
#include "nitro/mb/mb.h"
#include "private/wm_views.h"
#include "private/arm9_a0f90_channel_measure.h"
#include "Memory.h"
#include "private/fs_views.h"
#include "nitro/fs/archive.h"
#include "nitro/wm/wm_api.h"
#include "nitro/pad/pad.h"
#include "nitro/spi/tp.h"
#include "nitro/os/thread.h"
#include "nitro/os/tick.h"
#include "nitro/wm/wm.h"
#include "private/arm9_a1fc0_file_share.h"
#include "nitro/os/cache.h"
#include "nitro/os/system.h"
#include "nitro/fs/file.h"
#include "nitro/card/card_rom.h"
#include "private/file_share_alloc.h"
#include "nitro/fs/overlay.h"
#include "private/arm9_wbt.h"
#include "nitro/hw/registers.h"
#include "nitro/os/interrupt.h"
#include "WmWork.h"

/* The link module and the wireless layer under it: the input record each
   console samples every frame (data_020a1040), the four players' copies of
   it (data_020a1154), the link's status and error bits (data_020a0f1c) and
   their accessors, the per-frame step for each link mode (func_0203df40:
   local only, wireless, download-play parent), and below them the WM
   parent/child state machine, the measured-channel pick, data sharing, the
   WBT file share and the child's "rom" archive over it. The name is coined
   from the first address; no file name for it survives in the ROM.

   One translation unit, 0x0203d7b8..0x02042804, 121 functions:
   - Start: cstd::sqrt (0x0203d744..0x0203d7b8) is its own unit with its
     own .init and .ctors, and func_0203d7b8 is the first function after it.
   - End: SaveData2 (0x02042804..0x02042fe4) is its own unit; its private
     magic data_020a4b40 is used by its three functions and nothing else.
   - Data: the bss run from 0x020a0ef0 (the status words, the send packet
     data_020a1020, the two input records, the parent parameters
     data_020a1064 and the download-play profiles data_020a10a4/10fc) is
     used by the link half and the wireless half alike, their words
     interleaved.
   - Static init: sEmptyInit's empty constructor gives the file its .init
     (the bx lr at 0x02075150) and .ctors word (0x02086bb4). Both sit
     directly after cstd::sqrt's (0x0207511c, 0x02086bb0), the unit before.
   - Order: mwccarm emits a file's functions in reverse source order, so
     the file runs from the highest address (func_020427f8) down.
     func_0203ea5c also carries func_0203da4c's body inlined, and only a
     function in the same file can be inlined.

   Everything here has C linkage; it is C++ for Memory::Allocate
   (func_0203e20c, func_0203f604, func_020408b0) and the
   file-scope object. */

#define HW_SYSTEM_CLOCK 33514000
#define gGameCardHandlerPtr data_0209a03c
#define gGameCardHandlerArg data_02099e24
#define gGameCardSentinel   data_020a0f38

typedef struct {
    int start;
    int end;
} Entry;
struct Pair {
    int *f0;
    int f4;
};
typedef struct {
    char stack[0x400];
    OSThread thread;
    /* 0x494, just past the OSThread (0x94 bytes in this SDK): WFSi_CreateTaskThread
       clears it before creating the thread; nothing in src/ reads it */
    u16 mFlag;
} LoaderThreadBlock;
typedef struct {
    u32 kind : 4;
    u32 packetSizeHigh : 4; /* bits 8-11 of the packet size */
    u32 length : 24;
    u32 offset;
    u8 packetSizeLow;
} OpenRequest;
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
} RawMsg;
typedef char
    FileShareParentArea_mNodes_must_be_0xc0[(int)&((FileShareParentArea *)0)->mNodes == 0xc0 ? 1
                                                                                             : -1];
typedef char FileShareChildArea_mRecvWork_must_be_0xc70
    [(int)&((FileShareChildArea *)0)->mRecvWork == 0xc70 ? 1 : -1];
typedef struct {
    char pad[0xb50];
    u32 field_b50;
    u32 pad_b54;
    int field_b58; /* WFS_GetCurrentBitmap */
} UnkStruct_020a3fc0;
struct EmptyInit {
    EmptyInit() {}
};

extern void MB_End(void);
extern "C" {

extern u32 MB_GetSegmentLength(FSFile *file);
}

extern int MB_Init(void *work, const MBUserInfo *user, int ggid0, int ggid1, u16 tgid, int dma);
extern "C" {

extern int MB_ReadSegment(FSFile *file, void *buf, u32 len);
extern int MB_RegisterFile(MBGameRegistry *game_reg, const void *buf);
}

extern int MB_SetParentCommParam(int a, int b, int c);
extern int MB_StartParent(int param0);
extern "C" {

extern void OS_GetMacAddress(void *p);
extern void OS_GetOwnerInfo(void *dst);
}

extern int OS_GetTickLo(void);
extern "C" {

extern void WBT_MpChildRecvHook(int data, int length);
extern void WBT_MpParentRecvHook(int data, int length, int aid);
}

extern void func_0201a4d0(void);
extern "C" {

extern void func_0203b9bc(TPData *out);
extern WMErrCode func_0206259c(WMCallbackFunc callback, u32 param);
extern void (*data_0209a03c)(void *);
}

extern char data_02082158[];
extern u8 data_02099e18;
extern u8 data_02099e1c;
extern int data_02099e20;
extern void *data_02099e24;
extern char data_02099e6c[];
/* "rom" */
extern u8 data_020a0ef0;
extern u8 data_020a0ef4;
extern u8 data_020a0ef8;
extern u8 data_020a0efc;
extern u8 data_020a0f00;
extern u8 data_020a0f04;
extern u8 data_020a0f08;
extern unsigned char data_020a0f0c;
extern u8 data_020a0f10;
extern unsigned short data_020a0f14;
extern s16 data_020a0f18;
extern u16 data_020a0f1c;
extern u16 data_020a0f20;
/* 0x020a0f20 - counter */
extern unsigned short data_020a0f24;
extern unsigned short data_020a0f28;
extern unsigned short data_020a0f2c;
extern u16 data_020a0f30;
extern unsigned short data_020a0f34;
extern int data_020a0f38;
extern void *data_020a0f3c;
extern s32 data_020a0f40;

/* The WMParentParam func_0203fbc4 hands to WM_SetParentParameter. */
extern WMParentParam *data_020a0f44;

extern void *data_020a0f48;
extern void *data_020a0f4c;
extern s32 data_020a0f50;
extern WMBssDesc *data_020a0f54;
extern unsigned int data_020a0f58;
extern int data_020a0f5c;
extern void *data_020a0f60;
extern unsigned int data_020a0f64;
extern void *data_020a0f68;
extern s32 data_020a0f6c;
extern int data_020a0f70;
extern WMDataSharingInfo *data_020a0f74;
extern s32 data_020a0f78;
extern void *data_020a0f7c;
extern WMDataSet *data_020a0f80;
extern int data_020a0f84;
extern int data_020a0f88;
extern int data_020a0f8c;
extern int data_020a0f94;
extern int data_020a0f98;
extern u8 data_020a0f9d[4];
extern u16 data_020a0fa0[][3];
extern u8 data_020a0fb8[];
extern u16 data_020a0fbe[4][3];
extern u16 data_020a0fd0[];
extern MBGameRegistry data_020a0fec;

/* data_020a1020 is the packet this console sends each frame, packed from
   the local record and unpacked the same way into data_020a1154 below: +0x0
   flags, +0x2 frame, +0x6 keys, +0x8/+0x9/+0xa touch x/y/touch (one byte
   each), +0xb inputYaw, +0xd data_020a0f08, +0xe data_020a0f00, +0xf the
   0x11 bytes at the record's 0x12. While data_020a0ef8 counts down it
   carries instead a chunk index at +0x2 and one of the 0x16-byte chunks at
   data_020a10a4 at +0x3. */
extern u8 data_020a1020[0x20];

/* The WMParentParam template func_0203db64 keeps. */
extern WMParentParam data_020a1064;

/* A player's 0x16-byte profile as the link exchanges it (func_0203ea5c sends
   them a chunk at a time): the owner's favourite colour and nickname.
   data_020a10fc and data_020a10a4 are each a table of four (0x58 bytes,
   cleared by func_0203db64); func_0203d8a0 hands out player i's from
   data_020a10fc in link mode 3 and from data_020a10a4 otherwise. Off a
   download-play child func_0203db64 fills in entry 0 of both from the owner
   settings. Entries 1..3 of
   data_020a10fc are the download-play children's, which func_0203e20c
   fills in as they connect. */
extern MBUserInfo data_020a10a4[4];
extern MBUserInfo data_020a10fc[4];
extern char data_020a11e4[];

/* A worker thread; its 0x400-byte stack is data_020a11e4, just below it
 * (WFSi_CreateTaskThread creates it). */
extern OSThread data_020a15e4;

extern WMStatus data_020a1680;
extern char data_020a2400[];
extern char data_020a2404[];
extern u8 data_020a2408[];
extern u8 data_020a2409[];
extern UnkStruct_020a3fc0 data_020a3fc0;

int Wireless_Reset(void);
int func_0203d81c(void);
void func_0203d854(void);
int func_0203d8fc(void);
void func_0203d918(void);
void func_0203df40(void);
void func_0203e0ac(void);
void func_0203e20c(void);
void func_0203e978(ChannelMeasureReply *reply);
void func_0203e9a8(u16 aid, u32 status, void *arg);
void func_0203ea5c(void);
void *func_0203f604(void *arg, unsigned int size, void *ptr);
void func_0203f644(int state);
void func_0203f650(int x);
void func_0203f6a4(int v);
void func_0203f6b4(void);
void func_0203f6e8(short v);
void func_0203f714(WMMeasureChannelCallback *s);
int func_0203f818(u16 channel);
void func_0203f8a4(void);
void func_0203f920(void *arg);
int func_0203fa50(void);
int func_0203faa8(void);
int func_0203fb5c(void);
int func_0203fb98(void);
int func_0203fbc4(void);
int func_0203fc38(void);
int func_0203fc74(void);
int func_0203fcb0(void);
int func_0203fcec(void);
int func_0203fd28(void);
int func_0203fd64(void);
void func_0203fdac(void *arg);
void func_0203fec4(void *arg);
void func_02040014(void *arg);
void func_020401e8(void *arg);
void func_020402a0(void *arg);
void func_02040384(void *arg);
void func_02040388(void *arg);
void func_02040408(void *arg);
void func_02040504(void *arg);
void func_020405b4(void *arg);
void func_02040634(void *arg);
void func_02040638(void);
void *func_0204068c(unsigned short aid);
int func_020406b4(unsigned char *sendData, unsigned short *out);
int func_02040704(void);
int func_02040714(void);
void func_02040724(void);
void func_02040790(void);
void func_02040820(void);
void func_020408b0(unsigned short arg);
void WFS_EnableSync(u32 val);
int WFS_GetCurrentBitmap(void);
int WFS_GetStatus(void);
void WFS_End(void);
void WFS_Start(void);
void WFSi_OnChildSystemCallback(void *arg);
void WFSi_ReallocBitmap(char *thiz, int n);
void WFSi_OnParentSystemCallback(void *arg);
void func_0204175c(void);
void WFSi_PortCallback(void *arg);
void WFSi_OnSetMPDataDone(void *arg);
void WFSi_SetMPData(void);
void WFSi_ReadRequest(FSFile *file);
void WFSi_FromBusyToFree(FileShareParentArea *area, FileShareNode *node);
void WFSi_FromAliveToBusy(FileShareParentArea *area, FileShareNode *node);
void WFSi_FromBusyToAlive(FileShareParentArea *area, FileShareNode *node);
FileShareNode *WFSi_FromFreeToBusy(FileShareParentArea *area);
void WFSi_MoveList(FileShareNode **from, FileShareNode **to, FileShareNode *node);
FileShareNode *WFSi_FindAliveForID(FileShareParentArea *area, int blockId);
FileShareNode *WFSi_FindBusy(FileShareParentArea *area, int offset, int size);
FileShareNode *WFSi_FindAlive(FileShareParentArea *area, int offset, int size);
void WFSi_SendOpenAck(char *r7, FileShareNode *node, int);
void WFSi_SendAck(void);
void WFSi_OnSendMessageDone(void *arg);
void WFSi_EndTaskThread(void);
void WFSi_CreateTaskThread(void);
void WFSi_TaskThread(void);
void WFSi_NotifyBusy(void);
int WFSi_ReplaceRomArchive(int *r6);
void WFSi_LoadTables(void);
int WFSi_RomArchiveProc(FSFile *self, int sel);
int WFSi_WriteRomCallback(FSArchive *p_arc, const void *src, u32 pos, u32 size);
int WFSi_ReadRomCallback(FSArchive *p_arc, void *dst, u32 pos, u32 size);
int func_0204271c(unsigned short *o);
int func_02042748(unsigned short *a, unsigned short *b);
void func_02042778(void);
u64 func_02042784(void);
u64 func_020427c4(void);
void func_020427f8(void);

void func_02040c34(int isParent, int port, void (*stateCallback)(int),
                   void *(*allocFunc)(void *arg, unsigned int size, void *ptr), void *allocArg);

/* func_020427f8 @ 0x20427f8 (arm9) -- tail-call veneer to OS_InitTick (0x2059788).
 * ldr ip, [pc]; bx ip; .word 0x2059788
 */

void func_020427f8(void) {
    OS_InitTick();
}

/* The OS tick count in milliseconds: ticks * 64 / (HW_SYSTEM_CLOCK / 1000).
 * A tick is 64 system-clock cycles (OSi_TICK_TIMERCONTROL runs timer 0 at
 * prescaler 64). pret's pokediamond spells the same expression in
 * Timer3CountToMilliSeconds. ShowCrashScreen hands the result to nds_printt
 * with the time template "99h59m59s999".
 * The quotient goes through _ll_udiv. */

u64 func_020427c4(void) {
    return OS_GetTick() * 64ULL / (HW_SYSTEM_CLOCK / 1000);
}

/* The OS tick count in microseconds: ticks * 64 * 1000 / (HW_SYSTEM_CLOCK /
 * 1000). A tick is 64 system-clock cycles (OSi_TICK_TIMERCONTROL runs timer
 * 0 at prescaler 64), and HW_SYSTEM_CLOCK is the SDK's 33514000.
 * The quotient goes through _ll_udiv. */

u64 func_02042784(void) {
    return OS_GetTick() * 64000ULL / (HW_SYSTEM_CLOCK / 1000);
}

/* func_02042778 @ 0x2042778 (arm9) -- tail-call veneer to func_0201a4d0 (0x201a4d0).
 * ldr ip, [pc]; bx ip; .word 0x201a4d0
 */

void func_02042778(void) {
    func_0201a4d0();
}

/* 1 if two 6-byte MAC addresses (three halfwords each) are equal, else 0.
   The parent's WM_StartParent callback (func_02040014) admits a child
   with it, and func_0203ea5c matches addresses with it. */
int func_02042748(unsigned short *a, unsigned short *b) {
    int i;
    for (i = 0; i < 3; i++) {
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}

/* 1 if a 6-byte MAC address (three halfwords) is all zero, else 0.
   func_0203ea5c skips the empty entries of its address table with it. */
int func_0204271c(unsigned short *o) {
    int i;
    for (i = 0; i < 3; i++) {
        if (o[i] != 0)
            return 0;
    }
    return 1;
}

/* The read callback WFSi_ReplaceRomArchive gives the child's "rom" archive, a
   NitroSDK FS_ARCHIVE_READ_FUNC: the archive's tables are in memory, so a
   read copies size bytes from address pos to dst and succeeds (0). */

int WFSi_ReadRomCallback(FSArchive *p_arc, void *dst, u32 pos, u32 size) {
    MI_CpuCopy8((const void *)pos, dst, size);
    return 0;
}

/* The write callback WFSi_ReplaceRomArchive gives the child's "rom" archive, a
   NitroSDK FS_ARCHIVE_WRITE_FUNC: 1 (FS_RESULT_FAILURE) for every write. */

int WFSi_WriteRomCallback(FSArchive *p_arc, const void *src, u32 pos, u32 size) {
    return 1;
}

/* The child's archive proc. WFSi_ReplaceRomArchive installs it for every command with
 * FS_SetArchiveProc, and it returns NitroSDK FSResult codes.
 * A read (FS_COMMAND_READFILE, 0) fails with 5 (ERROR) until the share is
 * ready (mState 2), and succeeds at once (0) when empty. Otherwise
 * WFSi_ReadRequest asks the parent for the data and the read goes asynchronous
 * (6, PROC_ASYNC). A write (1) is unsupported (4), and any other command
 * falls through to the default handling (8, PROC_UNKNOWN). */
int WFSi_RomArchiveProc(FSFile *self, int sel) {
    switch (sel) {
    case 0:
        if (data_020a1fc0.mState != 2)
            return 5;
        if (self->arg.readfile.len == 0)
            return 0;
        WFSi_ReadRequest(self);
        return 6;
    case 1:
        return 4;
    default:
        return 8;
    }
}

/* Allocate one block for the card's FAT, FNT and both overlay tables (each
 * as its size word followed by the data rounded up to 0x20 bytes, after one
 * leading word) and store it as mTable / mTableSize. The parent fills it from
 * the ROM archive: it reads each region, then sets the leading word to the
 * larger of the block size and the largest FAT file. The child only
 * allocates. Its caller func_02040c34 then registers the parent's block as
 * region 0x20000, or on the child passes mTableSize to WFSi_ReallocBitmap to grow
 * the receive work. */
void WFSi_LoadTables(void) {
    int sizes[4];
    const CARDRomRegion *ptrs[4];
    FSFile file;
    u32 sl;
    void *fp;
    int i;

    ptrs[0] = CARD_GetRomRegionFAT();
    ptrs[1] = CARD_GetRomRegionFNT();
    ptrs[2] = CARD_GetRomRegionOVT(MI_PROCESSOR_ARM9);
    ptrs[3] = CARD_GetRomRegionOVT(MI_PROCESSOR_ARM7);

    /* The leading word, then each region's size word and rounded data. */
    sl = 0;
    sl += sizeof(u32);
    i = 0;
    do {
        sizes[i] = (int)((ptrs[i]->length + 0x1f) & ~0x1f);
        sl += sizes[i] + 4;
        i++;
    } while (i < 4);

    fp = FileShareAlloc(sl);

    if (data_020a1fc0.mIsParent != 0) {
        FSArchive *sb;
        char *buf;
        int roundedSize;
        int j;

        sb = FS_FindArchive(data_02099e6c, 3);
        buf = (char *)fp + 4;
        FS_InitFile(&file);
        j = 0;

        do {
            const CARDRomRegion *p = ptrs[j];
            int origSize = (int)p->length;
            int data0;
            int end;

            roundedSize = sizes[j];
            *(int *)buf = origSize;
            buf += 4;
            data0 = (int)p->offset;
            end = roundedSize + data0;
            FS_OpenFileDirect(&file, sb, data0, end, -1);
            FS_ReadFile(&file, buf, roundedSize);
            FS_CloseFile(&file);
            buf += roundedSize;
            j++;
        } while (j < 4);

        {
            Entry *entries = (Entry *)((char *)fp + 8);
            int count = (int)(ptrs[0]->length >> 3);
            u32 max = sl;
            int k;
            k = 0;
            if (count > 0) {
                do {
                    u32 diff = (u32)entries[k].end - (u32)entries[k].start;
                    if (max < diff)
                        max = diff;
                    k++;
                } while (k < count);
            }
            *(int *)fp = max;
        }

        DC_FlushRange(fp, sl);
        DC_WaitWriteBufferEmpty();
    }

    data_020a1fc0.mTable = fp;
    data_020a1fc0.mTableSize = sl;
}

/* Child: mount the file-share table block (WFSi_LoadTables's layout: a
 * leading word, then the FAT, FNT, ARM9 and ARM7 overlay tables, each as its
 * size word and data rounded up to 0x20) as the "rom" archive.
 * The calls have the NitroSDK FS shapes:
 * - FS_FindArchive finds the archive by name.
 * - FS_UnloadArchive unloads it.
 * - FS_SetArchiveProc installs WFSi_RomArchiveProc as its proc for every command.
 * - FS_LoadArchive loads it from the FAT and FNT here, at base 0.
 *   WFSi_ReadRomCallback copies from memory for reads, and WFSi_WriteRomCallback fails
 *   writes.
 * - FS_AttachOverlayTable attaches the two overlay tables.
 * Returns the block's leading word. */
int WFSi_ReplaceRomArchive(int *r6) {
    int r5;
    FSArchive *h;
    int r3;
    struct Pair arr[4];

    h = FS_FindArchive(data_02099e6c, 3);
    FS_UnloadArchive(h);
    FS_SetArchiveProc(h, &WFSi_RomArchiveProc, -1);
    r5 = *r6;
    r6++;
    for (r3 = 0; r3 < 4; r3++) {
        arr[r3].f0 = r6 + 1;
        arr[r3].f4 = *r6;
        r6 = (int *)((char *)arr[r3].f0 + ((arr[r3].f4 + 0x1f) & ~0x1f));
    }
    FS_LoadArchive(h, 0, (u32)arr[0].f0, arr[0].f4, (u32)arr[1].f0, arr[1].f4,
                   &WFSi_ReadRomCallback, &WFSi_WriteRomCallback);
    FS_AttachOverlayTable(MI_PROCESSOR_ARM9, arr[2].f0, arr[2].f4);
    FS_AttachOverlayTable(MI_PROCESSOR_ARM7, arr[3].f0, arr[3].f4);
    return r5;
}

void WFSi_NotifyBusy(void) {
    OS_WakeupThreadDirect(&data_020a15e4);
}

/* The parent's loader thread: while the share is up (mState), take each node
 * off the busy list; a loading one (state 1) has its region's file opened
 * from the ROM archive (FS_OpenFileDirect over
 * [mFileOffset, mFileOffset + mFileSize), by mFileId), its first three
 * 0x400-byte pages read into the cache, and is handed to WFSi_SendOpenAck; a
 * releasing one has its file closed and goes back to the free list
 * (WFSi_FromBusyToFree). The arc local is the archive FS_FindArchive finds (by name,
 * 3 characters). */
void WFSi_TaskThread(void) {
    int i;
    FileShareNode *node;
    FSArchive *arc = FS_FindArchive(data_02099e6c, 3);
    int b = 2;
    int c = 3;
    int d = 1;
    FileShareWork *work = &data_020a1fc0;
    char *base = data_020a2400;

    while (1) {
        OSIntrMode irq = OS_DisableInterrupts();
        FileShareParentArea *area = (FileShareParentArea *)base;
        node = area->mBusyList;
        while (node == 0) {
            if (work->mState == 0) {
                OS_RestoreInterrupts(irq);
                return;
            }
            OS_SleepThread(0);
            node = area->mBusyList;
        }
        OS_RestoreInterrupts(irq);

        if (node->mState == 1) {
            FS_OpenFileDirect(&node->mFile, arc, node->mFileOffset,
                              node->mFileOffset + node->mFileSize, node->mFileId);
            i = 0;
            {
                int val = 0;
                char *ptr = (char *)node->mPages;
                for (; i < 3; i++) {
                    node->mPageOffset[i] = val;
                    FS_ReadFile(&node->mFile, ptr, 0x400);
                    val += 0x400;
                    ptr += 0x400;
                }
            }
            node->mPrevSeqNo = 0;
            node->mLastRefillPage = b;
            node->mLoadingPage = c;
            irq = OS_DisableInterrupts();
            WFSi_SendOpenAck(base, node, d);
            OS_RestoreInterrupts(irq);
        } else {
            FS_CloseFile(&node->mFile);
            irq = OS_DisableInterrupts();
            WFSi_FromBusyToFree(area, node);
            OS_RestoreInterrupts(irq);
        }
    }
}

/* Start the file-share parent's loader thread (WFSi_TaskThread) in its block
   at data_020a11e4: the 0x400-byte stack, then the OSThread (also the
   symbol data_020a15e4, which WFSi_NotifyBusy wakes). OS_CreateThread takes
   (thread, entry, arg, stack top, stack size, priority 15 here), and
   OS_WakeupThreadDirect then wakes the thread. Both this and WFSi_EndTaskThread
   reach the block through a pointer cast; declaring data_020a11e4 itself a
   LoaderThreadBlock changes the code of both. */
void WFSi_CreateTaskThread(void) {
    LoaderThreadBlock *blk = (LoaderThreadBlock *)data_020a11e4;
    blk->mFlag = 0;
    OS_CreateThread(&blk->thread, (void (*)(void *))WFSi_TaskThread, blk,
                    blk->stack + sizeof(blk->stack), sizeof(blk->stack), 0xf);
    OS_WakeupThreadDirect(&blk->thread);
}

/* Stop the file-share loader thread (see WFSi_CreateTaskThread): until the thread is
 * terminated (state 2), wake it so that it sees mState cleared and returns,
 * then join it (OS_JoinThread sleeps on its join queue). WFS_End calls
 * this at shutdown. */
void WFSi_EndTaskThread(void) {
    LoaderThreadBlock *blk = (LoaderThreadBlock *)data_020a11e4;
    if (OS_IsThreadTerminated(&blk->thread))
        return;
    do {
        OS_WakeupThreadDirect(&blk->thread);
        OS_JoinThread(&blk->thread);
    } while (!OS_IsThreadTerminated(&blk->thread));
}

/* The parent's reply-send callback (WBT_PutUserData's; the WBTCommand it
 * is called with goes unread): once the file-sharing work is up (mState) and
 * running as the parent, clear mReplySending, which WFSi_SendAck set after
 * its send, and let it send the next reply. */
void WFSi_OnSendMessageDone(void *arg) {
    FileShareWork *g = &data_020a1fc0;
    if (g->mState == 0)
        return;
    if (g->mIsParent == 0)
        return;
    {
        FileShareParentArea *s = (FileShareParentArea *)g->mArea;
        s->mReplySending = 0;
        WFSi_SendAck();
    }
}

void WFSi_SendAck(void) {
    FileShareParentArea *area = (FileShareParentArea *)FILE_SHARE_WORK->mArea;
    FileShareMsg resizeMsg;
    FileShareMsg replyMsg;

    if (data_020a1fc0.mIsParent == 0) {
        return;
    }
    if (area->mReplySending != 0) {
        return;
    }

    area->mReplyPending &= area->mConnectedBitmap;
    area->mReplyGroup &= area->mConnectedBitmap;
    area->mOpenChildren &= area->mConnectedBitmap;
    area->mResizeChildren &= area->mConnectedBitmap;

    if (area->mResizePending != 0 && area->mBusyCount == 0) {
        int newSize = area->mNewPacketSize;
        int childSize = data_020a1fc0.mChildPacketSize;
        int children;

        area->mResizePending = 0;
        data_020a1fc0.mParentPacketSize = newSize;
        WBT_SetPacketSize(newSize, childSize);

        children = area->mResizeChildren;
        if (children == 0) {
            return;
        }
        resizeMsg.mKind = 1;
        resizeMsg.mFlag = 0;
        resizeMsg.mFileId = 0;
        resizeMsg.mPacketSizeHigh = (u8)(data_020a1fc0.mParentPacketSize >> 8);
        resizeMsg.mPacketSizeLow = (u8)data_020a1fc0.mParentPacketSize;
        WBT_PutUserData(children, &resizeMsg, WBT_SIZE_USER_DATA, WFSi_OnSendMessageDone);
        area->mReplySending = 1;
        area->mResizeChildren = 0;
    } else {
        int mask;
        FileShareMsg *first;
        int grouped;
        int i;
        int group;
        int bit;
        FileShareMsg *msg;
        u8 kind;
        int packetSize;
        int fileId;

        mask = area->mReplyPending;
        if (mask == 0) {
            return;
        }
        group = area->mReplyGroup;
        first = 0;
        grouped = (group != 0 && group == (group & mask));
        if (grouped) {
            mask = group;
        } else {
            mask &= ~group;
        }

        msg = (FileShareMsg *)area->mReplies;
        i = 0;
        for (;;) {
            bit = 1 << i;
            if (bit > mask) {
                break;
            }
            if (bit & mask) {
                if (first == 0) {
                    first = msg;
                } else if (first->mKind != msg->mKind || first->mFileId != msg->mFileId) {
                    mask &= ~bit;
                }
            }
            msg++;
            i++;
        }

        if (grouped && mask != group) {
            area->mReplyGroup = 0;
        }
        if (first == 0) {
            return;
        }

        switch (first->mKind) {
        case 0:
            first->mKind = 1;
            break;
        case 2:
            first->mKind = 3;
            area->mOpenChildren &= ~mask;
            break;
        }

        kind = first->mKind;
        fileId = first->mFileId;
        packetSize = data_020a1fc0.mParentPacketSize;
        replyMsg.mKind = kind;
        replyMsg.mFlag = 1;
        replyMsg.mFileId = fileId;
        replyMsg.mPacketSizeHigh = (u8)(packetSize >> 8);
        replyMsg.mPacketSizeLow = (u8)packetSize;
        WBT_PutUserData(mask, &replyMsg, WBT_SIZE_USER_DATA, WFSi_OnSendMessageDone);
        area->mReplySending = 1;
        area->mReplyPending &= ~mask;
    }
}

/* A node has finished loading (WFSi_TaskThread). The children that asked for it
   while it was loading become owed a reply. If no child holds it yet, it is
   registered with WBT_RegisterBlock (with the first page as data when the region
   fits in 0x400 bytes) and moved to the ready list by WFSi_FromBusyToAlive. Each
   waiting child then takes a reference and gets the node's file id in its
   reply slot. Only while mState is set. The third word is
   unused: WFSi_TaskThread passes 1 (reloaded from its frame into r2). */
void WFSi_SendOpenAck(char *r7, FileShareNode *node, int) {
    OSIntrMode saved = OS_DisableInterrupts();
    int mask;

    if (data_020a1fc0.mState != 0) {
        int bit;
        int *q = &((FileShareParentArea *)r7)->mReplyPending;
        mask = node->mWaitingChildren;
        node->mWaitingChildren = 0;
        *q |= mask;
        if (node->mRefCount <= 0) {
            unsigned int diff = FS_GetLength(&node->mFile);
            int fileId = node->mFileId;
            WBT_RegisterBlock(&node->mRegion, fileId, data_02082158,
                              (diff <= 0x400) ? node->mPages[0] : 0, diff, 0);
            WFSi_FromBusyToAlive((FileShareParentArea *)r7, node);
        }
        for (bit = 0;; bit++) {
            if ((1 << bit) > mask) {
                break;
            }
            if ((1 << bit) & mask) {
                node->mRefCount++;
                ((FileShareMsg *)r7)->mFileId = node->mFileId;
            }
            r7 += 0xc;
        }
    }
    OS_RestoreInterrupts(saved);
}

/* Find the ready node whose file covers [offset, offset + size); 0 if none.
   WFSi_OnParentSystemCallback serves an open request from it. */
FileShareNode *WFSi_FindAlive(FileShareParentArea *area, int offset, int size) {
    FileShareNode *node = area->mReadyList;
    if (node == 0)
        return node;
    do {
        if (node->mState == 2 && offset == node->mFile.prop.file.top &&
            size == FS_GetLength(&node->mFile))
            return node;
        node = node->mNext;
    } while (node != 0);
    return node;
}

/* The parent's node for a child's request (offset, length): a loading node
 * already holding that extent, or a fresh one off the free list
 * (WFSi_FromFreeToBusy) with the extent stored and the loader thread woken
 * (WFSi_NotifyBusy). */
FileShareNode *WFSi_FindBusy(FileShareParentArea *area, int offset, int size) {
    FileShareNode *node;
    node = area->mBusyList;
    while (node != 0) {
        if (node->mState == 1 && node->mFileOffset == offset && node->mFileSize == size) {
            break;
        }
        node = node->mNext;
    }
    if (node == 0) {
        node = WFSi_FromFreeToBusy(area);
        node->mRefCount = 0;
        node->mFileOffset = offset;
        node->mFileSize = size;
        WFSi_NotifyBusy();
    }
    return node;
}

/* The parent's ready-list search: the first ready node (mState 2) whose
 * registered block is blockId (the WBT block id WBT_RegisterBlock stored in
 * mRegion), else 0. */
FileShareNode *WFSi_FindAliveForID(FileShareParentArea *area, int blockId) {
    FileShareNode *node = area->mReadyList;
    if (node == 0)
        return node;
    do {
        if (node->mState == 2 && node->mRegion.data_info.id == blockId)
            return node;
        node = node->mNext;
    } while (node != 0);
    return node;
}

/* Moves node off the list headed by *from onto the front of the list headed
 * by *to, with interrupts off. The parent's three node lists (free, ready,
 * busy) change only through here: WFSi_FromFreeToBusy, WFSi_FromBusyToAlive,
 * WFSi_FromAliveToBusy and WFSi_FromBusyToFree are its four moves.
 *
 * The search never advances: the loop either unlinks the node from behind
 * the head or breaks. A node third in its list is left where it is, and
 * nothing is moved. That is the ROM's code (0x02041bf4..0x02041c14), not a
 * decompilation slip; with a pool of three nodes, a list can be three long. */
void WFSi_MoveList(FileShareNode **from, FileShareNode **to, FileShareNode *node) {
    OSIntrMode saved = OS_DisableInterrupts();
    FileShareNode *cur = *from;
    if (cur == node) {
        *from = node->mNext;
        node->mNext = *to;
        *to = node;
    } else {
        while (cur != 0) {
            if (cur->mNext != node)
                break;
            cur->mNext = node->mNext;
            node->mNext = *to;
            *to = node;
            break;
        }
    }
    OS_RestoreInterrupts(saved);
}

/* free -> busy: takes the free list's head to be loaded (state 1) and
 * counts it in mBusyCount. */
FileShareNode *WFSi_FromFreeToBusy(FileShareParentArea *area) {
    FileShareNode *node;
    node = area->mFreeList;
    WFSi_MoveList(&area->mFreeList, &area->mBusyList, node);
    node->mState = 1;
    area->mBusyCount += 1;
    return node;
}

/* busy -> ready: the loader has read the region's first pages and
 * WFSi_SendOpenAck has registered it (state 2). */
void WFSi_FromBusyToAlive(FileShareParentArea *area, FileShareNode *node) {
    WFSi_MoveList(&area->mBusyList, &area->mReadyList, node);
    node->mState = 2;
}

/* ready -> busy: no child reads the node's region any more, so the loader
 * closes its file (state 3, releasing). */
void WFSi_FromAliveToBusy(FileShareParentArea *area, FileShareNode *node) {
    WFSi_MoveList(&area->mReadyList, &area->mBusyList, node);
    node->mState = 3;
}

/* busy -> free: the loader has closed the node's file (state 0), and the
 * node is no longer counted in mBusyCount. */
void WFSi_FromBusyToFree(FileShareParentArea *area, FileShareNode *node) {
    WFSi_MoveList(&area->mBusyList, &area->mFreeList, node);
    node->mState = 0;
    area->mBusyCount -= 1;
}

/* The open request (kind 0) a child sends the parent: FileShareMsg's layout
 * (arm9_a1fc0_file_share.h), with the region's length and offset where the
 * replies carry mFlag and mFileId. WFSi_OnParentSystemCallback reads it back: it compares the two packet-size bytes with its own mParentPacketSize,
 * and looks the region up by (offset, length) with WFSi_FindAlive and
 * WFSi_FindBusy. */

/* Child: ask the parent to open the region the FSFile read covers. The
 * receive work is grown to the read's length first, and the file is kept in
 * mReadRequest until WFSi_OnChildSystemCallback completes it. */
void WFSi_ReadRequest(FSFile *file) {
    OpenRequest m;
    int packetSize;
    int len;
    int pos;
    WFSi_ReallocBitmap(data_020a2400, file->arg.readfile.len);
    data_020a1fc0.mReadRequest = file;
    len = file->arg.readfile.len;
    pos = file->prop.file.pos;
    m.kind = 0;
    m.length = len;
    packetSize = data_020a1fc0.mParentPacketSize;
    m.offset = pos;
    m.packetSizeHigh = (packetSize >> 8) & 0xff;
    m.packetSizeLow = (u8)packetSize;
    WBT_PutUserData(1, &m, WBT_SIZE_USER_DATA, WFSi_OnSendMessageDone);
}

/* Send the pending file-sharing packet on mPort: WBT_MpParentSendHook (parent) or
 * WBT_MpChildSendHook (child), by mIsParent, builds it in the buffer at +0x40 with
 * that side's packet size as the limit, then WM_SetMPDataToPort sends it to AIDs
 * 0xffff with WFSi_OnSetMPDataDone as the callback. */
void WFSi_SetMPData(void) {
    FileShareWork *work = &data_020a1fc0;
    int f0 = work->mIsParent;
    u16 a;
    u16 r;
    if (f0 != 0)
        a = (u16)work->mParentPacketSize;
    else
        a = (u16)work->mChildPacketSize;
    if (work->mSendRequest == 0)
        return;
    {
        unsigned char *sub = work->mSendBuf;
        if (f0 != 0)
            r = WBT_MpParentSendHook(sub, a);
        else
            r = WBT_MpChildSendHook(sub, a);
        work->mSendBusy = (WM_SetMPDataToPort(WFSi_OnSetMPDataDone, (u16 *)sub, r, 0xffff,
                                              work->mPort, WM_PRIORITY_LOW) == WM_ERRCODE_OPERATING)
                              ? 1
                              : 0;
    }
    /* re-read through the symbol: `work->mSendBusy` changes the .text bytes */
    work->mSendRequest = (data_020a1fc0.mSendBusy == 0) ? 1 : 0;
}

/* The send callback WFSi_SetMPData hands the port send: unless the file
   share is shut down (mState 0), raise mSendRequest, run WFSi_SendAck
   and send the next packet (WFSi_SetMPData). */
void WFSi_OnSetMPDataDone(void *arg) {
    FileShareWork *g = &data_020a1fc0;
    if (g->mState == 0)
        return;
    g->mSendRequest = 1;
    WFSi_SendAck();
    WFSi_SetMPData();
}

/* The file-sharing port receive callback. Once the work block is up
 * (mState): CONNECTED lets the parent resend (WFSi_SetMPData) unless a
 * send is in flight; DISCONNECTED drops the child from the parent's
 * mConnectedBitmap; PORT_RECV marks the sender connected and hands
 * its packet to the parent's handler (WBT_MpParentRecvHook), or on a child to
 * WBT_MpChildRecvHook. The argument is a NitroSDK WMPortRecvCallback
 * (private/wm_views.h). */
void WFSi_PortCallback(void *arg) {
    WMPortRecvCallback *cb = (WMPortRecvCallback *)arg;
    FileShareWork *g = FILE_SHARE_WORK;
    FileShareParentArea *base = (FileShareParentArea *)g->mArea;

    if (g->mState == 0)
        return;

    switch (cb->state) {
    case WM_STATECODE_CONNECTED:
        if (g->mIsParent == 0)
            return;
        if (g->mSendBusy != 0)
            return;
        WFSi_SetMPData();
        return;
    case WM_STATECODE_DISCONNECTED:
        if (g->mIsParent == 0)
            return;
        base->mConnectedBitmap &= ~(1 << cb->aid);
        return;
    case WM_STATECODE_PORT_RECV: {
        s32 data = (s32)cb->data;
        u32 length = cb->length;
        if (g->mIsParent != 0) {
            u32 aid = cb->aid;
            base->mConnectedBitmap |= (1 << aid);
            WBT_MpParentRecvHook(data, length, aid);
        } else {
            WBT_MpChildRecvHook(data, length);
        }
        return;
    }
    }
}

void func_0204175c(void) {
    OSIntrMode state;
    u32 ie;
    u32 iflags;

    state = OS_DisableInterrupts();
    ie = REG_IE;
    iflags = REG_IF;

    if ((iflags & OS_IE_CARD_DATA) && !(ie & OS_IE_CARD_DATA)) {
        OS_EnableIrqMask(OS_IE_CARD_DATA);
        gGameCardHandlerPtr(&gGameCardHandlerArg);
        gGameCardSentinel = 0x80000000;
    }

    OS_RestoreInterrupts(state);
}

/* WFSi_OnParentSystemCallback @ 0x020412f0 (arm9, size 0x46c)   [mwccarm 2004/b56]
 * Lever (notes 6y.1 refinement): the byte-triple store into data_020a2409 needs the
 * pooled base in r3 and the derived pointer (base + r6) in r2; every plain spelling
 * inverts them and stalls at 5 divergences. Three things must hold at once: the base
 * is a NAMED local, d9 is derived from that local rather than from the array symbol
 * again, and the zero-instruction self-select booster sits on the BASE (the shorter-
 * lived web), after the byte temps and immediately before the first store.
 * Boosting the DERIVED pointer is inert at all placements. Here the booster raises a
 * web toward the HIGHER-numbered scratch register, opposite to the callee-saved cases
 * in 6y. Do not drop `b9b = b9b ? b9b : b9b;` or fold b9b back to the array symbol.
 * Also improves 1.2/base|sp2|sp2p3 from 12 divergences to 7, so it is a better source
 * shape rather than a b56 accident. Link-verified: VERIFIED, 0 diffs, 0 blind.
 * Re-measured without the booster (b56, 0x46c unless noted): with volatile b9/ba
 * reads, 5 words (r2/r3 swapped); with plain reads, 7; the byte temps read before
 * d9 is formed, 8; d9 declared first, d9 taken as &b9b[r6], or d9 from the array
 * symbol, 7; three b9b[r6 + k] stores, 0x470; a 12-byte reply struct indexed off
 * data_020a2400, 0x454. With the booster in place the volatile b9/ba reads and the
 * (Msg *)(int) casts on the word-0 and kind reads change nothing, so they are gone.
 */

void WFSi_OnParentSystemCallback(void *arg) {
    int sb;
    int type;
    u32 r6;
    FileShareNode *p;
    WBTPrepareSendDataCallback *n;
    WBTCommand *cmd = (WBTCommand *)arg;

    sb = WBT_AidbitmapToAid(cmd->peer_bmp);
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
        RawMsg *m = (RawMsg *)cmd->u.user_data.data;
        char *base = data_020a2400;
        FileShareParentArea *area = (FileShareParentArea *)base;
        int kind;

        *(int *)(base + r6) = *(int *)m;
        *(int *)(data_020a2404 + r6) = m->w1;
        data_020a2408[r6] = m->b8;
        {
            u8 *b9b = data_020a2409;
            u8 *d9 = b9b + r6;
            u8 b9v = m->b9;
            u8 bav = m->ba;
            b9b = b9b ? b9b : b9b;
            b9b[r6] = b9v;
            d9[1] = bav;
            d9[2] = m->bb;
        }

        kind = m->f0;

        switch (kind) {
        case 0: {
            u32 nib1 = m->f8;
            u32 unk18v = m->w1;
            int val = (m->f4 << 8) | m->b8;

            if (area->mResizePending != 0 || val != data_020a1fc0.mParentPacketSize) {
                area->mResizePending = 1;
                area->mResizeChildren |= (1 << sb);
                area->mOpenChildren |= (1 << sb);
            } else {
                FileShareNode *p = WFSi_FindAlive(area, unk18v, nib1);

                if (p != 0) {
                    area->mReplyPending |= (1 << sb);
                    *(int *)(base + r6 + 4) = p->mFileId;
                    p->mRefCount += 1;
                } else {
                    FileShareNode *q = WFSi_FindBusy(area, unk18v, nib1);
                    q->mWaitingChildren |= (1 << sb);
                }

                area->mOpenChildren |= (1 << sb);
            }
            break;
        }
        case 2: {
            OSIntrMode saved = OS_DisableInterrupts();
            FileShareNode *p;

            area->mReplyPending |= (1 << sb);
            p = WFSi_FindAliveForID((FileShareParentArea *)base, m->w1);

            if (p != 0) {
                p->mRefCount -= 1;
                if (p->mRefCount <= 0) {
                    p->mWaitingChildren = 0;
                    WBT_UnregisterBlock(m->w1);
                    WFSi_FromAliveToBusy((FileShareParentArea *)base, p);
                    WFSi_NotifyBusy();
                }
            }

            OS_RestoreInterrupts(saved);
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
        p = WFSi_FindAliveForID((FileShareParentArea *)base, key);
        if (p == 0)
            break;

        shortVal = n->own_packet_size;
        p94 = p->mPrevSeqNo;
        unk18v = n->block_seq_no;

        if (p->mLoadingPage != 3) {
            /* FS_IsBusy: the page read is still in flight */
            int t = (p->mFile.stat & FS_FILE_STATUS_BUSY) ? 1 : 0;
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
                FS_SeekFile(&p->mFile, masked, FS_SEEK_SET);
                FS_ReadFileAsync(&p->mFile, p->mPages[nxt], 0x400);
            }
        }

        break;
    }
    }
}

/* Grow the child's receive work (WBT's packet bitmap) to cover a read of n
 * bytes, or with n < 0 reallocate it at the current mMaxReadSize. */
void WFSi_ReallocBitmap(char *thiz, int n) {
    FileShareChildArea *child = (FileShareChildArea *)thiz;
    void *work;
    unsigned int size;
    if (n < 0) {
        n = child->mMaxReadSize;
        child->mMaxReadSize = 0;
    }
    if ((unsigned int)child->mMaxReadSize >= (unsigned int)n)
        return;
    child->mMaxReadSize = n;
    FileShareFree(child->mRecvWork);
    {
        u32 payload = data_020a1fc0.mParentPacketSize - 0xe;  /* bytes per packet */
        size = (child->mMaxReadSize + payload - 1) / payload; /* packets */
    }
    size = ((size + 0x1f) >> 2) << 4; /* packets, then the work's size in bytes */
    work = FileShareAlloc(size);
    child->mRecvWork = work;
    child->mPacketBitmapTable[0] = child->mRecvWork;
}

/* The FSFile a child read is for (FileShareWork.mReadRequest, set by
 * WFSi_ReadRequest). */

void WFSi_OnChildSystemCallback(void *arg) {
    WBTCommand *cmd = (WBTCommand *)arg;
    int aid = WBT_AidbitmapToAid(cmd->peer_bmp);
    FileShareChildArea *child = (FileShareChildArea *)data_020a2400;
    switch (cmd->command) {
    case 2:
        if (cmd->target_bmp != 0) {
            return;
        }
        data_020a1fc0.mParentPacketSize = cmd->u.sync.peer_packet_size + 0xe;
        data_020a1fc0.mChildPacketSize = cmd->u.sync.my_packet_size + 0xe;
        if (data_020a1fc0.mState != 2) {
            WBT_GetBlockInfo(1, 0, child->mBlockInfoTable, WFSi_OnChildSystemCallback);
            return;
        }
        if (data_020a1fc0.mReadRequest != 0) {
            WFSi_ReadRequest(data_020a1fc0.mReadRequest);
        }
        return;

    case 6:
        child->mRecvBufTable[aid] = data_020a1fc0.mTable;
        WBT_GetBlock(1, 0x20000, child->mRecvBufTable, data_020a1fc0.mTableSize,
                     child->mPacketBitmapTable, WFSi_OnChildSystemCallback);
        return;

    case 4:
        if (cmd->target_bmp != 0) {
            return;
        }
        if (data_020a1fc0.mState != 2) {
            DC_FlushRange(data_020a1fc0.mTable, data_020a1fc0.mTableSize);
            WFSi_ReplaceRomArchive((int *)data_020a1fc0.mTable);
            data_020a1fc0.mState = 2;
            if (data_020a1fc0.mStateCallback != 0) {
                data_020a1fc0.mStateCallback(0);
            }
            return;
        }
        {
            FileShareMsg msg;
            u32 region = child->mReadRegion;
            int packetSize;
            msg.mKind = 2;
            msg.mFlag = 1;
            packetSize = data_020a1fc0.mParentPacketSize;
            msg.mFileId = region;
            msg.mPacketSizeHigh = (u8)(packetSize >> 8);
            msg.mPacketSizeLow = (u8)packetSize;
            WBT_PutUserData(1, &msg, WBT_SIZE_USER_DATA, WFSi_OnSendMessageDone);
        }
        return;

    case 0xc: {
        FileShareMsg *m;
        if (cmd->event != 0xa) {
            return;
        }
        m = (FileShareMsg *)cmd->u.user_data.data;
        switch (m->mKind) {
        case 1:
            if (m->mFlag == 0) {
                WBT_RequestSync(1, WFSi_OnChildSystemCallback);
                return;
            }
            child->mReadRegion = m->mFileId;
            child->mRecvBufTable[aid] = data_020a1fc0.mReadRequest->arg.readfile.dst;
            WBT_GetBlock(1, child->mReadRegion, child->mRecvBufTable,
                         data_020a1fc0.mReadRequest->arg.readfile.len, child->mPacketBitmapTable,
                         WFSi_OnChildSystemCallback);
            return;

        case 3: {
            FSFile *p = data_020a1fc0.mReadRequest;
            FSArchive *arc = p->arc;
            data_020a1fc0.mReadRequest = 0;
            p->prop.file.pos += p->arg.readfile.len;
            FS_NotifyArchiveAsyncEnd(arc, FS_RESULT_SUCCESS);
            return;
        }
        }

        return;
    }
    }
}

static inline void FlushFileShareArea(char *base, unsigned int size) {
    DC_FlushRange(base, size);
}

/* The other callees are C. */
void func_02040c34(int isParent, int port, void (*stateCallback)(int),
                   void *(*allocFunc)(void *arg, unsigned int size, void *ptr), void *allocArg) {
    FileShareWork *G = &data_020a1fc0;
    OSIntrMode state = OS_DisableInterrupts();
    if (data_020a0f84 != 0) {
        OS_RestoreInterrupts(state);
    } else {
        data_020a0f84 = 1;
        OS_RestoreInterrupts(state);

        G->mIsParent = isParent;
        G->mPort = port;
        G->mSendRequest = 0;
        G->mSendBusy = 0;
        G->mState = 1;
        G->mStateCallback = stateCallback;
        G->mAllocFunc = allocFunc;
        G->mAllocArg = allocArg;
        G->mReadRequest = 0;
        G->mTableSize = 0;
        G->mTable = 0;
        G->mParentPacketSize = 0;
        G->mChildPacketSize = 0xe;
        data_020a0f88 = 0;
        if (WM_SetPortCallback((u16)port, WFSi_PortCallback, 0) != 0)
            OS_Terminate();
        FS_Init(FS_DMA_NOT_USE);

        if (G->mIsParent != 0) {
            {
                char *base = (char *)G->mArea;
                FileShareNode *node;
                FileShareNode *next;
                FileShareNode *saved;
                char *zero;
                FileShareNode *end;
                FlushFileShareArea(base, sizeof(FileShareParentArea));
                DC_WaitWriteBufferEmpty();
                FileShareParentArea *nodes = (FileShareParentArea *)base;
                saved = nodes->mNodes;
                zero = 0;
                node = saved;
                next = node + 1;
                end = ((int)next & 1) ? nodes->mNodes + 3 : nodes->mNodes + 3;
                nodes->mFreeList = saved;
                if (next < end) {
                    do {
                        node->mNext = node + 1;
                        FS_InitFile(&node->mFile);
                        node->mRefCount = (int)zero;
                        node->mState = (int)zero;
                        node->mFileId = (((int)node + 0xcc00cc0) - (int)saved) / 0xcc0;
                        node += 1;
                    } while (node + 1 < end);
                }
                node->mNext = 0;
                FileShareParentArea *area = (FileShareParentArea *)base;
                area->mReplyGroup = 0;
                area->mReplyPending = 0;
                area->mReplySending = 0;
                area->mReadyList = 0;
                area->mBusyList = 0;
                G->mParentPacketSize = 0x200;
                area->mConnectedBitmap = 1;
                area->mOpenChildren = 0;
                area->mResizePending = 0;
                area->mResizeChildren = 0;
                area->mBusyCount = 0;
                area->mNewPacketSize = G->mParentPacketSize;
                FlushFileShareArea(base, sizeof(FileShareParentArea));
                DC_WaitWriteBufferEmpty();
                WBT_InitParent(G->mParentPacketSize, G->mChildPacketSize,
                               WFSi_OnParentSystemCallback);
                WFSi_LoadTables();
            }
            {
                FileShareParentArea *area = (FileShareParentArea *)G->mArea;
                FileShareNode *nd;
                nd = area->mFreeList;
                area->mFreeList = nd->mNext;
                MI_CpuClear8(nd, FILE_SHARE_NODE_SIZE);
                /* mTableSize is read through data_020a1fc0, not G, here and in the
                   child branch: the G-> spelling comes out 8 bytes short */
                WBT_RegisterBlock(&nd->mRegion, 0x20000, data_02082158, G->mTable,
                                  data_020a1fc0.mTableSize, 0);
                nd->mNext = area->mReadyList;
                area->mReadyList = nd;
                nd->mRefCount = 1;
                WFSi_CreateTaskThread();
            }
        } else {
            char *base = (char *)G->mArea;
            FileShareChildArea *arr = (FileShareChildArea *)base;
            int i;
            char *p;
            FlushFileShareArea(base, sizeof(FileShareChildArea));
            DC_WaitWriteBufferEmpty();
            p = base + 0xc0;
            for (i = 0; i < 0x10; i++) {
                arr->mBlockInfoTable[i] = p;
                arr->mRecvBufTable[i] = 0;
                arr->mPacketBitmapTable[i] = 0;
                p += 0x28;
            }
            arr->mRecvWork = 0;
            arr->mReadRegion = 0;
            G->mParentPacketSize = 0x80;
            arr->mMaxReadSize = 0x100000;
            WFSi_ReallocBitmap(base, -1);
            G->mParentPacketSize = 0;
            WFSi_LoadTables();
            WFSi_ReallocBitmap(base, data_020a1fc0.mTableSize);
            FlushFileShareArea(base, sizeof(FileShareChildArea));
            DC_WaitWriteBufferEmpty();
            WBT_InitChild(WFSi_OnChildSystemCallback);
        }
    }
    OS_RestoreInterrupts(state);
}

void WFS_Start(void) {
    int old;
    data_020a0f38 = 1;
    old = data_020a1fc0.mIsParent;
    data_020a1fc0.mSendRequest = 1;
    if (old == 0) {
        /* WM_ReadStatus copies the WM status block into the child area's
           mStatus (6 on a NULL buffer, then a cache-invalidated copy); its
           aid goes to WBT_SetOwnAid. */
        FileShareChildArea *child = (FileShareChildArea *)data_020a2400;
        WM_ReadStatus(&child->mStatus);
        WBT_SetOwnAid(child->mStatus.aid);
        WBT_RequestSync(1, WFSi_OnChildSystemCallback);
    } else {
        void (*fn)(int);
        data_020a1fc0.mState = 2;
        fn = data_020a1fc0.mStateCallback;
        if (fn != 0) {
            fn(0);
        }
    }
    WFSi_SetMPData();
}

/* The FSFile a child read is in flight on (mReadRequest); only its archive is
 * read here. */

/* Shut the file share down: free the card tables (mTable), and as a child
 * cancel the read in flight and free the receive work (mRecvWork). The
 * mIsParent read goes through data_020a1fc0, not work, as FileShareFree's
 * allocator reads do: the ROM reloads the block's address for it (through
 * work the function comes out shorter). */
void WFS_End(void) {
    FileShareWork *work = &data_020a1fc0;
    OSIntrMode saved0 = OS_DisableInterrupts();

    if (data_020a0f84 != 0) {
        void *table = work->mTable;

        if (table != 0) {
            FileShareFree(table);
            work->mTable = 0;
            work->mTableSize = 0;
        }

        work->mState = 0;

        if (data_020a1fc0.mIsParent == 0) {
            FSFile *p = work->mReadRequest;
            FileShareChildArea *child = (FileShareChildArea *)work->mArea;
            if (p != 0) {
                FSArchive *arc = p->arc;
                work->mReadRequest = 0;
                FS_NotifyArchiveAsyncEnd(arc, FS_RESULT_ERROR);
            }
            FileShareFree(child->mRecvWork);
        } else {
            WFSi_EndTaskThread();
        }

        WBT_End();
        data_020a0f84 = 0;
    }

    OS_RestoreInterrupts(saved0);
}

/* The file share's mState: 2 once it is ready.
   func_0203db64 waits for that before the download-play handshakes. */
int WFS_GetStatus(void) {
    return data_020a1fc0.mState;
}

/* The word at +0xb58 of data_020a3fc0 (see WFS_EnableSync). */
int WFS_GetCurrentBitmap(void) {
    return data_020a3fc0.field_b58;
}

/* Store val with bit 0 clear at +0xb50 of data_020a3fc0, under the IRQ
   lock. Its one caller, func_0203ea5c, passes it WFS_GetCurrentBitmap's result
   when there are more than two players (data_02099e18). */
void WFS_EnableSync(u32 val) {
    OSIntrMode saved = OS_DisableInterrupts();
    data_020a3fc0.field_b50 = val & ~1u;
    OS_RestoreInterrupts(saved);
}

/* Allocate the link's buffers. On a download-play child, rebuild the parent's WMBssDesc into data_020a0f54, keep its
   channel in data_020a0f28 (and in data_020a1064's channel) and step its tgid. */
void func_020408b0(unsigned short arg) {
    data_020a0f14 = arg;
    data_020a0f94 = 0;
    data_020a0f24 = 0;
    data_020a0f2c = 1;
    data_020a0f4c = Memory::Allocate(0x1300, 0x20);
    data_020a0f44 = (WMParentParam *)Memory::Allocate(sizeof(WMParentParam), 0x20);
    data_020a0f48 = Memory::Allocate(0x20, 0x20);
    data_020a0f54 = (WMBssDesc *)Memory::Allocate(sizeof(WMBssDesc), 0x20);
    if (MB_IsMultiBootChild()) {
        MB_ReadMultiBootParentBssDesc(data_020a0f54, 0x200, 0x20, 0, 1);
        data_020a0f28 = data_020a0f54->channel;
        ++data_020a0f54->gameInfo.tgid;
    }
    data_020a0f74 = (WMDataSharingInfo *)Memory::Allocate(sizeof(WMDataSharingInfo), 0x20);
    data_020a0f80 = (WMDataSet *)Memory::Allocate(sizeof(WMDataSet), 0x20);
    data_020a0f58 = 0x220;
    data_020a0f60 = Memory::Allocate(0x220, 0x20);
    data_020a0f64 = 0x480;
    data_020a0f68 = Memory::Allocate(0x480, 0x20);
    data_020a1064.channel = data_020a0f28;
    WM_Init(data_020a0f4c, 1);
}

/* Start the link as the parent (target mode 3), as func_02040790 does
   for a child: from idle enable the hardware, while starting retarget,
   while running as child retarget and reset. func_0203ea5c starts it
   from idle when data_020a0f04 is 1. */
void func_02040820(void) {
    switch (data_020a0f94) {
    case 0:
        data_020a0f94 = 2;
        data_020a0f5c = 3;
        func_0203fd28();
        break;
    case 2:
        data_020a0f5c = 3;
        break;
    case 4:
        data_020a0f5c = 3;
        Wireless_Reset();
        break;
    case 1:
    case 3:
        break;
    }
}

/* Start the link as a child (target mode 4): from idle, mark it starting
   (status 2) and enable the hardware (func_0203fd28); while starting,
   just retarget; while running as parent, retarget and reset
   (Wireless_Reset), which reconnects as a child. func_0203ea5c starts it
   from idle when data_020a0f04 is 2. */
void func_02040790(void) {
    switch (data_020a0f94) {
    case 0:
        data_020a0f94 = 2;
        data_020a0f5c = 4;
        func_0203fd28();
        break;
    case 2:
        data_020a0f5c = 4;
        break;
    case 3:
        data_020a0f5c = 4;
        Wireless_Reset();
        break;
    case 1:
    case 4:
        break;
    }
}

/* Shut the link down by its status data_020a0f94: while starting (2),
   clear the target mode data_020a0f5c so the callback chain winds down;
   while running (3 or 4), clear it and reset (Wireless_Reset). Idle or
   error does nothing. func_0203db64 calls it when a handshake times out,
   func_0203df40 when the link mode drops to 0, and the card pull-out
   thread func_02019f64 until the status reads 0 or 1. */
void func_02040724(void) {
    switch (data_020a0f94) {
    case 0:
        break;
    case 1:
        break;
    case 2:
        data_020a0f5c = 0;
        break;
    case 3:
    case 4:
        data_020a0f5c = 0;
        Wireless_Reset();
        break;
    }
}

/* The link status data_020a0f94: 0 idle, 1 error, 2 starting, 3 running
   as parent, 4 running as child (func_0203fdac copies the target mode
   into it). */
int func_02040714(void) {
    return data_020a0f94;
}

/* This console's AID, data_020a0f24 (0 for the parent); func_0203fec4
   keeps it once connected and func_020402a0 clears it on a reset. */
int func_02040704(void) {
    return data_020a0f24;
}

/* Step the data sharing: WM_StepDataSharing sends sendData and receives into
   data_020a0f80.
   On success store the received set's receivedBitmap in *out and return 1;
   else 0. */
int func_020406b4(unsigned char *sendData, unsigned short *out) {
    if (WM_StepDataSharing(data_020a0f74, sendData, data_020a0f80) != WM_ERRCODE_SUCCESS)
        return 0;
    *out = data_020a0f80->receivedBitmap;
    return 1;
}

/* daPly_c aid's packet in the last shared-data step: WM_GetSharedDataAddress
   on the sharing info data_020a0f74 and received set data_020a0f80.
   func_0203ea5c unpacks each player's packet through it. */
void *func_0204068c(unsigned short aid) {
    return WM_GetSharedDataAddress(data_020a0f74, data_020a0f80, aid);
}

/* Reload the parent parameters from the template at data_020a1064
 * and give the session the next tgid. */
void func_02040638(void) {
    MI_CpuCopy32(&data_020a1064, data_020a0f44, sizeof(WMParentParam));
    data_020a0f20 = data_020a0f20 + 1;
    data_020a0f44->tgid = data_020a0f20;
}

/* Empty: the callback func_0203fd64 hands its open/close-entry request. */
void func_02040634(void *arg) {}

/* The callback func_0203fd28 hands WM_Enable. On success, for target mode data_020a0f5c 0 disable the
   hardware again (func_0203fcec); for 3 or 4 power it on (func_0203fcb0).
   An error sets the link status data_020a0f94 to 1. */
void func_020405b4(void *arg) {
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS) {
        switch (data_020a0f5c) {
        case 0:
            func_0203fcec();
            return;
        case 3:
        case 4:
            func_0203fcb0();
            return;
        }
        return;
    }
    data_020a0f94 = 1;
}

/* The callback func_0203fcec hands WM_Disable. On success the link status data_020a0f94 drops to 0 (idle),
   and a pending target mode data_020a0f5c starts the link again: 3 as
   parent (func_02040820), 4 as child (func_02040790). An error sets the
   status to 1. */
void func_02040504(void *arg) {
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS) {
        switch (data_020a0f5c) {
        case 0:
            data_020a0f94 = 0;
            break;
        case 3:
            data_020a0f94 = 0;
            func_02040820();
            break;
        case 4:
            data_020a0f94 = 0;
            func_02040790();
            break;
        }
    } else {
        data_020a0f94 = 1;
    }
}

/* The callback func_0203fcb0 hands WM_PowerOn. On success, for target mode data_020a0f5c 0 power off again
   (func_0203fc74). For 3 (parent), install the indication callback
   (func_0203fc38), then set the parent up (func_02040638,
   func_0203fbc4); for 4 (child), install it and connect
   (func_0203fb5c). A refused request clears the mode and resets
   (Wireless_Reset). An error sets the link status data_020a0f94 to 1. */
void func_02040408(void *arg) {
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS) {
        switch (data_020a0f5c) {
        case 0:
            func_0203fc74();
            return;
        case 3:
            if (func_0203fc38() == 0)
                return;
            func_02040638();
            if (func_0203fbc4() != 0)
                return;
            data_020a0f5c = 0;
            Wireless_Reset();
            return;
        case 4:
            if (func_0203fc38() != 1)
                return;
            if (func_0203fb5c() != 0)
                return;
            data_020a0f5c = 0;
            Wireless_Reset();
            return;
        }
    } else {
        data_020a0f94 = 1;
    }
}

/* The callback func_0203fc74 hands WM_PowerOff. On success, for target mode data_020a0f5c 0 disable the
   hardware (func_0203fcec); for 3 or 4 power it on again (func_0203fcb0).
   An error sets the link status data_020a0f94 to 1. */
void func_02040388(void *arg) {
    WMCallback *cb = (WMCallback *)arg;
    int s;

    if (cb->errcode == WM_ERRCODE_SUCCESS) {
        s = data_020a0f5c;
        switch (s) {
        case 0:
            func_0203fcec();
            break;
        case 3:
        case 4:
            func_0203fcb0();
            break;
        }
        return;
    }
    data_020a0f94 = 1;
}

/* Empty: the WM indication callback func_0203fc38 installs. */
void func_02040384(void *arg) {}

/* A file-scope object with an empty inline constructor. mwcc emits the
   4-byte bx lr at 0x02075150. The object stays static so the linker drops
   it: data_020a0df4 is padding between data_020a0df0 and data_020a0df8.
   Claiming that word here would list it after cstd_sqrt's .bss. */
static EmptyInit sEmptyInit;

/* The callback Wireless_Reset hands WM_Reset (command 1).
   On success clear the AID data_020a0f24 and go on by target mode
   data_020a0f5c: 0 powers off (func_0203fc74); 3 sets the parent up
   again (func_02040638, then func_0203fbc4); 4 connects to the parent
   again (func_0203fb5c). A refused request clears the mode and resets.
   An error sets the link status data_020a0f94 to 1. */
void func_020402a0(void *arg) {
    WMCallback *cb = (WMCallback *)arg;
    int x;

    if (cb->errcode == WM_ERRCODE_SUCCESS) {
        x = data_020a0f5c;
        data_020a0f24 = 0;
        switch (x) {
        case 0:
            func_0203fc74();
            return;
        case 3:
            func_02040638();
            if (func_0203fbc4() != 0)
                return;
            data_020a0f5c = 0;
            Wireless_Reset();
            return;
        case 4:
            if (func_0203fb5c() != 0)
                return;
            data_020a0f5c = 0;
            Wireless_Reset();
            return;
        }
    } else {
        data_020a0f94 = 1;
    }
}

/* The callback func_0203fbc4 hands WM_SetParentParameter. On success, for target mode data_020a0f5c 3 (parent) start
   the parent (func_0203fb98); if that is refused, clear the mode and
   reset (Wireless_Reset). For 0 and 4 (child) just reset. An error
   clears the mode and resets. */
void func_020401e8(void *arg) {
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS) {
        switch (data_020a0f5c) {
        case 0:
            Wireless_Reset();
            break;
        case 3:
            if (func_0203fb98() != 0) {
                return;
            }
            data_020a0f5c = 0;
            Wireless_Reset();
            break;
        case 4:
            data_020a0f5c = 4;
            Wireless_Reset();
            break;
        }
    } else {
        data_020a0f5c = 0;
        Wireless_Reset();
    }
}

/* The parent's WM_StartParent callback. A connecting child stays only when
 * its MAC address equals one of the three entries of data_020a0fbe whose
 * data_020a0f9d flag is set (func_02042748 compares the three halfwords);
 * data_020a0fa0[aid] then takes the address. Any other child is
 * disconnected (WM_Disconnect). */
void func_02040014(void *arg) {
    WmStartParentCallback *cb = (WmStartParentCallback *)arg;
    int v;

    if (cb->errcode == WM_ERRCODE_SUCCESS) {
        v = cb->state;
        switch (v) {
        case WM_STATECODE_PARENT_START: {
            int sel = data_020a0f5c;
            data_020a0f24 = 0;
            switch (sel) {
            case 0:
                Wireless_Reset();
                break;
            case 3:
                if (func_0203faa8() != 0)
                    return;
                data_020a0f5c = 0;
                Wireless_Reset();
                break;
            case 4:
                data_020a0f5c = 4;
                Wireless_Reset();
                break;
            }
            break;
        }
        case WM_STATECODE_BEACON_SENT:
            break;
        case WM_STATECODE_CONNECTED: {
            u16 buf[3];
            int matched;
            int cnt;
            u8 *flag_p;
            u16 *item_p;

            matched = 0;
            MI_CpuCopy8(cb->macAddress, buf, 6);

            flag_p = data_020a0f9d;
            item_p = data_020a0fbe[0];
            cnt = 1;
            do {
                if (*flag_p != 0) {
                    if (func_02042748(buf, item_p) != 0) {
                        u16 idx = cb->aid;
                        MI_CpuCopy8(buf, data_020a0fa0[idx], 6);
                        matched = 1;
                        break;
                    }
                }
                cnt++;
                flag_p++;
                item_p += 3;
            } while (cnt < 4);

            /* matched kept: an int-returning inline search predicate re-colours the loop */
            if (matched != 0) {
                return;
            }
            WM_Disconnect(0, cb->aid);
            break;
        }
        case WM_STATECODE_DISCONNECTED:
            break;
        }
    } else {
        data_020a0f5c = 0;
        Wireless_Reset();
    }
}

/* The callback func_0203fb5c hands WM_StartConnect. An
   error clears data_020a0f5c and resets the link (Wireless_Reset). On
   success it resets when data_020a0f5c is 0 or 3, returns when it is
   anything but 4, and at 4 follows the state:
     CONNECTED: keep the AID in data_020a0f24 (func_02040704 returns
       it); unless data_020a0f94 is 4, start MP (func_0203faa8 returns 1
       when WM_StartMP is accepted) and clear data_020a0f5c and reset if not.
     DISCONNECTED: clear data_020a0f5c; reset unless data_020a0f94 is 2.
     CONNECT_START, BEACON_LOST: nothing.
     anything else: clear data_020a0f5c and reset. */
void func_0203fec4(void *arg) {
    WmStartConnectCallback *p = (WmStartConnectCallback *)arg;
    int g;
    if (p->errcode == WM_ERRCODE_SUCCESS) {
        g = data_020a0f5c;
        switch (g) {
        case 0:
        case 3:
            Wireless_Reset();
            return;
        case 4:
            switch (p->state) {
            case WM_STATECODE_CONNECTED:
                data_020a0f24 = p->aid;
                if (data_020a0f94 == 4)
                    return;
                if (func_0203faa8() != 0)
                    return;
                data_020a0f5c = 0;
                Wireless_Reset();
                return;
            case WM_STATECODE_DISCONNECTED:
                data_020a0f5c = 0;
                if (data_020a0f94 == 2)
                    return;
                Wireless_Reset();
                return;
            case WM_STATECODE_CONNECT_START:
            case WM_STATECODE_BEACON_LOST:
                break;
            default:
                data_020a0f5c = 0;
                Wireless_Reset();
                return;
            }
            break;
        default:
            return;
        }
    } else {
        data_020a0f5c = 0;
        Wireless_Reset();
    }
}

/* The WM_StartMP callback func_0203faa8 installs. On MP_START: in link
   mode 0 reset; as parent (3) or child (4) start data sharing
   (func_0203fa50), and if that fails clear the mode and reset, else copy
   the mode into the link status data_020a0f94 and go on (WFS_Start).
   TIMEOUT, INVALID_POLLBITMAP and SEND_FAILED are let pass; any other
   error clears the mode and resets. */
void func_0203fdac(void *arg) {
    WMStartMPCallback *cb = (WMStartMPCallback *)arg;
    unsigned short errcode = cb->errcode;
    if (errcode == WM_ERRCODE_SUCCESS) {
        switch (cb->state) {
        case WM_STATECODE_MP_START:
            switch (data_020a0f5c) {
            case 0:
                Wireless_Reset();
                return;
            case 3:
            case 4:
                if (func_0203fa50() == 0) {
                    data_020a0f5c = 0;
                    Wireless_Reset();
                } else {
                    data_020a0f94 = data_020a0f5c;
                    WFS_Start();
                }
                return;
            default:
                return;
            }
        case WM_STATECODE_MPEND_IND:
        case WM_STATECODE_MP_IND:
            return;
        default:
            return;
        }
    } else {
        switch (errcode) {
        case WM_ERRCODE_TIMEOUT:
        case WM_ERRCODE_INVALID_POLLBITMAP:
        case WM_ERRCODE_SEND_FAILED:
            return;
        default:
            data_020a0f5c = 0;
            Wireless_Reset();
            return;
        }
    }
}

/* Open (data_020a0f2c 1) or close (0) the parent to new children:
   func_0206259c sends WM command 0x23 with that flag, MP-parent state only,
   and func_02040634 (empty) as the callback. func_0203ea5c closes it when
   every child slot is taken and reopens it when one frees, which is what
   the SDK's WM_SetEntry does. As func_0203fc74 otherwise. */
int func_0203fd64(void) {
    if (func_0206259c(func_02040634, data_020a0f2c) != WM_ERRCODE_OPERATING) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}

/* Enable the wireless hardware: WM_Enable sends WM command 3 (the
   SDK's WM_APIID_ENABLE, ready state only) with func_020405b4 as the
   callback. As func_0203fc74 otherwise. */
int func_0203fd28(void) {
    if (WM_Enable(func_020405b4) != WM_ERRCODE_OPERATING) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}

/* Disable the wireless hardware: WM_Disable sends WM command 4 (the
   SDK's WM_APIID_DISABLE, stop state only) with func_02040504 as the
   callback. As func_0203fc74 otherwise. */
int func_0203fcec(void) {
    if (WM_Disable(func_02040504) != WM_ERRCODE_OPERATING) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}

/* Power the wireless hardware on: WM_PowerOn sends WM command 5 (the
   SDK's WM_APIID_POWER_ON, stop state only) with func_02040408 as the
   callback. As func_0203fc74 otherwise. */
int func_0203fcb0(void) {
    if (WM_PowerOn(func_02040408) != WM_ERRCODE_OPERATING) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}

/* Power the wireless hardware off: WM_PowerOff sends WM command 6 (the
   SDK's WM_APIID_POWER_OFF, idle state only) with func_02040388 as the
   callback. A refusal sets the link status data_020a0f94 to 1 (error) and
   returns 0; otherwise 1. func_0203fcb0, func_0203fcec, func_0203fd28
   and func_0203fd64 are the same wrapper on other commands.
   Wireless_Reset is close: it sends WM command 1 (WM_Reset)
   but shuts the file share down (WFS_End) before it checks the
   result. */
int func_0203fc74(void) {
    if (WM_PowerOff(func_02040388) != WM_ERRCODE_OPERATING) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}

/* Install func_02040384 as the WM indication callback. A refusal sets the link status data_020a0f94 to 1
   (error). */
int func_0203fc38(void) {
    if (WM_SetIndCallback(func_02040384) != WM_ERRCODE_SUCCESS) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}

int Wireless_Reset(void) {
    int r = WM_Reset(func_020402a0);
    WFS_End();
    if (r != WM_ERRCODE_OPERATING) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}

/* Set the parent parameters: WM_SetParentParameter on the
   WMParentParam in data_020a0f44, with func_020401e8 as the callback, which
   starts the parent (func_0203fb98). 1 when the request is accepted (2). */
int func_0203fbc4(void) {
    return WM_SetParentParameter(func_020401e8, data_020a0f44) == WM_ERRCODE_OPERATING;
}

/* Start as parent: WM_StartParent sends WM command 8 (the SDK's
   WM_APIID_START_PARENT) with func_02040014 as the callback. 1 when the
   request is accepted (2). */
int func_0203fb98(void) {
    return WM_StartParent(func_02040014) == WM_ERRCODE_OPERATING;
}

/* Connect to the chosen parent: WM_StartConnect on the
   WMBssDesc in data_020a0f54, no SSID, with func_0203fec4 as the callback.
   1 when the request is accepted (2). */
int func_0203fb5c(void) {
    return WM_StartConnect(func_0203fec4, data_020a0f54, 0) == WM_ERRCODE_OPERATING;
}

/* func_0203faa8 at 0x0203faa8 (arm9), size 0xb4
 * Compiler mwccarm 1.2/sp2p3, flags:
 * -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 * The ne-path must be textually first: plain ==/!= return spellings canonicalize to
 * moveq-first (see notes/mwccarm-codegen.md); return r == 2, ?1:0, if/else and a
 * switch were re-tried and differ, so the r == 2 arm is left empty. */

/* WM_StartMPEx's mpFreq (the sixth argument) is 1 unless the parent
 * parameters ask for continuous MP (CS_Flag). */
int func_0203faa8(void) {
    unsigned short flag = (unsigned short)(data_020a0f44->CS_Flag == 0 ? 1 : 0);
    unsigned short s0 = (unsigned short)data_020a0f58;

    int r = WM_StartMPEx(func_0203fdac, (u16 *)data_020a0f68, (unsigned short)data_020a0f64,
                         (u16 *)data_020a0f60, s0, flag, 0, 0, 0, 0, 1);

    if (r == WM_ERRCODE_OPERATING) {
    } else {
        return 0;
    }
    return 1;
}

/* Start data sharing once MP is running: WM_StartDataSharing
   on the 0x420-byte WMDataSharingInfo func_020408b0 allocates
   into data_020a0f74, port 0xc, the four AIDs 0..3 (0xf), 0x20 bytes per
   station, and double mode when data_020a0f14 (func_020408b0's argument) is
   at most 1. 1 when it succeeds, 0 when it fails. */
int func_0203fa50(void) {
    int flag = (data_020a0f14 <= 1) ? 1 : 0;
    if (WM_StartDataSharing(data_020a0f74, 0xc, 0xf, 0x20, flag) != WM_ERRCODE_SUCCESS)
        return 0;
    return 1;
}

/* Pick a quiet wireless channel. sysBuf is the WM system buffer
   (func_0203e20c allocates 0xb000 bytes), callback the one that receives
   the {result, channel} pair (func_0203e978). Refuse with -10 unless idle
   (state 3); then initialize WM (WM_Initialize, command 0, DMA 1) with
   func_0203f920 as the callback: -1 if that is refused, else state 0.
   Every allowed channel is then measured and one of the least busy drawn
   at random (func_0203f714), the shape of NitroSDK's wireless sample
   library's channel measurement. */
void func_0203f9d4(void *sysBuf, ChannelMeasureCallback callback) {
    data_020a0f90 = callback;
    if (data_02099e20 != 3) {
        func_0203f650(-10);
        return;
    }
    if (WM_Initialize(sysBuf, func_0203f920, 1) != WM_ERRCODE_OPERATING) {
        func_0203f650(-1);
        return;
    }
    func_0203f6a4(0);
}

/* The WM callback for the channel measurement, by the API id of the
   result: WM_Initialize fails with -2 or starts measuring,
   WM_MeasureChannel goes to func_0203f714, and WM_End returns to idle,
   reports data_020a0f18 and drops the caller's callback. Anything else
   just reports. */
void func_0203f920(void *arg) {
    WMCallback *cb = (WMCallback *)arg;

    switch (cb->apiid) {
    case WM_APIID_INITIALIZE:
        if (cb->errcode != WM_ERRCODE_SUCCESS)
            func_0203f650(-2);
        else
            func_0203f8a4();
        break;
    case WM_APIID_MEASURE_CHANNEL:
        func_0203f714((WMMeasureChannelCallback *)arg);
        break;
    case WM_APIID_END:
        func_0203f6a4(3);
        func_0203f650(data_020a0f18);
        data_020a0f90 = 0;
        break;
    default:
        func_0203f650(data_020a0f18);
        break;
    }
}

/* Begin measuring once WM has initialized: clear the outcome, seed the
   best busy ratio above any real one (0x65, past 100 percent) and measure
   from channel 1. No allowed channel ends with -9, a refused request with
   -3; otherwise enter state 1. */
void func_0203f8a4(void) {
    int r;
    data_020a0f18 = 0;
    data_020a0f34 = 0x65;
    r = func_0203f818(1);
    if (r == 1) {
        func_0203f6e8(-9);
    } else if (r == 2) {
        func_0203f6e8(-3);
    } else {
        func_0203f6a4(1);
    }
}

/* Measure the next channel from `channel` on that the allowed-channel mask
   (WM_GetAllowedChannel, read from the WM status) permits: 1 when none is left
   up to channel 14, 2 when the measure request (WM_MeasureChannel: WM command
   0x1e, the SDK's WM_APIID_MEASURE_CHANNEL, with ccaMode 3, edThreshold
   0x11 and a measure time of 0x1e) is refused, else 0. */
int func_0203f818(u16 channel) {
    u16 n = channel;
    u16 mask = WM_GetAllowedChannel();
    while (((1 << (n - 1)) & mask) == 0) {
        n++;
        if (n > 0xe)
            return 1;
    }
    if (WM_MeasureChannel(func_0203f920, 3, 0x11, n, 0x1e) != WM_ERRCODE_OPERATING)
        return 2;
    return 0;
}

/* One channel measured: end with -4 if it failed. Otherwise keep the least
   busy ratio in data_020a0f34 and the channels that share it in
   data_020a0fd0 (data_020a0ef4 of them), then measure the next allowed
   channel. Once none is left, draw one of the kept channels with
   OS_GetTickLo and finish with it; a refused request finishes with -3. */
void func_0203f714(WMMeasureChannelCallback *s) {
    u16 a, cur, v;
    int r;
    if (s->errcode != WM_ERRCODE_SUCCESS) {
        func_0203f6e8(-4);
        return;
    }
    a = s->ccaBusyRatio;
    cur = data_020a0f34;
    v = s->channel;
    if (a < cur) {
        data_020a0f34 = a;
        data_020a0ef4 = 1;
        data_020a0fd0[0] = v;
    } else if (a == cur) {
        data_020a0fd0[data_020a0ef4] = v;
        data_020a0ef4++;
    }
    r = func_0203f818((u16)(v + 1));
    if (r == 1) {
        int n = OS_GetTickLo();
        int idx = n % data_020a0ef4;
        data_020a0f18 = data_020a0fd0[idx];
        func_0203f6e8(data_020a0f18);
    } else if (r == 2) {
        func_0203f6e8(-3);
    }
}

/* Finish a measurement with outcome v (a channel, or a negative error):
   keep it in data_020a0f18 for the WM_End callback to report, start
   WM_End and enter state 2. */
void func_0203f6e8(short v) {
    data_020a0f18 = v;
    func_0203f6b4();
    func_0203f6a4(2);
}

/* Shut the wireless hardware down after a measurement: WM_End
   sends WM_APIID_END (2) through callback slot 2, with
   func_0203f920 as the callback, and anything but 2 (the SDK's
   WM_ERRCODE_OPERATING, "accepted") halts. */
void func_0203f6b4(void) {
    if (WM_End(func_0203f920) == WM_ERRCODE_OPERATING)
        return;
    OS_Terminate();
}

/* Set the channel measurement's state word. As the callers use it: 0
   initializing WM (func_0203f9d4), 1 measuring (func_0203f8a4), 2 shutting
   WM down (func_0203f6e8), 3 idle, set when WM_End completes
   (func_0203f920). func_0203f9d4 refuses to start unless it reads 3. */
void func_0203f6a4(int v) {
    data_02099e20 = v;
}

/* Report the channel measurement's outcome to the caller's callback
   (data_020a0f90, set by func_0203f9d4): a positive x is the chosen channel,
   sent with result 0; zero or a negative error code is sent as the result
   with channel 0. */
void func_0203f650(int x) {
    ChannelMeasureReply reply;
    ChannelMeasureCallback fn = data_020a0f90;
    if (fn == 0)
        return;
    if (x > 0) {
        reply.result = 0;
        reply.channel = (unsigned short)x;
    } else {
        reply.result = (short)x;
        reply.channel = 0;
    }
    fn(&reply);
}

/* func_0203f644 @ 0x203f644 (arm9) -- tail-call veneer to WFS_GetStatus (0x2040a94).
 * ldr ip, [pc]; bx ip; .word 0x2040a94
 */

void func_0203f644(int state) {
    WFS_GetStatus();
}

/* func_0203f604 at 0x0203f604
 *
 * Combined allocate/free helper. r0 (first arg) is unused.
 *   if (ptr == 0) return Memory::Allocate(size, 0x20);
 *   Memory::Deallocate(ptr);
 *   return 0;
 *
 *   0x0203c1d8 = Memory::Allocate(u32 size, s32 align)
 *   0x0203c1b4 = Memory::Deallocate(void *ptr)
 */

void *func_0203f604(void *arg, unsigned int size, void *ptr) {
    if (ptr == 0)
        return Memory::Allocate(size, 0x20);
    Memory::Deallocate(ptr);
    return 0;
}

/* The link step for modes 1 and 2 (func_0203df40): pack the local input
   record into the send packet (see data_020a1020), hand it to the link, and
   unpack each player's received packet (func_0204068c, by player index) into
   data_020a1154. The loop runs until the link reports the frame synced or
   the timeout (0x12C frames, 0x4B0 the first time) runs out. */
void func_0203ea5c(void) {
    /* Declared in the ROM's register-assignment order: grouping these by
       use moves 42 words. */
    s32 timeout;
    s32 synced;
    u16(*mac)[3];
    s32 slot;
    MBUserInfo *user;
    u16 sendStatus;
    PlayerInputRecord *endRec;
    PlayerInputRecord *inRec;
    u16(*peerMac)[3];
    PlayerInputRecord *frameRec;
    s32 frameIdx;
    s32 allReady;
    PlayerInputRecord *anyRec;
    MBUserInfo *info;
    s32 endIdx;
    PlayerInputRecord *verRec;
    s32 anyIdx;
    s32 allEnd;
    s32 peer;
    s32 inIdx;
    PlayerInputRecord *own;
    s32 verIdx;
    s32 aid;
    s32 allIn;
    u8 *hostPkt;
    u8 *pkt;
    u8 chunk;
    s32 anyIn;
    PlayerInputRecord *rdyRec;
    u32 peers;
    s32 rdyIdx;

    /* allReady, allIn, anyIn and allEnd stay 0/1 search flags: an inline
       AllPeersHave()/AnyPeerHas() predicate re-colours registers (13-17 insns).
       Their loops walk a PlayerInputRecord pointer beside the index; indexing
       data_020a1154[i] directly changes the loop's codegen. */
    if (data_02099e1c != 0) {
        func_020408b0(2);
        data_02099e1c = 0;
    }
    synced = 0;
    timeout = data_020a0ef0 != 0 ? 0x12C : 0x4B0;
    data_020a0f1c |= 0x2000;
    data_020a0f1c &= 0xF0FF;
    data_020a0f1c |= (data_020a0f28 << 8) & 0xF00;
    while (!synced && timeout != 0) {
        if (data_020a0ef8 != 0) {
            if (data_020a0f1c & 0x4000) {
                data_020a0ef8--;
            }
            if (data_020a0ef8 != 0) {
                if (data_020a0ef8 > 4) {
                    chunk = 3;
                } else {
                    chunk = data_020a0ef8 - 1;
                }
                data_020a1040.flags |= 1;
                MI_CpuCopy8(&data_020a1040.flags, data_020a1020, 2);
                MI_CpuCopy8(&chunk, &data_020a1020[0x2], 1);
                MI_CpuCopy8(&data_020a10a4[chunk], &data_020a1020[0x3], 0x16);
            } else {
                data_020a1040.flags &= 0xFFFE;
            }
        }
        if (data_020a0ef8 == 0) {
            MI_CpuCopy8(&data_020a1040.flags, data_020a1020, 2);
            MI_CpuCopy8(&data_020a1040.frame, &data_020a1020[0x2], 4);
            MI_CpuCopy8(&data_020a1040.keys, &data_020a1020[0x6], 2);
            MI_CpuCopy8(&data_020a1040.touch.x, &data_020a1020[0x8], 1);
            MI_CpuCopy8(&data_020a1040.touch.y, &data_020a1020[0x9], 1);
            MI_CpuCopy8(&data_020a1040.touch.touch, &data_020a1020[0xa], 1);
            MI_CpuCopy8(&data_020a1040.inputYaw, &data_020a1020[0xb], 2);
            MI_CpuCopy8(&data_020a0f08, &data_020a1020[0xd], 1);
            MI_CpuCopy8(&data_020a0f00, &data_020a1020[0xe], 1);
            MI_CpuCopy8(data_020a1040.settings, &data_020a1020[0xf], 0x11);
        }
        data_020a0f1c &= 0xBFFF;
        switch (func_02040714()) {
        case 0:
            if (data_020a0f04 == 1) {
                func_02040820();
                func_02040c34(1, 1, &func_0203f644, &func_0203f604, 0);
            } else if (data_020a0f04 == 2) {
                func_02040790();
                func_02040c34(0, 1, &func_0203f644, &func_0203f604, 0);
            }
            synced = 0;
            break;
        case 1:
            synced = 0;
            break;
        case 2:
            synced = 0;
            break;
        case 3:
            synced = func_020406b4(data_020a1020, &sendStatus);
            if (synced != 0) {
                data_020a0f1c |= 0xC000;
            }
            if (data_020a1154[0].flags & 0x2000) {
                synced = 1;
            }
            break;
        case 4:
            if ((data_020a1040.flags & 0x4000) && (data_020a1154[0].flags & 0x2000)) {
                synced = 1;
            } else {
                synced = func_020406b4(data_020a1020, &sendStatus);
                if (synced != 0) {
                    data_020a0f1c |= 0xC000;
                }
                if (data_020a1040.flags & 0x4000) {
                    synced = 1;
                }
            }
            break;
        default:
            synced = 0;
            break;
        }
        if (data_020a0f1c & 0x4000) {
            data_020a0f10 = func_02040704();
            data_02099e18 = 0;
            timeout = 0x12C;
            hostPkt = (u8 *)func_0204068c(0);
            if (hostPkt != 0) {
                MI_CpuCopy8(hostPkt, &data_020a1154[0].flags, 2);
                if (data_020a1154[0].flags & 0x8000) {
                    if (data_020a1154[0].flags & 1) {
                        if (data_020a0f10 != 0) {
                            MI_CpuCopy8(hostPkt + 2, &chunk, 1);
                            switch (chunk) {
                            default:
                                break;
                            case 0:
                                data_020a1040.flags &= 0xFFFE;
                                MI_CpuClear8(&data_020a10a4[0], 0x16);
                                MI_CpuCopy8(hostPkt + 3, &data_020a10a4[0], 0x16);
                                break;
                            case 1:
                                MI_CpuClear8(&data_020a10a4[1], 0x16);
                                MI_CpuCopy8(hostPkt + 3, &data_020a10a4[1], 0x16);
                                break;
                            case 2:
                                MI_CpuClear8(&data_020a10a4[2], 0x16);
                                MI_CpuCopy8(hostPkt + 3, &data_020a10a4[2], 0x16);
                                break;
                            case 3:
                                MI_CpuClear8(&data_020a10a4[3], 0x16);
                                MI_CpuCopy8(hostPkt + 3, &data_020a10a4[3], 0x16);
                                break;
                            }
                        }
                        synced = 0;
                    } else {
                        for (aid = 0; aid < 4; aid++) {
                            pkt = (u8 *)func_0204068c(aid);
                            MI_CpuClear8(&data_020a1154[aid], sizeof(PlayerInputRecord));
                            if (pkt != 0) {
                                MI_CpuCopy8(pkt, &data_020a1154[aid].flags, 2);
                                if (data_020a1154[aid].flags & 0x8000) {
                                    data_02099e18++;
                                    MI_CpuCopy8(pkt + 2, &data_020a1154[aid].frame, 4);
                                    MI_CpuCopy8(pkt + 6, &data_020a1154[aid].keys, 2);
                                    MI_CpuCopy8(pkt + 8, &data_020a1154[aid].touch.x, 1);
                                    MI_CpuCopy8(pkt + 9, &data_020a1154[aid].touch.y, 1);
                                    MI_CpuCopy8(pkt + 0xA, &data_020a1154[aid].touch.touch, 1);
                                    MI_CpuCopy8(pkt + 0xB, &data_020a1154[aid].inputYaw, 2);
                                    MI_CpuCopy8(pkt + 0xF, data_020a1154[aid].settings, 0x11);
                                    if ((data_020a0f10 != 0) && (aid == 0)) {
                                        MI_CpuCopy8(pkt + 0xD, &data_020a0f08, 1);
                                        MI_CpuCopy8(pkt + 0xE, &data_020a0f00, 1);
                                    }
                                }
                            }
                        }
                        if (data_020a1154[data_020a0f10].flags & 0x8000) {
                            if (data_020a0f10 == 0) {
                                if (data_020a0ef8 == 0) {
                                    peers = data_02099e18;
                                    if (peers > 1) {
                                        allReady = 1;
                                        for (rdyRec = &data_020a1154[1], rdyIdx = 1; rdyIdx < 4;
                                             rdyRec++, rdyIdx++) {
                                            if ((rdyRec->flags & 0x8000) && !(rdyRec->flags & 1)) {
                                                allReady = 0;
                                                break;
                                            }
                                        }
                                        if ((allReady != 0) && (peers > 1)) {
                                            data_020a0ef8 = 5;
                                            for (slot = 1, mac = &data_020a0fa0[1],
                                                user = &data_020a10a4[1];
                                                 slot < 4; mac++, user++, slot++) {
                                                if (func_0204271c(*mac) == 0) {
                                                    for (peerMac = data_020a0fbe,
                                                        info = &data_020a10fc[1], peer = 1;
                                                         peer < 4; peerMac++, info++, peer++) {
                                                        if (func_02042748(*mac, *peerMac) != 0) {
                                                            MI_CpuCopy8(info, user, 0x16);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                if (data_02099e18 > 1) {
                                    allIn = 1;
                                    for (inRec = &data_020a1154[1], inIdx = 1; inIdx < 4;
                                         inRec++, inIdx++) {
                                        if ((inRec->flags & 0x8000) && !(inRec->flags & 0x4000)) {
                                            allIn = 0;
                                            break;
                                        }
                                    }
                                    if ((allIn != 0) && !(data_020a1154[0].flags & 0x2000) &&
                                        !(data_020a1040.flags & 0x2000)) {
                                        if (data_02099e18 > 2) {
                                            WFS_EnableSync(WFS_GetCurrentBitmap());
                                        }
                                        data_020a0f2c = 0;
                                        while (func_0203fd64() != 1) {
                                        }
                                        data_020a1040.flags |= 0x2000;
                                    }
                                    anyIn = 0;
                                    for (anyRec = &data_020a1154[1], anyIdx = 1; anyIdx < 4;
                                         anyRec++, anyIdx++) {
                                        if ((anyRec->flags & 0x8000) && (anyRec->flags & 0x4000)) {
                                            anyIn = 1;
                                            break;
                                        }
                                    }
                                    if ((anyIn == 0) && (data_020a1154[0].flags & 0x2000) &&
                                        (data_020a1040.flags & 0x2000)) {
                                        data_020a0f2c = 1;
                                        while (func_0203fd64() != 1) {
                                        }
                                        data_020a1040.flags &= 0xDFFF;
                                    }
                                }
                            }
                            for (frameRec = data_020a1154, frameIdx = 0; frameIdx < 4;
                                 frameRec++, frameIdx++) {
                                if ((frameRec->flags & 0x8000) &&
                                    (frameRec->frame != data_020a1154[data_020a0f10].frame)) {
                                    data_020a1040.frame = data_020a1154[0].frame;
                                    data_020a0f1c |= 2;
                                    if (data_020a0efc == 0) {
                                        synced = 0;
                                    }
                                    break;
                                }
                            }
                        } else {
                            synced = 0;
                        }
                    }
                } else {
                    synced = 0;
                }
            } else {
                synced = 0;
            }
        }
        if (timeout != 0) {
            timeout--;
        }
        if (synced == 0) {
            func_02042778();
            if (data_020a0f1c & 0x4000) {
                func_02042778();
            }
        }
    }
    data_020a0f1c &= 0xDFFF;
    if (timeout != 0) {
        own = &data_020a1154[data_020a0f10];
        for (verIdx = 0, verRec = data_020a1154; verIdx < 4; verIdx++, verRec++) {
            if (verRec->flags & 0x8000) {
                if (verRec->unk22 != own->unk22) {
                    data_020a0f1c |= 8;
                    break;
                } else if (verRec->unk22 != own->unk22) {
                    /* Unreachable: the cartridge tests the same byte twice
                       (unk23 here moves the code). */
                    data_020a0f1c |= 0x10;
                    break;
                }
            }
        }
        if ((data_020a1154[0].flags & 0x2000) || (data_020a1040.flags & 0x4000)) {
            data_020a0efc = 3;
        }
        if (data_020a0efc != 0) {
            data_020a0f1c &= 0xFF60;
            /* The cartridge stores unk22 twice and never clears unk23
               (unk23 in the second store moves 22 words). */
            data_020a1040.unk22 = 0;
            data_020a1040.unk22 = 0;
            data_020a0f30 = 0;
            data_020a1040.flags &= 0xEFFF;
            data_020a0efc--;
        }
        if (data_020a0efc != 0) {
            return;
        }
        allEnd = 1;
        for (endIdx = 0, endRec = data_020a1154; endIdx < 4; endIdx++, endRec++) {
            if ((endRec->flags & 0x8000) && !(endRec->flags & 0x1000)) {
                allEnd = 0;
                break;
            }
        }
        if (!(data_020a1040.flags & 0x1000)) {
            return;
        }
        if (allEnd == 0) {
            return;
        }
        data_020a1040.flags &= 0xEFFF;
        data_020a0f30 = 1;
        return;
    }
    data_020a0f04 = 0;
    data_020a0f1c |= 1;
}

/* The parent state callback func_0203e20c installs
   (MB_CommSetParentStateCallback). When child aid (0..3) connects it keeps
   the child's MAC address from the WM record in data_020a0fb8; on
   MB_COMM_PSTATE_END it raises data_020a0f70, which func_0203e20c waits on.
   The other states are ignored. */
void func_0203e9a8(u16 aid, u32 status, void *arg) {
    switch (status) {
    case MB_COMM_PSTATE_NONE:
    case MB_COMM_PSTATE_INIT_COMPLETE:
        break;
    case MB_COMM_PSTATE_CONNECTED:
        if (aid >= 4)
            return;
        if (arg == 0)
            return;
        MI_CpuCopy8(((WmStartParentCallback *)arg)->macAddress, data_020a0fb8 + aid * 6, 6);
        break;
    case MB_COMM_PSTATE_DISCONNECTED:
    case MB_COMM_PSTATE_KICKED:
    case MB_COMM_PSTATE_REQ_ACCEPTED:
    case MB_COMM_PSTATE_SEND_PROCEED:
    case MB_COMM_PSTATE_SEND_COMPLETE:
    case MB_COMM_PSTATE_BOOT_REQUEST:
    case MB_COMM_PSTATE_BOOT_STARTABLE:
    case MB_COMM_PSTATE_REQUESTED:
    case MB_COMM_PSTATE_MEMBER_FULL:
        break;
    case MB_COMM_PSTATE_END:
        data_020a0f70 = 1;
        break;
    case MB_COMM_PSTATE_ERROR:
    case MB_COMM_PSTATE_WAIT_TO_SEND:
        break;
    }
}

/* The callback func_0203e20c hands func_0203f9d4: on success, keep the
   channel in data_020a0f28 (as func_020408b0 sets it from the parent's
   WMBssDesc) and raise data_020a0f0c. */
void func_0203e978(ChannelMeasureReply *reply) {
    if (reply->result != 0)
        return;
    data_020a0f28 = reply->channel;
    data_020a0f0c = 1;
}

/* Link mode 3's step (func_0203df40), a download-play parent by its
   shape. Step 0 allocates 0xb000 bytes of work and picks a channel
   (func_0203f9d4, with func_0203e978 keeping it). Step 2 starts the
   parent with ids from data_020a1064 (the block func_0203db64 keeps),
   this console's profile and func_0203e9a8 as the state callback. Step 3
   seats up to three children: a child in state 0xa is answered with 1,
   one past the third with 0, and once data_020a0f70 is up and all are
   counted every child is answered with 2. Step 4 waits until each seated
   child is ready (MB_CommIsBootable), step 5 answers them with 3, and step 6
   ends it. A stop request (data_020a0f40) in step 3, or a child lost in
   steps 4 and 5, jumps to step 6; a lost child also sets link error bit
   4. Every frame the local input record data_020a1040 is copied into all
   four records of data_020a1154, and the channel goes into bits 8-11 of
   data_020a0f1c. */
void func_0203e20c(void) {
    if (data_020a0f40 != 0 && data_020a0f50 == 3) {
        data_020a0f50 = 6;
    }
    if (data_020a0f50 > 3 && data_020a0f50 < 6 &&
        (data_020a0f08 != (u8)(MB_CommGetChildrenNumber() + 1) ||
         (u32)MB_CommGetChildrenNumber() < 1)) {
        data_020a0f40 = 1;
        data_020a0f50 = 6;
        data_020a0f1c |= 4;
    }

    switch (data_020a0f50) {
    case 0: {
        void *ptr = Memory::Allocate(0xB000, 0x20);
        data_020a0f3c = ptr;
        data_020a0f0c = 0;
        func_0203f9d4(ptr, func_0203e978);
        data_020a0f50 += 1;
        data_020a0f1c &= 0xFFFB;
        break;
    }

    case 1:
        if (data_020a0f0c != 0) {
            data_020a0f50 += 1;
        }
        break;

    case 2: {
        void *p;
        int size;
        data_020a0f20 = (u16)OS_GetTickLo();
        MB_Init(data_020a0f3c, data_020a10fc, data_020a1064.ggid[0], data_020a1064.ggid[1],
                data_020a0f20, 1);
        MB_SetParentCommParam(0x1fe, 8, 3);
        MB_CommSetParentStateCallback(func_0203e9a8);
        MB_StartParent(data_020a0f28);
        size = MB_GetSegmentLength(0);
        p = Memory::Allocate(size, 0x20);
        data_020a0f7c = p;
        if (MB_ReadSegment(0, p, size) != 0) {
            MB_RegisterFile(&data_020a0fec, data_020a0f7c);
        }
        data_020a0f70 = 0;
        data_020a0f50 += 1;
        break;
    }

    case 3: {
        int cnt2;
        int i2;
        u8 *p9;
        MBUserInfo *profile;
        const MBUserInfo *found;
        u8 *pIdx;
        MBUserInfo *childProfile;
        int idx;
        MBCommResponseRequestType sel1;
        int oneVal;

        cnt2 = 1;
        i2 = cnt2;
        p9 = data_020a0f9d;
        profile = &data_020a10fc[1];

        data_020a0f1c |= 0x9000;
        data_020a0f08 = cnt2;
        do {
            *p9 = 0;
            MI_CpuClear8(profile, 0x16);
            i2 += 1;
            p9 += 1;
            profile++;
        } while (i2 < 4);

        WM_ReadStatus(&data_020a1680);

        pIdx = data_020a0f9d;
        childProfile = &data_020a10fc[1];
        idx = 1;
        sel1 = MB_COMM_RESPONSE_REQUEST_ACCEPT;
        oneVal = idx;
        do {
            found = MB_CommGetChildUser(idx);
            if (found != 0 && ((data_020a1680.child_bitmap >> idx) & 1)) {
                if (idx < 4) {
                    if (MB_CommGetParentState(idx) == MB_COMM_PSTATE_REQUESTED) {
                        MB_CommResponseRequest(idx, sel1);
                    } else if (MB_CommGetParentState(idx) == MB_COMM_PSTATE_WAIT_TO_SEND) {
                        cnt2 += 1;
                    }
                    *pIdx = oneVal;
                    MI_CpuCopy8(found, childProfile, 0x16);
                    data_020a0f08 += 1;
                } else if (MB_CommGetParentState(idx) == MB_COMM_PSTATE_REQUESTED) {
                    MB_CommResponseRequest(idx, MB_COMM_RESPONSE_REQUEST_KICK);
                }
            }
            idx += 1;
            pIdx += 1;
            childProfile++;
        } while (idx < 0x10);

        if (data_020a0f70 != 0 && data_020a0f08 == cnt2) {
            if ((u32)data_020a0f08 > 1U) {
                u8 j;
                u8 cnt;
                int result;
                MBCommResponseRequestType sel2;
                cnt = 0;
                j = 1;
                sel2 = MB_COMM_RESPONSE_REQUEST_DOWNLOAD;
                do {
                    if (MB_CommResponseRequest(j, sel2) == 1) {
                        cnt = (u8)(cnt + 1);
                    }
                    j = (u8)(j + 1);
                } while ((u32)j <= 0xF);
                result = (cnt != 0) ? 1 : 0;
            }
            data_020a0f50 += 1;
        }
        break;
    }

    case 4: {
        int ok = 1;
        u8 *p = data_020a0f9d;
        int k = 1;
        do {
            if (*p != 0 && MB_CommIsBootable(k) != 1) {
                ok = 0;
            }
            k += 1;
            p += 1;
        } while (k < 4);
        if (ok != 0) {
            data_020a0f50 += 1;
            data_020a0f6c = 0;
            data_020a0f78 = 1;
        }
        break;
    }

    case 5:
        if ((u32)data_020a0f08 > 1U) {
            int result;
            do {
                u8 m;
                u8 cnt;
                cnt = 0;
                m = 1;
                do {
                    if (MB_CommResponseRequest(m, MB_COMM_RESPONSE_REQUEST_BOOT) == 1) {
                        cnt = (u8)(cnt + 1);
                    }
                    m = (u8)(m + 1);
                } while ((u32)m <= 0xF);
                result = (cnt != 0) ? 1 : 0;
            } while (result == 0);
        }
        data_020a0f50 += 1;
        break;

    case 6:
        data_020a0f70 = 0;
        data_020a0f1c &= 0x6FFF;
        MB_End();
        data_020a0f50 += 1;
        break;

    case 7:
        if (data_020a0f70 != 0) {
            data_020a0f50 += 1;
        }
        break;

    default:
        data_020a0f50 = 0;
        data_020a0f04 = 0;
        Memory::Deallocate(data_020a0f3c);
        Memory::Deallocate(data_020a0f7c);
        if (data_020a0f40 != 0) {
            u8 *p9d = data_020a0f9d;
            MBUserInfo *profile = &data_020a10fc[1];
            u16(*pbe)[3] = data_020a0fbe;
            int sl;
            data_020a0f40 = 0;
            data_020a0f08 = 0;
            sl = 1;
            do {
                *p9d = 0;
                MI_CpuClear8(profile, 0x16);
                MI_CpuClear8(pbe, 6);
                sl += 1;
                p9d += 1;
                profile++;
                pbe += 1;
            } while (sl < 4);
        }
        break;
    }

    /* The same copy as func_0203e0ac, without clearing 0x8000. Here the
       settings bytes (and unk22) go through two walking byte pointers: an
       indexed settings[i] loop colours the registers differently. */
    {
        u8 *src = (u8 *)&data_020a1040;
        u8 *dst = (u8 *)data_020a1154;
        int i;

        data_020a0f1c &= 0xF0FF;
        data_020a0f1c |= (data_020a0f28 << 8) & 0xF00;
        data_020a1154[0].flags = data_020a1154[1].flags = data_020a1154[2].flags =
            data_020a1154[3].flags = data_020a1040.flags;
        data_020a1154[0].keys = data_020a1154[1].keys = data_020a1154[2].keys =
            data_020a1154[3].keys = data_020a1040.keys;
        data_020a1154[0].touch = data_020a1154[1].touch = data_020a1154[2].touch =
            data_020a1154[3].touch = data_020a1040.touch;
        data_020a1154[0].inputYaw = data_020a1154[1].inputYaw = data_020a1154[2].inputYaw =
            data_020a1154[3].inputYaw = data_020a1040.inputYaw;
        for (i = 0; i < 0x11; i++) {
            dst[0x12] = dst[0x36] = dst[0x5a] = dst[0x7e] = src[0x12];
            src++;
            dst++;
        }
    }
}

/* The local-only link step (func_0203df40, mode 0): clear the record's 0x8000
   flag and copy this frame's input record (data_020a1040) into all four
   player records of data_020a1154. Each field is copied with one chained
   assignment, record 3 first: the cartridge reads every record back before
   it stores the next, which is how mwccarm evaluates a = b = c, and takes
   the touch samples of records 3, 2 and 1 from their own addresses (the
   pool's 0x020a11c4, 0x020a11a0 and 0x020a117c). The byte loop runs 0x11
   times, through settings and on into unk22. */
void func_0203e0ac(void) {
    int i;

    data_020a1040.flags &= 0x7fff;
    data_020a1154[0].flags = data_020a1154[1].flags = data_020a1154[2].flags =
        data_020a1154[3].flags = data_020a1040.flags;
    data_020a1154[0].keys = data_020a1154[1].keys = data_020a1154[2].keys = data_020a1154[3].keys =
        data_020a1040.keys;
    data_020a1154[0].touch = data_020a1154[1].touch = data_020a1154[2].touch =
        data_020a1154[3].touch = data_020a1040.touch;
    data_020a1154[0].inputYaw = data_020a1154[1].inputYaw = data_020a1154[2].inputYaw =
        data_020a1154[3].inputYaw = data_020a1040.inputYaw;
    for (i = 0; i < 0x11; i++) {
        data_020a1154[0].settings[i] = data_020a1154[1].settings[i] = data_020a1154[2].settings[i] =
            data_020a1154[3].settings[i] = data_020a1040.settings[i];
    }
}

/* Sample this frame's local input into data_020a1040, the buttons and the
   calibrated touch point, then run the link step for the current mode
   (data_020a0f04: 0 local only, func_0203e0ac; 1 and 2 func_0203ea5c; 3
   func_0203e20c) and count the frame. When the mode has just dropped back to
   0 from 1 or 2, it shuts the link down with func_02040724. */
void func_0203df40(void) {
    TPData sample;
    u8 sel;

    data_020a1040.keys = PAD_Read();

    func_0203b9bc(&sample);

    data_020a1040.touch.x = sample.x;
    data_020a1040.touch.y = sample.y;
    data_020a1040.touch.touch = sample.touch;
    data_020a1040.touch.validity = sample.validity;
    data_020a1040.flags |= 0x8000;

    sel = data_020a0f04;
    switch (sel) {
    case 1:
        func_0203ea5c();
        break;
    case 2:
        func_0203ea5c();
        break;
    case 3:
        func_0203e20c();
        break;
    default:
        data_020a0f04 = 0;
    case 0:
        func_0203e0ac();
        break;
    }

    data_020a1040.frame += 1;

    if (data_020a0f04 == 0) {
        switch (data_020a0f98) {
        default:
        case 0:
        case 3:
            break;
        case 1:
        case 2:
            data_020a0f1c &= 0x7fff;
            func_02040724();
            data_02099e18 = 1;
            data_020a0f10 = 0;
            break;
        }
    }

    data_020a0f98 = data_020a0f04;
}

/* The owner settings OS_GetOwnerInfo copies out of the firmware's user info at
   0x27ffc80: NitroSDK's OSOwnerInfo, which OS_GetOwnerInfo fills in (the
   language there is the one func_0201fec8 switches on). This is the older
   SDK's 0x50-byte layout: nickName and comment have no terminator slot. */
typedef struct {
    u8 language;        /* 0x00 */
    u8 favoriteColor;   /* 0x01 */
    u8 birthday[2];     /* 0x02 -- month, day */
    u16 nickName[10];   /* 0x04 */
    u16 nickNameLength; /* 0x18 */
    u16 comment[26];    /* 0x1a */
    u16 commentLength;  /* 0x4e */
} OwnerInfo;

/* Start the link (func_0201fec8 passes per-language data): keep the 0x40 and
   0x1c bytes it is given, clear the input records, profiles and link state.
   A download-play child (MB_IsMultiBootChild) takes mode 2 and runs the
   link step (func_0203df40) until each of four handshake waits is met, 0x4b0
   frames for the first and 0x258 for the rest; a wait that runs out flags
   data_020a0f1c bit 0, shuts the link down and parks the thread. Otherwise
   this console fills in its own profile from the owner settings. */
void func_0203db64(void *arg0, void *arg1) {
    OwnerInfo buf;

    data_020a0ef0 = 0;
    MI_CpuCopy8(arg0, &data_020a1064, sizeof(WMParentParam));
    MI_CpuCopy8(arg1, &data_020a0fec, sizeof(MBGameRegistry));
    MI_CpuClear8(&data_020a1040, 0x24);
    MI_CpuClear8(data_020a0fb8, 0x18);
    MI_CpuClear8(data_020a0fa0, 0x18);
    MI_CpuClear8(data_020a10fc, 0x58);
    MI_CpuClear8(data_020a10a4, 0x58);
    MI_CpuClear8(data_020a1154, 0x90);
    data_020a0f08 = 0;
    data_020a0f1c = 0;
    data_020a0f30 = 0;
    data_020a0f28 = 0;
    data_02099e18 = 1;
    data_020a0f10 = 0;
    data_020a0f04 = 0;
    if (MB_IsMultiBootChild()) {
        int count;

        data_020a0f04 = 2;
        count = 0x4b0;
        while (!(WFS_GetStatus() == 2 && data_02099e18 == data_020a0f08 && data_020a0f08 != 0) &&
               count != 0) {
            data_020a0f1c |= 0x2000;
            func_02042778();
            func_0203df40();
            count--;
        }
        if (count == 0) {
            data_020a0f1c |= 1;
            func_02040724();
            OS_SleepThread(0);
        }

        count = 0x258;
        data_020a1040.flags |= 1;
        while ((data_020a1040.flags & 1) && count != 0) {
            data_020a0f1c |= 0x2000;
            func_02042778();
            func_0203df40();
            count--;
        }
        if (count == 0) {
            data_020a0f1c |= 1;
            func_02040724();
            OS_SleepThread(0);
        }

        func_0203d854();
        count = 0x258;
        while (func_0203d81c() == 0 && count != 0) {
            data_020a0f1c |= 0x2000;
            func_02042778();
            func_0203df40();
            count--;
        }
        if (count == 0) {
            data_020a0f1c |= 1;
            func_02040724();
            OS_SleepThread(0);
        }

        func_0203d918();
        count = 0x258;
        while (func_0203d8fc() == 0 && count != 0) {
            data_020a0f1c |= 0x2000;
            func_02042778();
            func_0203df40();
            count--;
        }
        if (count == 0) {
            data_020a0f1c |= 1;
            func_02040724();
            OS_SleepThread(0);
        }
    } else {
        OS_GetOwnerInfo(&buf);
        MI_CpuClear8(&data_020a10fc[0], 0x16);
        data_020a10fc[0].favoriteColor = buf.favoriteColor;
        data_020a10fc[0].nameLength = buf.nickNameLength;
        data_020a10a4[0].favoriteColor = data_020a10fc[0].favoriteColor;
        data_020a10fc[0].playerNo = 0;
        data_020a10a4[0].playerNo = data_020a10fc[0].playerNo;
        data_020a10a4[0].nameLength = data_020a10fc[0].nameLength;
        MI_CpuCopy8(buf.nickName, data_020a10fc[0].name, buf.nickNameLength * 2);
        MI_CpuCopy8(buf.nickName, data_020a10a4[0].name, buf.nickNameLength * 2);
        data_020a0f00 = buf.language;
        OS_GetMacAddress(data_020a0fb8);
        OS_GetMacAddress(data_020a0fa0);
    }
    data_020a0ef0 = 1;
}

/* Byte p of player i's link settings (data_020a1154[i].settings).
   func_0203db3c writes the same bytes in this console's own record,
   data_020a1040. The accessors func_02020028..func_02020388 give the bits
   their roles (ready, go, course, slot mask, controller mode). */
unsigned char GetPlayerFlagByte(int i, int p) {
    return data_020a1154[i].settings[p];
}

/* Set link settings byte `byte` in this console's own input record
   data_020a1040: the writer the 0202 accessors pair with GetPlayerFlagByte. */
void func_0203db3c(int byte, int value) {
    data_020a1040.settings[byte] = value;
}

/* Player i's camera input yaw, as func_0203db0c stored it. The field is
   unsigned (both record copies read it back with ldrh); returning it as a
   short is what gives this read its ldrsh. */
short GetAngleToCamera(int i) {
    return data_020a1154[i].inputYaw;
}

/* Set player idx's input yaw (GetAngleToCamera reads it back). */
void func_0203db0c(int idx, unsigned short val) {
    data_020a1154[idx].inputYaw = val;
}

/* Store the camera's input yaw in the input record (data_020a1040.inputYaw);
   dCamera_c::Behavior passes mAngleY + mInputYawLag. */
void func_0203dafc(int v) {
    data_020a1040.inputYaw = v;
}

/* daPly_c idx's held buttons (func_0203bc7c and the DSMT scene read them). */
unsigned short func_0203dae4(int idx) {
    return data_020a1154[idx].keys;
}

/* The input record's frame counter (data_020a1040.frame, counted up by
   func_0203df40). The VS stage seeds from it. */
int func_0203dad4(void) {
    return data_020a1040.frame;
}

/* daPly_c idx's touch sample (x, y, touch, validity at +0x4); func_0203bb60
   and the DSMT scene read it. */
TPData *func_0203dabc(int idx) {
    return &data_020a1154[idx].touch;
}

/* How many players are present: func_0203ea5c counts the records whose
   flag 0x8000 is set; it is 1 for local play (func_0203df40) and while
   func_0203db64 starts the link. */
int func_0203daac(void) {
    return data_02099e18;
}

/* This console's player index on the link, data_020a0f10 (0 for the
   parent): the AID func_0203ea5c takes from func_02040704. */
int func_0203da9c(void) {
    return data_020a0f10;
}

/* The link status bits of data_020a0f1c that mask x selects.
   func_02030e4c and func_020197b8 test the error bits 0x9d with it. */
unsigned short func_0203da80(unsigned short x) {
    return data_020a0f1c & x;
}

/* Clear the link error bits 0x9f of data_020a0f1c and unk22 (byte 0x22) of
   the local input record data_020a1040. func_02030aa4 calls it when a session
   starts, and dMeter_c when it sets up. func_0203ea5c runs the same three
   statements inline in its error countdown, so the two shared a translation
   unit. The cartridge stores unk22 twice here as well. */
void func_0203da4c(void) {
    data_020a0f1c &= 0xff60;
    data_020a1040.unk22 = 0;
    data_020a1040.unk22 = 0;
}

/* The link mode data_020a0f04 that func_0203da2c sets (0 local only, 1 and 2
   func_0203ea5c, 3 func_0203e20c). */
int func_0203da3c(void) {
    return data_020a0f04;
}

/* Set the link mode data_020a0f04 (func_0203df40 runs the link step for it:
   0 local only, 1 and 2 func_0203ea5c, 3 func_0203e20c). */
void func_0203da2c(int v) {
    data_020a0f04 = v;
}

/* While link bit 0x8000 of data_020a0f1c is set (func_0203e20c sets it
   with 0x1000), WM_GetLinkLevel's value, else 0. func_0203083c indexes its
   link icon table data_020927a0 with it. */
int func_0203d9f4(void) {
    if ((data_020a0f1c & 0x8000) == 0)
        return 0;
    return WM_GetLinkLevel();
}

/* This console's player index on the link (see func_0203d974.cpp). */

/* Nonzero only on the host: not a download-play child
   (MB_IsMultiBootChild) and player 0. The inverse of func_0203d974. */
int func_0203d9b4(void) {
    if (MB_IsMultiBootChild())
        return 0;
    return data_020a0f10 == 0;
}

/* data_020a0f10 is this console's player index on the link: the AID that
   func_0203ea5c takes from func_02040704, 0 for the parent. */

/* Nonzero unless this console is the host: a download-play child
   (MB_IsMultiBootChild) or any player but 0.
   func_0203d9b4 is the same test the other way round. */
int func_0203d974(void) {
    if (MB_IsMultiBootChild())
        return 1;
    return data_020a0f10 != 0;
}

/* Whether player i's record has flag 0x8000 (func_0203df40 sets it in each
 * frame's local record, data_020a1040). */
int func_0203d950(int i) {
    return (data_020a1154[i].flags & 0x8000) != 0;
}

/* Drop the load request, flag 0x4000 of this console's input record
   (func_0203d918), once the course resources are in (dScEntry_c, dScMB_c). */
void func_0203d930(void) {
    data_020a1040.flags &= 0xbfff;
}

/* Raise flag 0x4000 of this console's input record, the load request:
   once every present child has it, player 0 sets 0x2000 in its own record
   (func_0203ea5c), which func_0203d8fc reads. dScEntry_c::EnterMatchLoading
   and func_0203db64 raise it; func_0203d930 drops it. */
void func_0203d918(void) {
    data_020a1040.flags |= 0x4000;
}

/* Flag 0x2000 of player 0's input record (data_020a1154[0].flags): the load
   signal player 0 raises once every present child has posted its load
   request (func_0203d918), and drops when none has. */
int func_0203d8fc(void) {
    return (data_020a1154[0].flags & 0x2000) != 0;
}

/* Raise data_020a0f70, the flag the mode-3 link step (func_0203e20c)
   waits on in steps 3 and 7; func_0203e9a8 raises it on parent state 12.
   dScEntry_c reaches it through the veneer func_02030a6c. */
void func_0203d8e8(void) {
    data_020a0f70 = 1;
}

/* Raise data_020a0f40, which tells the mode-3 link step (func_0203e20c)
   to wind down. func_02030a38 calls it. */
void func_0203d8d4(void) {
    data_020a0f40 = 1;
}

/* daPly_c idx's 0x16-byte profile: from data_020a10fc in link mode 3,
   else from data_020a10a4 (four entries each; see func_0203db64). */
MBUserInfo *func_0203d8a0(int idx) {
    if (data_020a0f04 == 3)
        return &data_020a10fc[idx];
    return &data_020a10a4[idx];
}

/* data_020a0f08, the player count the parent posts: a child copies it from
   byte 0xd of the parent's packet (func_0203ea5c), and func_0203db64 waits
   until the counted players (func_0203daac) reach it. */
int func_0203d890(void) {
    return data_020a0f08;
}

int GetOwnerLanguage(void) {
    return data_020a0f00;
}

/* Raise the link handshake flag, 0x1000 of this console's input record
   (func_020308b4; func_0203d81c polls it). */
void func_0203d868(void) {
    data_020a1040.flags |= 4096;
}

/* Clear the link handshake's passed latch (func_020308b4). */
void func_0203d854(void) {
    data_020a0f30 = 0;
}

/* The link handshake's poll (func_020308a8): keep flag 0x1000 raised in this
   console's input record (data_020a1040.flags) and return 1, clearing the
   latch, once func_0203ea5c has set data_020a0f30 -- which it does when every
   present player's record carries 0x1000. */
int func_0203d81c(void) {
    data_020a1040.flags |= 0x1000;
    if (data_020a0f30 != 0) {
        data_020a0f30 = 0;
        return 1;
    }
    return 0;
}

/* Check a file operation's result: nonzero passes (1). Zero sets link
   error bit 0x80 in data_020a0f1c and returns whether this is a
   download-play child (MB_IsMultiBootChild), which cannot read the
   card again; func_02018e68 then sleeps the thread (see myFS_ReadFile.c). */
int func_0203d7d4(int arg) {
    if (arg != 0)
        return 1;
    data_020a0f1c |= 0x80;
    return MB_IsMultiBootChild() != 0;
}

/* Whether a file read has failed (link error bit 0x80 of data_020a0f1c,
   set by func_0203d7d4). LoadOverlay, UnloadOverlay and func_02017e94
   then leave the overlay's RAM alone, and dScMb_c waits on it. */
int func_0203d7b8(void) {
    return (data_020a0f1c & 128) != 0;
}

/* extern "C" */
