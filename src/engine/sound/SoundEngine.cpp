//cpp
/* SoundEngine, the sequencer half of the sound module.
 * arm9 .text 0x0204eda4..0x020524e4: NitroSystem's sound library. Among
 * it are snd_main.c's NNS_SndInit and NNS_SndMain, all of
 * snd_player.c (0x0204f214..0x0204fce8, the 32-record player table
 * sPlayer), snd_arc.c (0x02050970..0x02050fb0, the current archive
 * sCurrent) and snd_heap.c (0x02050fb0..0x020513a8). Each named body
 * matches pret/pokediamond's NNS_SND_player.s, NNS_SND_sndarc.s and
 * NNS_SND_heap.s and the Sonic Rush Adventure decomp's sources by field
 * offsets and callees; the bodies run in reverse source order. The
 * resource manager (0x0204f0b8..0x0204f214), the capture functions
 * (0x020501b8..0x02050970), NNSi_SndSeqArcGetSeqInfo (0x020523e0) and
 * the fader (0x02052420..0x020524e4) carry pret's names from
 * NNS_SND_resource_mgr.s, NNS_SND_capture.s, NNS_SND_seqdata.s and
 * NNS_SND_fader.s, matched body for body. NNSi_SndInitResourceMgr comes
 * last of its file here where pret's comes first, and the capture file
 * has a thread procedure and starter (func_02050038, func_020506fc) that
 * pret's revision does not. The public Sound:: calls in
 * the earlier sound TU land in this range. No RTTI names a class for
 * these workers.
 *
 * leftovers:
 * - The workers outside those files stay func_0204* and func_0205*.
 *   There is no class to hang them on.
 * - func_02050d54 sits where snd_arc.c's NNS_SndArcSetup does, but this
 *   2004 build reads the header in NNS_SndArcInit and tests the heap in
 *   the caller, so the name of this older function is not evidenced.
 * - Sound::Player::SetPlayableSeqCount and the two Sound::Info*::GetWithID
 *   members are snd_player.c's NNS_SndPlayerSetPlayableSeqCount and
 *   snd_arc.c's NNS_SndArcGetSeqInfo/NNS_SndArcGetBankInfo. Moving them
 *   off include/Sound.h renumbers @NNN in its consumers. The player-side views name the
 *   nnsys/snd types (NNSSndSeqPlayer, NNSSndHandle, NNSSndFader); the
 *   sound heap is the local struct SndHeap (Sound.cpp holds one opaque);
 *   HeapAllocator_205212c keeps its own local layout.
 * - OS_DisableInterrupts and OS_RestoreInterrupts stay namespace-scope externs so
 *   they mangle as the IRQ TU defines them.
 * - Channel and heap records stay address arithmetic. A guessed
 *   sequencer member is not what these bodies share.
 */
#include "types.h"
#include "nitro/fs/file.h"
#include "nitro/fs/archive.h"
#include "Sound.h"
#include "SoundPlayerRecord.h"
#include "nnsys/fnd.h"
#include "Heap.h"
#include "msl/string.h"
#include "nitro/os/cache.h"
#include "nnsys/snd/player.h"
#include "nitro/os/message.h"
#include "nitro/os/thread.h"
#include "nitro/snd/alarm.h"
#include "nitro/os/system.h"
#include "nitro/mi/memory.h"

struct Elem_204f278;
struct HeapAllocator_205212c;
struct Obj_205212c;

typedef short s16;
/* A block NNS_SndHeapAlloc carves from a sound heap: its link in the heap's
   newest level, then what freeing the level calls back with. The caller's
   memory starts at 0x20. */
typedef void (*SndHeapDisposeCallback)(void *mem, u32 size, u32 data1, u32 data2);
struct SndHeapBlock {
    NNSFndLink link;                 /* 0x00 */
    u32 size;                        /* 0x08 */
    SndHeapDisposeCallback callback; /* 0x0c */
    u32 data1;                       /* 0x10 */
    u32 data2;                       /* 0x14 */
    u8 pad18[8];
    u8 mem[1]; /* 0x20 */
};
/* A sound heap (NNS_SndHeapCreate): a frame heap and the stack of levels
   NewSection pushes. A level is a 0x14-byte list of the blocks carved
   since it was pushed, linked into `levels` at 0xc; NNS_SndHeapLoadState pops
   levels back to a count and NNS_SndHeapClear pops them all, calling each
   block's dispose callback, newest first. */
struct SndHeap {
    NNSFndHeapHandle frmHeap; /* 0x0 */
    NNSFndList levels;        /* 0x4 */
};
typedef int s32;
typedef void (*FP)(void *, int, int *, int, int, int);
typedef signed char s8;
/* The Nitro binary file header every NARC, SDAT, SSEQ and SSAR file opens
   with (func_0204ef9c checks a NARC's), and the header each of its blocks
   starts with (func_0204ee40 walks a NARC's by kind and size). */
struct NitroFileHeader {
    u32 magic;      /* 0x00 */
    u16 byteOrder;  /* 0x04 */
    u16 version;    /* 0x06 */
    u32 fileSize;   /* 0x08 */
    u16 headerSize; /* 0x0c */
    u16 numBlocks;  /* 0x0e */
};
struct NitroBlockHeader {
    u32 kind; /* 0x0 */
    u32 size; /* 0x4 */
};
struct NarcFileSpan {
    int start;
    int end;
};
struct NarcFatBlock {
    NitroBlockHeader header;
    u16 fileCount;
    u16 reserved;
    NarcFileSpan files[1];
};
/* One sequence of a sequence archive: its offset in the archive's data
   (-1 for none), then the same parameters a Sound::InfoSequenceEntry
   carries after its fileId. */
struct Elem {
    int offset;         /* 0x0 */
    u16 bankId;         /* 0x4: Sound::InfoInstrumentBankEntry id */
    u8 volume;          /* 0x6: the player's initial volume */
    u8 channelPriority; /* 0x7 */
    u8 playerPriority;  /* 0x8: NNSi_SndPlayerAllocSeqPlayer's priority */
    u8 playerNumber;    /* 0x9: sPlayer record */
};
/* A loaded sequence file: past the file and DATA block headers, the offset
   of its sequence data from the file start. */
struct P5 {
    NitroFileHeader fileHeader;   /* 0x00 */
    NitroBlockHeader blockHeader; /* 0x10 */
    int baseOffset;               /* 0x18 */
};
/* A loaded sequence archive: a sequence file whose DATA block goes on with
   the count of its sequences and their table (NNSi_SndSeqArcGetSeqInfo). */
struct ElemTable {
    NitroFileHeader fileHeader;   /* 0x00 */
    NitroBlockHeader blockHeader; /* 0x10 */
    int baseOffset;               /* 0x18 */
    int count;                    /* 0x1c */
    struct Elem arr[1];           /* 0x20 */
};
struct Entry {
    unsigned char kind;
    unsigned char b1;
    unsigned char b2;
    unsigned char b3;
    int w;
};
struct List {
    unsigned int count;
    struct Entry entries[1];
};
/* The sound archive sCurrent points at, opened by NNS_SndArcInit.
   0x00..0x30 is the archive file's own header, read from offset 0; its
   (offset, size) pairs locate the blocks func_02050d54 loads. */
struct data_020a5bb8_t {
    /* 0x00 */ NitroFileHeader fileHeader;
    /* 0x10 */ int symbolOffset; /* the SYMB block, loaded only on request */
    /* 0x14 */ int symbolSize;
    /* 0x18 */ int infoOffset;
    /* 0x1c */ int infoSize;
    /* 0x20 */ int fileTableOffset;
    /* 0x24 */ int fileTableSize;
    /* 0x28 */ int fileImageOffset; /* the FILE block; files are read by fileTable offset */
    /* 0x2c */ int fileImageSize;
    /* 0x30 */ FSFile file;
    /* 0x74 */ FSFileID fileId;         /* filled from the path by FS_ConvertPathToFileID */
    /* 0x7c */ struct Hdr84
        *fileTable; /* per-file records (NNS_SndArcGetFileAddress, NNS_SndArcReadFile) */
    /* 0x80 */ void *symbols;           /* loaded only when func_02050d54's flag is set */
    /* 0x84 */ void *info;              /* the Info*Entry::GetWithID tables */
};
/* The INFO block data_020a5bb8_t::info points at. After its 8-byte block
   header come the offsets, from the block start, of its record tables; each
   lookup below reads one, bounds-checks the id against the table's count and
   returns the record at the id's offset (0 for none). */
/* A count, then one file offset per id. Offset 0 means the id is absent. */
struct SoundOffsetTable {
    unsigned count;
    int offset[1];
};
struct SndInfoBlock {
    NitroBlockHeader header;
    int seqTable;     /* 0x08: Sound::InfoSequenceEntry::GetWithID */
    int seqArcTable;  /* 0x0c: NNS_SndArcGetSeqArcInfo */
    int bankTable;    /* 0x10: Sound::InfoInstrumentBankEntry::GetWithID */
    int waveArcTable; /* 0x14: NNS_SndArcGetWaveArcInfo; a bank's four ids index it */
    int playerTable;  /* 0x18: NNS_SndArcGetPlayerInfo; NNS_SndArcPlayerSetup reads each record's
                         playable-seq count from its first byte */
    int groupTable;   /* 0x1c: NNS_SndArcGetGroupInfo */
};
/* The sequence-archive and wave-archive records (NNS_SndArcGetSeqArcInfo,
   NNS_SndArcGetWaveArcInfo): the file id of the archive. */
struct SndArchiveInfo {
    u32 fileId;
};
/* A player record (NNS_SndArcGetPlayerInfo): how many sequences the player plays at
   once, and the heap NNS_SndPlayerCreateHeap gives each of them. */
struct SndPlayerInfo {
    u8 seqMax;
    u8 pad1[3];
    u32 heapSize;
};
/* One file record: NNS_SndArcReadFile reads `size` bytes from `offset` into the
   SDAT; NNSi_SndArcLoadFile sizes its buffer from NNS_SndArcGetFileSize's `size`, and
   NNS_SndArcSetFileAddress caches the loaded copy that NNS_SndArcGetFileAddress hands back. */
struct FatEntry {
    int offset;    /* 0x0 */
    int size;      /* 0x4 */
    void *address; /* 0x8 */
    int pad_c;
};
struct Hdr84 {
    NitroBlockHeader header;
    int count;
    struct FatEntry entries[1]; /* 0xc, count of them */
};
enum Fmt {
    FMT_A = 0,
    FMT_B = 1
};
/* The capture stream at data_020a5634, started by NNSi_SndCaptureStart and
   stopped by NNSi_SndCaptureStop. Its alarm, AlarmCallback, hands each finished
   block to f30 (directly, or through the sound thread func_02050038). */
struct T {
    /* 0x00 */ int f0; /* nonzero while running */
    /* 0x04 */ int f4; /* the start mode; 1 sets the output selector */
    /* 0x08 */ int f8; /* passed through to f30 */
    /* 0x0c */ int fc; /* the two buffers, f14 bytes each */
    /* 0x10 */ int f10;
    /* 0x14 */ int f14;
    /* 0x18 */ int f18; /* current block, wraps at f2c */
    /* 0x1c */ int f1c; /* channels locked by NNS_SndLockChannel */
    /* 0x20 */ int f20; /* SND_StartTimer / SND_StopTimer channel mask */
    /* 0x24 */ int f24; /* capture mask */
    /* 0x28 */ int f28; /* alarm number from NNS_SndAllocAlarm, or -1 */
    /* 0x2c */ int f2c; /* block count */
    /* 0x30 */ void (*f30)(int, int, int, int, int);
    /* 0x34 */ int f34;          /* passed through to f30 */
    /* 0x38 */ NNSSndFader fade; /* channel volume ramp, NNSi_SndCaptureMain */
    /* 0x48 */ int f48;          /* stop once fade finishes */
    /* 0x4c */ int f4c;          /* last volume sent with SND_SetChannelVolume */
};
/* One message the alarm posts to data_020a5600 (the 8-slot ring
   data_020a5718): the stream, the block size, its offset, and the block
   address in each buffer. */
struct Q {
    struct T *f0;
    int f4, f8, fc, f10;
};
struct C {
    /* 0x00 */ char pad0[8];
    /* 0x08 */ int unk8;    /* passed through to callback */
    /* 0x0c */ int fc;      /* bit 0: on data_020a552c, bit 1: timer started */
    /* 0x10 */ int bufSize; /* split into blockCount blocks */
    /* 0x14 */ int blockCount;
    /* 0x18 */ FP callback; /* called by func_0204fce8 once per block */
    /* 0x1c */ int unk1c;   /* passed through to callback */
    /* 0x20 */ int f20;     /* current block, wraps at blockCount */
    /* 0x24 */ int volume;  /* func_0204fe5c: plus each channel's data_020a5578 entry */
    /* 0x28 */ int f28;     /* alarm number from NNS_SndAllocAlarm */
    /* 0x2c */ int f2c;     /* the channels' mask: SND_StartTimer / SND_StopTimer,
                               unlocked by func_02050008 */
    /* 0x30 */ int chanCount;
    /* chanCount channel numbers; room for all 16, as the stream that embeds
       this goes on at 0x44 */
    /* 0x34 */ unsigned char chans[16];
    void func_0204fce8(void *);
    void func_0204ffcc();
};
struct E
{
  int field0;
  int field4;
};
/* The 16 sequence players at sSeqPlayer: nnsys/snd/player.h's
   NNSSndSeqPlayer (fader at 0x1c, status at 0x2c, playerNo at 0x3c). The
   free list sFreeList and the priority list sPrioList link them
   through prioLink, their sPlayer record's first list through
   playerLink. */
/* The sound archive embeds the shared NitroSDK FSArchive before its NARC view. */
struct S82 {
    /* 0x00 */ FSArchive arc;
    /* 0x50 */ char *narc;         /* the file func_0204ef9c checked for 'NARC' */
    /* 0x54 */ NarcFatBlock *f54;  /* its FATB block */
    /* 0x58 */ unsigned char *f58; /* its FIMG data */
};
struct HeapAllocator_205212c {
    char pad0[8];
    void *owner_8;    
};
/* The 4 streams at data_020a5bd4 (stride 0x12c), each a C (0x00..0x44)
   followed by its own state. */
struct Obj_205212c : C {
    /* 0x044 */ FSFile file;
    /* 0x088 */ char pad1[0xc8 - 0x88];
    /* volume ramp func_020522c4 steps; its value >> 8 indexes data_02086384 */
    /* 0x0c8 */ NNSSndFader fade;
    /* 0x0d8 */ char pad1b[0xf0 - 0xd8];
    /* 0x0f0 */ u32 uf0;
    /* func_020522c4 reads the next two only while uf0 bit 0 is set: a nonzero
       stopPending stops the stream at once (func_02052208), and a nonzero
       startReady lets a pending start (bit 2) go through func_0204ffcc, after
       which func_020522c4 swaps bit 2 for bit 1. Nothing in this file writes either (medium
       confidence). */
    /* 0x0f4 */ int stopPending;
    /* 0x0f8 */ int startReady;
    /* 0x0fc */ char pad2[0x100 - 0xfc];
    /* 0x100 */ int useCount; /* func_020520dc releases on the last drop */
    /* 0x104 */ char pad3[0x114 - 0x104];
    /* 0x114 */ int *u114;
    /* 0x118 */ char pad4[0x11c - 0x118];
    /* 0x11c */ int volumeIndex; /* indexes data_02086384; func_020522c4 adds the fade term */
    /* 0x120 */ int lastVolume;  /* last value func_020522c4 sent with func_0204fe5c */
    /* 0x124 */ char pad5[0x12c - 0x124];
};
/* A player heap: NNS_SndPlayerCreateHeap carves one per playable sequence and parks it
   on its sPlayer record's second list; NNSi_SndPlayerAllocHeap hands it to a
   sequence player, whose heap word points back here. */
struct Elem_204f278 {
    /* 0x00 */ NNSFndLink link;         /* the record's second list */
    /* 0x08 */ struct SndHeap *heap;    /* NNS_SndHeapCreate's heap, freed by NNS_SndHeapDestroy */
    /* 0x0c */ NNSSndSeqPlayer *player; /* claimant; its heap word points back here */
    /* 0x10 */ int playerIndex;         /* sPlayer slot whose list holds this */
};

extern "C" {
/* C-linkage callback types used by foreign decls. */
/* Foreign callees and data. */

extern int __aeabi_idiv(int a, int b);
extern const s16 data_02086384[];
extern int data_020a4d44;
extern unsigned int sCaptureLock;
extern unsigned int sAlarmLock;
extern unsigned int sChannelLock;
extern NNSFndList sFreeList;
extern NNSFndList sPrioList;
extern SoundPlayerRecord sPlayer[];
extern NNSSndSeqPlayer sSeqPlayer[];
extern NNSFndList data_020a552c;
extern int data_020a5538[];
extern struct E data_020a5578[];
extern int data_020a55f8;
extern int data_020a55fc;
extern OSMessageQueue data_020a5600;
extern void *data_020a5614[]; /* the message array of data_020a5600 */
extern struct T data_020a5634;
extern OSThread data_020a5684;
extern struct Q data_020a5718[];
extern data_020a5bb8_t *sCurrent;
extern NNSFndList sFreeCommandList;
extern NNSFndList data_020a5bc8;
extern struct Obj_205212c data_020a5bd4[];
extern void SND_Init(void);
extern void SND_SetOutputSelector(s32 left, s32 right, s32 channel1, s32 channel3);
extern void SND_InvalidateWaveData(const void *start, const void *end);
extern void SND_InvalidateBankData(const void *start, const void *end);
extern void SND_InvalidateSeqData(const void *start, const void *end);
extern void SND_SetupChannelPcm(s32 chn, s32 waveFormat, const void *dataAddr, s32 loopMode,
                                s32 loopStart, s32 dataLen, s32 volume, s32 chnDataShift, s32 timer,
                                s32 pan);
extern void SND_SetChannelVolume(u32 chnBitMask, s32 volume, s32 chnDataShift);
extern void SND_UnlockChannel(u32 chnBitMask, u32 flags);
extern void SND_LockChannel(u32 chnBitMask, u32 flags);
extern void SND_SetTrackMute(s32 player, u32 trackBitMask, int flag);
extern void SND_SetupAlarm(s32 alarm, u32 tick, u32 period, SNDAlarmCallback cb, void *userData);
extern void SND_SetupCapture(s32 capture, s32 format, void *bufferPtr, u32 length, int loopFlag,
                             s32 in, s32 out);
extern void SND_StopTimer(u32 chnBitMask, u32 capBitMask, u32 alarmBitMask, u32 flags);
extern void SND_StartTimer(u32 chnBitMask, u32 capBitMask, u32 alarmBitMask, u32 flags);
extern void SND_SetTrackModSpeed(s32 player, u32 trackBitMask, s32 speed);
extern void SND_SetTrackModDepth(s32 player, u32 trackBitMask, s32 depth);
extern void SND_SetTrackPan(s32 player, u32 trackBitMask, s32 pan);
extern void SND_SetTrackPitch(s32 player, u32 trackBitMask, s32 pitch);
extern void SND_SetTrackVolume(s32 player, u32 trackBitMask, s32 volume);
extern void SND_SetPlayerChannelPriority(s32 player, s32 prio);
extern void SND_SetPlayerVolume(s32 player, s32 volume);
extern void SND_SetPlayerTempoRatio(s32 player, s32 ratio);
extern void SND_PauseSeq(s32 player, int flag);
extern void SND_StopSeq(s32 player);
extern void SND_StartSeq(s32 player, const void *seqBasePtr, u32 seqOffset,
                         struct SNDBankData *bankData);
extern int SND_IsFinishedCommandTag(u32 tag);
extern u32 SND_GetCurrentCommandTag(void);
extern void SND_WaitForCommandProc(u32 tag);
extern int SND_FlushCommand(u32 flags);
extern const struct SNDCommand *SND_RecvCommandReply(u32 flags);
extern u32 SND_GetPlayerStatus(void);
extern u16 SND_CalcChannelVolume(s32 x);
struct SNDWaveArc;
extern void SND_DestroyWaveArc(struct SNDWaveArc *waveArc);
extern void SND_DestroyBank(struct SNDBankData *bank);
extern void SND_AssignWaveArc(struct SNDBankData *bank, int index, struct SNDWaveArc *waveArc);

/* In-span forward declarations. */
extern int func_0204eda4(int r0, char* a, u32 idx);
extern int func_0204ee10(struct S82 *self);
extern int func_0204ede8(struct S82 *o, unsigned int i);
extern int func_0204ee40(struct S82 *self, char *name, char *p);
extern int func_0204ef9c(void* p);
extern void func_0204efe0(void);
extern void BeginSleep(void);
extern void NNS_SndMain(void);
extern void NNS_SndInit(void);
extern int NNS_SndFreeAlarm(unsigned int bit);
extern int NNS_SndAllocAlarm(void);
extern void NNS_SndUnlockCapture(int mask);
extern int NNS_SndLockCapture(unsigned int mask);
extern void NNS_SndUnlockChannel(unsigned int mask);
extern int NNS_SndLockChannel(unsigned int a);
extern void NNSi_SndInitResourceMgr(void);
extern void SetPlayerPriority(NNSSndSeqPlayer *player, int prio);
extern void PlayerHeapDisposeCallback(void *mem, u32 size, u32 data1, u32 data2);
extern void ShutdownPlayer(NNSSndSeqPlayer *obj);
extern NNSSndSeqPlayer *AllocSeqPlayer(int prio);
extern void ForceStopSeq(NNSSndSeqPlayer *player);
extern void InsertPrioList(NNSSndSeqPlayer *self);
extern void InsertPlayerList(NNSFndList *list, NNSSndSeqPlayer *player);
extern void InitPlayer(NNSSndSeqPlayer *player);
extern struct SndHeap *NNSi_SndPlayerAllocHeap(int playerNo, NNSSndSeqPlayer *player);
extern void NNSi_SndPlayerPause(NNSSndSeqPlayer *player, int flag);
extern void NNSi_SndPlayerStopSeq(NNSSndSeqPlayer *player, int fadeFrames);
extern int NNSi_SndPlayerStartSeq(NNSSndSeqPlayer *player, const void *seqData, u32 seqOffset,
                                  struct SNDBankData *bank);
extern void NNSi_SndPlayerFreeSeqPlayer(NNSSndSeqPlayer *player);
extern NNSSndSeqPlayer *NNSi_SndPlayerAllocSeqPlayer(NNSSndHandle *handle, int playerIdx, int prio);
extern void NNS_SndPlayerSetSeqArcNo(NNSSndHandle *handle, int seqArcNo, int index);
extern void NNS_SndPlayerSetSeqNo(NNSSndHandle *handle, int seqNo);
extern void NNS_SndHandleReleaseSeq(NNSSndHandle *handle);
extern void NNS_SndHandleInit(NNSSndHandle *handle);
extern void NNS_SndPlayerPauseByPlayerNo(int idx, int val);
extern void NNS_SndPlayerStopSeqByPlayerNo(int idx, int arg1);
extern int NNS_SndPlayerStopSeq(NNSSndHandle *handle, int fade);
extern int NNS_SndPlayerCreateHeap(int playerNo, struct SndHeap *heap, u32 size);
extern void NNSi_SndPlayerMain(void);
extern void NNSi_SndPlayerInit(void);
extern void func_0204fda4(struct C *c);
extern void func_0204fde8(struct C *obj);
extern void func_0204fe5c(struct C *c, int volume);
extern void func_0204fed0(struct C *c);
extern void func_0204ff48(struct C *p);
extern void func_0204ff9c(struct C *o);
extern void func_02050008(struct C *o);
extern void func_02050038(void *arg);
extern void AlarmCallback(void *arg);
extern void NNSi_SndCaptureEndSleep(void);
extern void NNSi_SndCaptureBeginSleep(void);
extern void NNSi_SndCaptureStop(void);
extern int NNSi_SndCaptureStart(int a, int b, int c, int d, int p5, int p6, int p7, int p8, int p9,
                                int p10, int p11, int p12, int p13, int p14, int p15);
extern void func_020506fc(int arg);
extern void NNS_SndCaptureStopEffect(void);
extern int NNS_SndCaptureStartEffect(void *a0, unsigned int a1, int a2, int a3, int a4, void *a5,
                                     void *a6);
extern void NNSi_SndCaptureMain(void);
extern void NNSi_SndCaptureInit(void);
extern void SymbolDisposeCallback(void *mem, u32 size, u32 arc, u32 data2);
extern void FatDisposeCallback(void *mem, u32 size, u32 arc, u32 data2);
extern void InfoDisposeCallback(void *mem, u32 size, u32 arc, u32 data2);
extern void NNS_SndArcSetFileAddress(u32 fileId, void *address);
extern void *NNS_SndArcGetFileAddress(u32 fileId);
extern int NNS_SndArcReadFile(u32 fileId, void *buffer, u32 size, u32 offset);
extern int NNS_SndArcGetFileSize(unsigned int i);
extern struct List *NNS_SndArcGetGroupInfo(int id);
extern struct SndPlayerInfo *NNS_SndArcGetPlayerInfo(int id);
extern struct SndArchiveInfo *NNS_SndArcGetWaveArcInfo(int id);
extern struct SndArchiveInfo *NNS_SndArcGetSeqArcInfo(int id);
extern void *NNS_SndArcGetSeqArcParam(int a, int idx);
extern int NNS_SndArcGetCurrent(void);
extern int NNS_SndArcSetCurrent(int v);
extern int func_02050d54(data_020a5bb8_t *arc, struct SndHeap *heap, int loadSymbols);
extern void NNS_SndArcInit(data_020a5bb8_t *arc, const char *path, struct SndHeap *heap,
                           int loadSymbols);
extern void EraseSync(void);
extern s32 NewSection(struct SndHeap *heap);
extern s32 InitHeap(struct SndHeap *heap, NNSFndHeapHandle frmHeap);
extern void InitHeapSection(NNSFndList *level);
extern u32 NNS_SndHeapGetFreeSize(struct SndHeap *heap);
extern void NNS_SndHeapLoadState(struct SndHeap *heap, int level);
extern s32 NNS_SndHeapSaveState(struct SndHeap *heap);
extern void *NNS_SndHeapAlloc(struct SndHeap *heap, u32 size, SndHeapDisposeCallback callback,
                              u32 data1, u32 data2);
extern void NNS_SndHeapClear(struct SndHeap *heap);
extern void NNS_SndHeapDestroy(struct SndHeap *heap);
extern struct SndHeap *NNS_SndHeapCreate(u32 addr, u32 size);
extern void WaveArcDisposeCallback(void *mem, u32 size, u32 arc, u32 fileId);
extern void BankDisposeCallback(void *mem, u32 size, u32 arc, u32 fileId);
extern void SeqDisposeCallback(void *mem, u32 size, u32 arc, u32 fileId);
extern void DisposeCallback(void *mem, int arc, u32 fileId);
extern void *LoadWaveArc(u32 fileId, struct SndHeap *heap);
extern void *LoadBank(u32 fileId, struct SndHeap *heap);
extern void *LoadSeqArc(u32 fileId, struct SndHeap *heap);
extern void *LoadSeq(u32 fileId, struct SndHeap *heap);
extern void *NNSi_SndArcLoadFile(u32 fileId, SndHeapDisposeCallback callback, u32 data1, u32 data2,
                                 struct SndHeap *heap);
extern int func_020516d4(unsigned int id, int flags, struct SndHeap *heap);
extern int func_0205179c(unsigned int id, int flags, struct SndHeap *heap);
extern int func_020518a0(u32 id, struct SndHeap *heap);
extern int func_020518dc(u32 id, struct SndHeap *heap);
extern int func_02051918(unsigned int id, struct SndHeap *heap);
extern struct SndHeap *func_02051a28(int playerNo, NNSSndSeqPlayer *player);
extern void func_02051a48(void *mem, u32 size, u32 data1, u32 data2);
extern void func_02051a68(void *mem, u32 size, u32 data1, u32 data2);
extern void func_02051a88(void *mem, u32 size, u32 data1, u32 data2);
extern int func_02051a98(NNSSndHandle *handle, int playerNo, unsigned int bankNo, int playerPrio,
                         struct Elem *seq, struct ElemTable *arc, int seqArcNo, int index);
extern int func_02051bd0(NNSSndHandle *handle, int playerNo, unsigned int bankNo, int playerPrio,
                         Sound::InfoSequenceEntry *info, int seqNo);
extern int func_02051e60(NNSSndHandle *handle, int b, int c, int d, int arg5, int arg6);
extern int NNS_SndArcPlayerStartSeqArc(void *a0, u32 a1, int a2);
extern int NNS_SndArcPlayerStartSeq(void *thiz, unsigned int id);
extern int NNS_SndArcPlayerSetup(struct SndHeap *heap);
extern void FreeCommandBuffer(void *node);
extern void func_020520dc(Obj_205212c *self);
extern void func_0205212c(Obj_205212c *self);
extern void func_02052208(Obj_205212c *stream);
extern void func_02052234(void);
extern void func_0205227c(void);
extern void func_020522c4(void);
extern struct Elem *NNSi_SndSeqArcGetSeqInfo(struct ElemTable *obj, int idx);
extern int NNSi_SndFaderIsFinished(NNSSndFader *fader);
extern void NNSi_SndFaderUpdate(NNSSndFader *fader);
extern int NNSi_SndFaderGet(NNSSndFader *fader);
extern void NNSi_SndFaderSet(NNSSndFader *o, int a, int b);
extern void NNSi_SndFaderInit(NNSSndFader *fader);
}

extern "C" void NNSi_SndFaderInit(NNSSndFader *fader) {
    fader->target = 0;
    fader->origin = fader->target;
    fader->frame = 0;
    fader->counter = fader->frame;
}

extern "C" void NNSi_SndFaderSet(NNSSndFader *o, int a, int b) {
    o->origin = NNSi_SndFaderGet(o);
    o->target = a;
    o->frame = b;
    o->counter = 0;
}

extern "C" int NNSi_SndFaderGet(NNSSndFader *fader) {
    int den = fader->frame;
    int num = fader->counter;
    if (num >= den)
        return fader->target;
    {
        int lo = fader->origin;
        int d = fader->target - lo;
        return lo + num * d / den;
    }
}

extern "C" void NNSi_SndFaderUpdate(NNSSndFader *fader) {
    if (fader->counter < fader->frame) {
        fader->counter += 1;
    }
}

extern "C" int NNSi_SndFaderIsFinished(NNSSndFader *fader) {
    return fader->counter >= fader->frame;
}

extern "C" struct Elem *NNSi_SndSeqArcGetSeqInfo(struct ElemTable *obj, int idx) {
    struct Elem* e;
    if (idx < 0) return 0;
    if ((u32)idx >= (u32)obj->count) return 0;
    e = &obj->arr[idx];
    if (e->offset == -1)
        return 0;
    return e;
}

extern "C" void func_020522c4(void) {
    int i;
    int t1;
    Obj_205212c *stream = data_020a5bd4;
    for (i = 0; i < 4; i++) {
        int flags = stream->uf0;
        if ((flags << 31) >> 31) {
            if (stream->stopPending != 0) {
                func_02052208(stream);
            } else {
                if ((flags << 29) >> 31) {
                    if (stream->startReady != 0) {
                        stream->func_0204ffcc();
                        {
                            stream->uf0 |= 2;
                            stream->uf0 &= ~4;
                        }
                    }
                }
                if (((int)stream->uf0 << 30) >> 31) {
                    unsigned long t2;
                    NNSi_SndFaderUpdate(&stream->fade);
                    t1 = data_02086384[stream->volumeIndex];
                    t2 = data_02086384[NNSi_SndFaderGet(&stream->fade) >> 8];
                    t1 = t2 + t1;
                    if (t1 != stream->lastVolume) {
                        func_0204fe5c(stream, t1);
                        stream->lastVolume = t1;
                    }
                    if (((int)stream->uf0 << 28) >> 31) {
                        if (NNSi_SndFaderIsFinished(&stream->fade)) {
                            func_02052208(stream);
                        }
                    }
                }
            }
        }
        stream++;
    }
}

extern "C" void func_0205227c(void) {
    int i;
    for (i = 0; i < 4; i++) {
        if (((int)data_020a5bd4[i].uf0 << 30) >> 31) {
            func_0204ff48(&data_020a5bd4[i]);
        }
    }
}

extern "C" void func_02052234(void) {
    int i;
    for (i = 0; i < 4; i++) {
        if (((int)data_020a5bd4[i].uf0 << 30) >> 31) {
            func_0204fed0(&data_020a5bd4[i]);
        }
    }
}

extern "C" void func_02052208(Obj_205212c *stream) {
    int v = stream->uf0;
    if ((v << 30) >> 31)
        func_0204ff9c(stream);
    func_0205212c(stream);
}

extern "C" void func_0205212c(Obj_205212c *self) {
    if (self->u114 != 0) {
        *self->u114 = 0;
        self->u114 = 0;
    }
    func_020520dc(self);
    if (FS_IsFile(&self->file)) {
        FS_CloseFile(&self->file);
    }
    OSIntrMode irq = OS_DisableInterrupts();
    HeapAllocator_205212c *node =
        (HeapAllocator_205212c *)NNS_FndGetNextListObject(&data_020a5bc8, 0);
    while (node != 0) {
        HeapAllocator_205212c *next =
            (HeapAllocator_205212c *)NNS_FndGetNextListObject(&data_020a5bc8, node);
        if (node->owner_8 == self) {
            NNS_FndRemoveListObject(&data_020a5bc8, node);
            FreeCommandBuffer(node);
        }
        node = next;
    }
    OS_RestoreInterrupts(irq);
    u32 *p = &self->uf0;
    *p &= ~1u;
    *p &= ~4u;
    *p &= ~2u;
}

extern "C" void func_020520dc(Obj_205212c *self) {
    if (self->useCount == 0)
        return;

    self->useCount -= 1;

    if (self->useCount != 0)
        return;

    func_02050008(self);
}

extern "C" void FreeCommandBuffer(void *node) {
    OSIntrMode state;

    state = OS_DisableInterrupts();
    NNS_FndAppendListObject(&sFreeCommandList, node);
    OS_RestoreInterrupts(state);
}

extern "C" int NNS_SndArcPlayerSetup(struct SndHeap *heap) {
    int i;
    NNS_SndArcGetCurrent();
    for (i = 0; i < 32; i++) {
        struct SndPlayerInfo *info = NNS_SndArcGetPlayerInfo(i);
        if (info == 0)
            continue;
        Sound::Player::SetPlayableSeqCount(i, info->seqMax);
        if (info->heapSize == 0)
            continue;
        if (heap == 0)
            continue;
        {
            int j;
            for (j = 0; j < info->seqMax; j++) {
                if (NNS_SndPlayerCreateHeap(i, heap, info->heapSize) == 0)
                    return 0;
            }
        }
    }
    return 1;
}

extern "C" int NNS_SndArcPlayerStartSeq(void *handle, unsigned int seqNo) {
    Sound::InfoSequenceEntry *info = Sound::InfoSequenceEntry::GetWithID(seqNo);
    if (!info)
        return 0;
    return func_02051bd0((NNSSndHandle *)handle, info->playerNumber, info->bankId,
                         info->playerPriority, info, seqNo);
}

extern "C" int NNS_SndArcPlayerStartSeqArc(void *handle, u32 seqArcNo, int index) {
    struct SndArchiveInfo *info;
    struct ElemTable *arc;
    struct Elem *seq;
    info = NNS_SndArcGetSeqArcInfo(seqArcNo);
    if (info == 0)
        return 0;
    arc = (struct ElemTable *)NNS_SndArcGetFileAddress(info->fileId);
    if (arc == 0)
        return 0;
    seq = NNSi_SndSeqArcGetSeqInfo(arc, index);
    if (seq == 0)
        return 0;
    return func_02051a98((NNSSndHandle *)handle, seq->playerNumber, seq->bankId,
                         seq->playerPriority, seq, arc, seqArcNo, index);
}

extern "C" int func_02051e60(NNSSndHandle *handle, int playerNo, int bankNo, int playerPrio,
                             int seqArcNo, int index) {
    struct SndArchiveInfo *info;
    struct Elem *seq;
    struct ElemTable *arc;
    info = NNS_SndArcGetSeqArcInfo(seqArcNo);
    if (info == 0)
        return 0;
    arc = (struct ElemTable *)NNS_SndArcGetFileAddress(info->fileId);
    if (arc == 0)
        return 0;
    seq = NNSi_SndSeqArcGetSeqInfo(arc, index);
    if (seq == 0)
        return 0;
    if (playerPrio < 0)
        playerPrio = seq->playerPriority;
    if (bankNo < 0)
        bankNo = seq->bankId;
    if (playerNo < 0)
        playerNo = seq->playerNumber;
    return func_02051a98(handle, playerNo, bankNo, playerPrio, seq, arc, seqArcNo, index);
}

extern "C" int func_02051bd0(NNSSndHandle *handle, int playerNo, unsigned int bankNo,
                             int playerPrio, Sound::InfoSequenceEntry *info, int seqNo) {
    Sound::InfoInstrumentBankEntry *bank;
    struct SndArchiveInfo *waveArcInfo;
    NNSSndSeqPlayer *player;
    struct SndHeap *heap = 0;
    void *bankData;
    int i;
    void *seqData;

    bank = Sound::InfoInstrumentBankEntry::GetWithID(bankNo);
    if (bank == 0) return 0;

    player = NNSi_SndPlayerAllocSeqPlayer(handle, playerNo, playerPrio);
    if (player == 0)
        return 0;

    bankData = NNS_SndArcGetFileAddress(bank->fileId);
    if (bankData == 0) {
        heap = func_02051a28(playerNo, player);
        if (heap == 0) {
            NNSi_SndPlayerFreeSeqPlayer(player);
            return 0;
        }
        bankData = NNSi_SndArcLoadFile(bank->fileId, func_02051a68, 0, 0, heap);
        if (bankData == 0) {
            NNSi_SndPlayerFreeSeqPlayer(player);
            return 0;
        }
    }

    for (i = 0; i < 4; i++) {
        u16 waveArcNo = bank->waveArchiveIds[i];
        if (waveArcNo != 0xffff) {
            void *waveArc;
            waveArcInfo = NNS_SndArcGetWaveArcInfo(waveArcNo);
            if (waveArcInfo == 0) {
                NNSi_SndPlayerFreeSeqPlayer(player);
                return 0;
            }
            waveArc = NNS_SndArcGetFileAddress(waveArcInfo->fileId);
            if (waveArc == 0) {
                if (heap == 0) {
                    heap = func_02051a28(playerNo, player);
                    if (heap == 0) {
                        NNSi_SndPlayerFreeSeqPlayer(player);
                        return 0;
                    }
                }
                waveArc = NNSi_SndArcLoadFile(waveArcInfo->fileId, func_02051a48, 0, 0, heap);
                if (waveArc == 0) {
                    NNSi_SndPlayerFreeSeqPlayer(player);
                    return 0;
                }
            }
            SND_AssignWaveArc((struct SNDBankData *)bankData, i, (struct SNDWaveArc *)waveArc);
        }
    }

    seqData = NNS_SndArcGetFileAddress(info->fileId);
    if (seqData == 0) {
        if (heap == 0) {
            heap = func_02051a28(playerNo, player);
            if (heap == 0) {
                NNSi_SndPlayerFreeSeqPlayer(player);
                return 0;
            }
        }
        seqData = NNSi_SndArcLoadFile(info->fileId, func_02051a88, 0, 0, heap);
        if (seqData == 0) {
            NNSi_SndPlayerFreeSeqPlayer(player);
            return 0;
        }
    }

    NNSi_SndPlayerStartSeq(player, (char *)seqData + ((struct P5 *)seqData)->baseOffset, 0,
                           (struct SNDBankData *)bankData);
    NNS_SndPlayerSetInitialVolume(handle, info->volume);
    NNS_SndPlayerSetChannelPriority(handle, info->channelPriority);
    NNS_SndPlayerSetSeqNo(handle, seqNo);
    return 1;
}

extern "C" int func_02051a98(NNSSndHandle *handle, int playerNo, unsigned int bankNo,
                             int playerPrio, struct Elem *seq, struct ElemTable *arc, int seqArcNo,
                             int index) {
    Sound::InfoInstrumentBankEntry *bank;
    NNSSndSeqPlayer *player;
    void *bankData;
    int i;

    bank = Sound::InfoInstrumentBankEntry::GetWithID(bankNo);
    if (bank == 0)
        return 0;
    player = NNSi_SndPlayerAllocSeqPlayer(handle, playerNo, playerPrio);
    if (player == 0)
        return 0;
    bankData = NNS_SndArcGetFileAddress(bank->fileId);
    if (bankData == 0) {
        NNSi_SndPlayerFreeSeqPlayer(player);
        return 0;
    }
    for (i = 0; i < 4; i++) {
        unsigned int waveArcNo = bank->waveArchiveIds[i];
        struct SndArchiveInfo *info;
        void *waveArc;
        if (waveArcNo != 0xffff) {
            info = NNS_SndArcGetWaveArcInfo(waveArcNo);
            if (info == 0) {
                NNSi_SndPlayerFreeSeqPlayer(player);
                return 0;
            }
            waveArc = NNS_SndArcGetFileAddress(info->fileId);
            if (waveArc == 0) {
                NNSi_SndPlayerFreeSeqPlayer(player);
                return 0;
            }
            SND_AssignWaveArc((struct SNDBankData *)bankData, i, (struct SNDWaveArc *)waveArc);
        }
    }
    NNSi_SndPlayerStartSeq(player, (char *)arc + arc->baseOffset, seq->offset,
                           (struct SNDBankData *)bankData);
    NNS_SndPlayerSetInitialVolume(handle, seq->volume);
    NNS_SndPlayerSetChannelPriority(handle, seq->channelPriority);
    NNS_SndPlayerSetSeqArcNo(handle, seqArcNo, index);
    return 1;
}

extern "C" void func_02051a88(void *mem, u32 size, u32 data1, u32 data2) {
    SND_InvalidateSeqData(mem, (u8 *)mem + size);
}

extern "C" void func_02051a68(void *mem, u32 size, u32 data1, u32 data2) {
    SND_InvalidateBankData(mem, (u8 *)mem + size);
    SND_DestroyBank((struct SNDBankData *)mem);
}

extern "C" void func_02051a48(void *mem, u32 size, u32 data1, u32 data2) {
    SND_InvalidateWaveData(mem, (u8 *)mem + size);
    SND_DestroyWaveArc((struct SNDWaveArc *)mem);
}

extern "C" struct SndHeap *func_02051a28(int playerNo, NNSSndSeqPlayer *player) {
    struct SndHeap *heap = NNSi_SndPlayerAllocHeap(playerNo, player);
    if (heap) {
        NNS_SndHeapClear(heap);
    }
    return heap;
}

extern "C" int func_02051918(unsigned int id, struct SndHeap *heap) {
    struct List *l;
    unsigned int i;
    struct Entry *e;

    l = NNS_SndArcGetGroupInfo(id);
    if (!l)
        return 0;

    i = 0;
    if (i < l->count) {
        e = l->entries;
        do {
            switch (e->kind) {
            case 0:
                if (func_0205179c(e->w, e->b1, heap) == 0)
                    return 0;
                break;
            case 3:
                if (func_020518dc(e->w, heap) == 0)
                    return 0;
                break;
            case 1:
                if (func_020516d4(e->w, e->b1, heap) == 0)
                    return 0;
                break;
            case 2:
                if (func_020518a0(e->w, heap) == 0)
                    return 0;
                break;
            }
            i++;
            e++;
        } while (i < l->count);
    }

    return 1;
}

extern "C" int func_020518dc(u32 id, struct SndHeap *heap) {
    struct SndArchiveInfo *info = NNS_SndArcGetSeqArcInfo(id);
    if (info == 0)
        return 0;
    return LoadSeqArc(info->fileId, heap) != 0;
}

extern "C" int func_020518a0(u32 id, struct SndHeap *heap) {
    struct SndArchiveInfo *info = NNS_SndArcGetWaveArcInfo(id);
    if (info == 0)
        return 0;
    return LoadWaveArc(info->fileId, heap) != 0;
}

extern "C" int func_0205179c(unsigned int id, int flags, struct SndHeap *heap) {
    Sound::InfoSequenceEntry *seq;
    Sound::InfoInstrumentBankEntry *bank;
    void *bankData = 0;
    int i;

    seq = Sound::InfoSequenceEntry::GetWithID(id);
    if (!seq)
        return 0;
    bank = Sound::InfoInstrumentBankEntry::GetWithID(seq->bankId);
    if (!bank)
        return 0;

    if (flags & 2) {
        bankData = LoadBank(bank->fileId, heap);
        if (!bankData)
            return 0;
    }
    if (flags & 4) {
        for (i = 0; i < 4; i++) {
            u16 id2 = bank->waveArchiveIds[i];
            if (id2 != 0xffff) {
                struct SndArchiveInfo *info = NNS_SndArcGetWaveArcInfo(id2);
                void *waveArc;
                if (!info)
                    return 0;
                waveArc = LoadWaveArc(info->fileId, heap);
                if (!waveArc)
                    return 0;
                if (bankData != 0)
                    SND_AssignWaveArc((struct SNDBankData *)bankData, i,
                                      (struct SNDWaveArc *)waveArc);
            }
        }
    }
    if (flags & 1) {
        void *seqData = LoadSeq(seq->fileId, heap);
        if (!seqData)
            return 0;
    }
    return 1;
}

extern "C" int func_020516d4(unsigned int id, int flags, struct SndHeap *heap) {
    Sound::InfoInstrumentBankEntry *entry;
    void *bankData;
    int i;

    bankData = 0;
    entry = Sound::InfoInstrumentBankEntry::GetWithID(id);
    if (entry == 0) return 0;

    if (flags & 2) {
        bankData = LoadBank(entry->fileId, heap);
        if (bankData == 0)
            return 0;
    }

    if (flags & 4) {
        for (i = 0; i < 4; i++) {
            if (entry->waveArchiveIds[i] != 0xffff) {
                struct SndArchiveInfo *info;
                void *waveArc;
                info = NNS_SndArcGetWaveArcInfo(entry->waveArchiveIds[i]);
                if (info == 0)
                    return 0;
                waveArc = LoadWaveArc(info->fileId, heap);
                if (waveArc == 0)
                    return 0;
                if (bankData != 0) {
                    SND_AssignWaveArc((struct SNDBankData *)bankData, i,
                                      (struct SNDWaveArc *)waveArc);
                }
            }
        }
    }
    return 1;
}

extern "C" void *NNSi_SndArcLoadFile(u32 fileId, SndHeapDisposeCallback callback, u32 data1,
                                     u32 data2, struct SndHeap *heap) {
    int size = NNS_SndArcGetFileSize(fileId);
    void *buffer;
    if (size == 0)
        return 0;
    buffer = NNS_SndHeapAlloc(heap, size + 0x20, callback, data1, data2);
    if (buffer == 0)
        return 0;
    if (size != NNS_SndArcReadFile(fileId, buffer, size, 0))
        return 0;
    DC_StoreRange(buffer, size);
    return buffer;
}

extern "C" void *LoadSeq(u32 fileId, struct SndHeap *heap) {
    void *data = NNS_SndArcGetFileAddress(fileId);
    if (data == 0) {
        data =
            NNSi_SndArcLoadFile(fileId, SeqDisposeCallback, NNS_SndArcGetCurrent(), fileId, heap);
        if (data != 0) {
            NNS_SndArcSetFileAddress(fileId, data);
        }
    }
    return data;
}

extern "C" void *LoadSeqArc(u32 fileId, struct SndHeap *heap) {
    void *data = NNS_SndArcGetFileAddress(fileId);
    if (data == 0) {
        data =
            NNSi_SndArcLoadFile(fileId, SeqDisposeCallback, NNS_SndArcGetCurrent(), fileId, heap);
        if (data != 0) {
            NNS_SndArcSetFileAddress(fileId, data);
        }
    }
    return data;
}

extern "C" void *LoadBank(u32 fileId, struct SndHeap *heap) {
    void *data = NNS_SndArcGetFileAddress(fileId);
    if (data == 0) {
        data =
            NNSi_SndArcLoadFile(fileId, BankDisposeCallback, NNS_SndArcGetCurrent(), fileId, heap);
        if (data != 0) {
            NNS_SndArcSetFileAddress(fileId, data);
        }
    }
    return data;
}

extern "C" void *LoadWaveArc(u32 fileId, struct SndHeap *heap) {
    void *data = NNS_SndArcGetFileAddress(fileId);
    if (data == 0) {
        data = NNSi_SndArcLoadFile(fileId, WaveArcDisposeCallback, NNS_SndArcGetCurrent(), fileId,
                                   heap);
        if (data != 0) {
            NNS_SndArcSetFileAddress(fileId, data);
        }
    }
    return data;
}

extern "C" void DisposeCallback(void *mem, int arc, u32 fileId) {
    int prev;
    OSIntrMode irq;
    irq = OS_DisableInterrupts();
    prev = NNS_SndArcSetCurrent(arc);
    if (mem == NNS_SndArcGetFileAddress(fileId)) {
        NNS_SndArcSetFileAddress(fileId, 0);
    }
    NNS_SndArcSetCurrent(prev);
    OS_RestoreInterrupts(irq);
}

extern "C" void SeqDisposeCallback(void *mem, u32 size, u32 arc, u32 fileId) {
    DisposeCallback(mem, arc, fileId);
    SND_InvalidateSeqData(mem, (u8 *)mem + size);
}

extern "C" void BankDisposeCallback(void *mem, u32 size, u32 arc, u32 fileId) {
    DisposeCallback(mem, arc, fileId);
    SND_InvalidateBankData(mem, (u8 *)mem + size);
    SND_DestroyBank((struct SNDBankData *)mem);
}

extern "C" void WaveArcDisposeCallback(void *mem, u32 size, u32 arc, u32 fileId) {
    DisposeCallback(mem, arc, fileId);
    SND_InvalidateWaveData(mem, (u8 *)mem + size);
    SND_DestroyWaveArc((struct SNDWaveArc *)mem);
}

extern "C" struct SndHeap *NNS_SndHeapCreate(u32 addr, u32 size) {
    u32 end = addr + size;
    u32 aligned = (addr + 3) & ~3u;
    NNSFndHeapHandle frmHeap;
    if (aligned > end)
        return 0;
    /* The SndHeap InitHeap fills sits at aligned; the frame heap takes the rest. */
    if (end - aligned < sizeof(struct SndHeap))
        return 0;
    frmHeap = NNS_FndCreateFrmHeapEx((void *)(aligned + sizeof(struct SndHeap)),
                                     (end - aligned) - sizeof(struct SndHeap), 0);
    if (frmHeap == 0)
        return 0;
    if (InitHeap((struct SndHeap *)aligned, frmHeap) != 0)
        return (struct SndHeap *)aligned;
    NNS_FndDestroyFrmHeap(frmHeap);
    return 0;
}

extern "C" void NNS_SndHeapDestroy(struct SndHeap *heap) {
    NNS_SndHeapClear(heap);
    NNS_FndDestroyFrmHeap(heap->frmHeap);
}

extern "C" void NNS_SndHeapClear(struct SndHeap *heap) {
    NNSFndList *level;
    struct SndHeapBlock *block;
    level = (NNSFndList *)NNS_FndGetPrevListObject(&heap->levels, 0);
    if (level != 0) {
        do {
            block = (struct SndHeapBlock *)NNS_FndGetPrevListObject(level, 0);
            while (block != 0) {
                if (block->callback != 0) {
                    block->callback(block->mem, block->size, block->data1, block->data2);
                }
                block = (struct SndHeapBlock *)NNS_FndGetPrevListObject(level, block);
            }
            NNS_FndRemoveListObject(&heap->levels, level);
            level = (NNSFndList *)NNS_FndGetPrevListObject(&heap->levels, 0);
        } while (level != 0);
    }
    NNS_FndFreeToFrmHeap(heap->frmHeap, NNS_FND_FRMHEAP_FREE_ALL);
    EraseSync();
    NewSection(heap);
}

extern "C" void *NNS_SndHeapAlloc(struct SndHeap *heap, u32 size, SndHeapDisposeCallback callback,
                                  u32 data1, u32 data2) {
    struct SndHeapBlock *block;
    NNSFndList *level;

    block = (struct SndHeapBlock *)NNS_FndAllocFromFrmHeapEx(heap->frmHeap,
                                                             ((size + 0x1f) & ~0x1fu) + 0x20, 0x20);
    if (block == 0)
        return 0;

    level = (NNSFndList *)NNS_FndGetPrevListObject(&heap->levels, 0);
    block->size = size;
    block->callback = callback;
    block->data1 = data1;
    block->data2 = data2;
    NNS_FndAppendListObject(level, block);

    return block->mem;
}

extern "C" s32 NNS_SndHeapSaveState(struct SndHeap *heap) {
    u16 level = heap->levels.numObjects;
    NNSFndHeapHandle frmHeap = heap->frmHeap;
    if (!NNS_FndRecordStateForFrmHeap(frmHeap, level)) {
        return ~0;
    }
    if (NewSection(heap)) {
        return (u32)heap->levels.numObjects - 1;
    }
    NNS_FndFreeByStateToFrmHeap(heap->frmHeap, 0);
    return ~0;
}

extern "C" void NNS_SndHeapLoadState(struct SndHeap *heap, int level) {
    struct SndHeapBlock *block;
    NNSFndList *top;

    block = 0;
    if (level == 0) {
        NNS_SndHeapClear(heap);
        return;
    }
    if (level < heap->levels.numObjects) {
        do {
            top = (NNSFndList *)NNS_FndGetPrevListObject(&heap->levels, 0);
            block = (struct SndHeapBlock *)NNS_FndGetPrevListObject(top, block);
            while (block != 0) {
                if (block->callback != 0) {
                    block->callback(block->mem, block->size, block->data1, block->data2);
                }
                block = (struct SndHeapBlock *)NNS_FndGetPrevListObject(top, block);
            }
            NNS_FndRemoveListObject(&heap->levels, top);
        } while (level < heap->levels.numObjects);
    }
    NNS_FndFreeByStateToFrmHeap(heap->frmHeap, level);
    EraseSync();
    NNS_FndRecordStateForFrmHeap(heap->frmHeap, heap->levels.numObjects);
    NewSection(heap);
}

extern "C" u32 NNS_SndHeapGetFreeSize(struct SndHeap *heap) {
    u32 size;
    size = NNS_FndGetAllocatableSizeForFrmHeapEx(heap->frmHeap, 0x20);
    if (size < 0x20)
        return 0;
    size -= 0x20;
    size &= ~0x1fu;
    return size;
}

extern "C" void InitHeapSection(NNSFndList *level) {
    NNS_FndInitList(level, 0);
}

extern "C" s32 InitHeap(struct SndHeap *heap, NNSFndHeapHandle frmHeap) {
    NNS_FndInitList(&heap->levels, 0xc);
    heap->frmHeap = frmHeap;
    if (NewSection(heap)) {
        return 1;
    }
    return 0;
}

extern "C" s32 NewSection(struct SndHeap *heap) {
    NNSFndList *level = (NNSFndList *)NNS_FndAllocFromFrmHeapEx(heap->frmHeap, 0x14, 4);
    if (!level) {
        return 0;
    }
    InitHeapSection(level);
    NNS_FndAppendListObject(&heap->levels, level);
    return 1;
}

extern "C" void EraseSync(void) {
    u32 r = SND_GetCurrentCommandTag();
    SND_FlushCommand(1);
    SND_WaitForCommandProc(r);
}

extern "C" void NNS_SndArcInit(data_020a5bb8_t *arc, const char *path, struct SndHeap *heap,
                               int loadSymbols) {
    arc->info = 0;
    arc->fileTable = 0;
    arc->symbols = 0;
    FS_ConvertPathToFileID(&arc->fileId, path);
    FS_InitFile(&arc->file);
    FS_OpenFileFast(&arc->file, arc->fileId);
    FS_ReadFile(&arc->file, arc, 0x30);
    if (heap != 0)
        func_02050d54(arc, heap, loadSymbols);
    sCurrent = arc;
}

extern "C" int func_02050d54(data_020a5bb8_t *arc, struct SndHeap *heap, int loadSymbols) {
    unsigned int sz;
    int r;
    arc->info = NNS_SndHeapAlloc(heap, arc->infoSize, InfoDisposeCallback, (u32)arc, 0);
    if (arc->info == 0)
        return 0;
    arc->fileTable =
        (struct Hdr84 *)NNS_SndHeapAlloc(heap, arc->fileTableSize, FatDisposeCallback, (u32)arc, 0);
    if (arc->fileTable == 0)
        return 0;
    if (loadSymbols != 0 && (sz = arc->symbolSize) != 0) {
        arc->symbols = NNS_SndHeapAlloc(heap, sz, SymbolDisposeCallback, (u32)arc, 0);
        if (arc->symbols == 0)
            return 0;
    }
    if (FS_SeekFile(&arc->file, arc->infoOffset, FS_SEEK_SET) == 0)
        return 0;
    r = FS_ReadFile(&arc->file, arc->info, arc->infoSize);
    if (r != arc->infoSize)
        return 0;
    if (FS_SeekFile(&arc->file, arc->fileTableOffset, FS_SEEK_SET) == 0)
        return 0;
    r = FS_ReadFile(&arc->file, arc->fileTable, arc->fileTableSize);
    if (r != arc->fileTableSize)
        return 0;
    if (loadSymbols != 0 && arc->symbolSize != 0) {
        if (FS_SeekFile(&arc->file, arc->symbolOffset, FS_SEEK_SET) == 0)
            return 0;
        r = FS_ReadFile(&arc->file, arc->symbols, arc->symbolSize);
        if (r != arc->symbolSize)
            return 0;
    }
    return 1;
}

extern "C" int NNS_SndArcSetCurrent(int v) {
    int old = ((int)sCurrent);
    sCurrent = (data_020a5bb8_t *)v;
    return old;
}

extern "C" int NNS_SndArcGetCurrent(void) {
    return ((int)sCurrent);
}

extern "C" void *NNS_SndArcGetSeqArcParam(int a, int idx) {
    struct SndArchiveInfo *info;
    struct ElemTable *o;
    struct Elem *e;

    info = NNS_SndArcGetSeqArcInfo(a);
    if (info == 0)
        return 0;
    o = (struct ElemTable *)NNS_SndArcGetFileAddress(info->fileId);
    if (o == 0)
        return 0;
    e = NNSi_SndSeqArcGetSeqInfo(o, idx);
    if (e == 0)
        return 0;
    return &e->bankId;
}

Sound::InfoSequenceEntry *Sound::InfoSequenceEntry::GetWithID(int id) {
    data_020a5bb8_t *root = sCurrent;
    char *sub = (char *)root->info;
    int off = ((struct SndInfoBlock *)sub)->seqTable;
    char* tbl = (off == 0) ? 0 : sub + off;
    if (tbl == 0) return 0;
    if (id < 0)
        return 0;
    SoundOffsetTable *table = (SoundOffsetTable *)tbl;
    if (id >= table->count)
        return 0;
    int v = table->offset[id];
    char *base = (char *)root->info;
    return (v == 0) ? 0 : (InfoSequenceEntry *)(base + v);
}

extern "C" struct SndArchiveInfo *NNS_SndArcGetSeqArcInfo(int id) {
    data_020a5bb8_t *root = sCurrent;
    char *sub = (char *)root->info;
    int off = ((struct SndInfoBlock *)sub)->seqArcTable;
    char* tbl = (off == 0) ? 0 : sub + off;
    if (tbl == 0) return 0;
    if (id < 0)
        return 0;
    SoundOffsetTable *table = (SoundOffsetTable *)tbl;
    if (id >= table->count)
        return 0;
    int v = table->offset[id];
    char *base = (char *)root->info;
    return (v == 0) ? 0 : (struct SndArchiveInfo *)(base + v);
}

Sound::InfoInstrumentBankEntry *Sound::InfoInstrumentBankEntry::GetWithID(int id) {
    data_020a5bb8_t *root = sCurrent;
    char *sub = (char *)root->info;
    int off = ((struct SndInfoBlock *)sub)->bankTable;
    char* tbl = (off == 0) ? 0 : sub + off;
    if (tbl == 0) return 0;
    if (id < 0)
        return 0;
    SoundOffsetTable *table = (SoundOffsetTable *)tbl;
    if (id >= table->count)
        return 0;
    int v = table->offset[id];
    char *base = (char *)root->info;
    return (v == 0) ? 0 : (InfoInstrumentBankEntry *)(base + v);
}

extern "C" struct SndArchiveInfo *NNS_SndArcGetWaveArcInfo(int id) {
    data_020a5bb8_t *root = sCurrent;
    char *sub = (char *)root->info;
    int off = ((struct SndInfoBlock *)sub)->waveArcTable;
    char* tbl = (off == 0) ? 0 : sub + off;
    if (tbl == 0) return 0;
    if (id < 0)
        return 0;
    SoundOffsetTable *table = (SoundOffsetTable *)tbl;
    if (id >= table->count)
        return 0;
    int v = table->offset[id];
    char *base = (char *)root->info;
    return (v == 0) ? 0 : (struct SndArchiveInfo *)(base + v);
}

extern "C" struct SndPlayerInfo *NNS_SndArcGetPlayerInfo(int id) {
    data_020a5bb8_t *root = sCurrent;
    char *sub = (char *)root->info;
    int off = ((struct SndInfoBlock *)sub)->playerTable;
    char* tbl = (off == 0) ? 0 : sub + off;
    if (tbl == 0) return 0;
    if (id < 0)
        return 0;
    SoundOffsetTable *table = (SoundOffsetTable *)tbl;
    if (id >= table->count)
        return 0;
    int v = table->offset[id];
    char *base = (char *)root->info;
    return (v == 0) ? 0 : (struct SndPlayerInfo *)(base + v);
}

extern "C" struct List *NNS_SndArcGetGroupInfo(int id) {
    data_020a5bb8_t *root = sCurrent;
    char *sub = (char *)root->info;
    int off = ((struct SndInfoBlock *)sub)->groupTable;
    char* tbl = (off == 0) ? 0 : sub + off;
    if (tbl == 0) return 0;
    if (id < 0)
        return 0;
    SoundOffsetTable *table = (SoundOffsetTable *)tbl;
    if (id >= table->count)
        return 0;
    int v = table->offset[id];
    char *base = (char *)root->info;
    return (v == 0) ? 0 : (struct List *)(base + v);
}

extern "C" int NNS_SndArcGetFileSize(unsigned int i) {
    struct Hdr84 *h = sCurrent->fileTable;
    if (i >= (unsigned int)h->count) return 0;
    return h->entries[i].size;
}

extern "C" int NNS_SndArcReadFile(u32 fileId, void *buffer, u32 size, u32 offset) {
    data_020a5bb8_t *arc = sCurrent;
    struct Hdr84 *fat = arc->fileTable;
    struct FatEntry *entry;
    u32 avail;

    if (fileId >= (u32)fat->count)
        return -1;

    entry = &fat->entries[fileId];
    avail = (u32)entry->size - offset;
    if (size > avail)
        size = avail;

    if (FS_SeekFile(&arc->file, entry->offset + offset, FS_SEEK_SET) == 0)
        return -1;

    return FS_ReadFile(&arc->file, buffer, size);
}

extern "C" void *NNS_SndArcGetFileAddress(u32 fileId) {
    struct Hdr84 *h = sCurrent->fileTable;
    if (fileId >= (unsigned int)h->count)
        return 0;
    return h->entries[fileId].address;
}

extern "C" void NNS_SndArcSetFileAddress(u32 fileId, void *address) {
    sCurrent->fileTable->entries[fileId].address = address;
}

extern "C" void InfoDisposeCallback(void *mem, u32 size, u32 arc, u32 data2) {
    ((data_020a5bb8_t *)arc)->info = 0;
}

extern "C" void FatDisposeCallback(void *mem, u32 size, u32 arc, u32 data2) {
    ((data_020a5bb8_t *)arc)->fileTable = 0;
}

extern "C" void SymbolDisposeCallback(void *mem, u32 size, u32 arc, u32 data2) {
    ((data_020a5bb8_t *)arc)->symbols = 0;
}

extern "C" void NNSi_SndCaptureInit(void) {
    data_020a55fc = 0;
    data_020a5634.f0 = (0);
}

extern "C" void NNSi_SndCaptureMain(void) {
    T *g = &data_020a5634;
    NNSSndFader *sub;
    int v;
    if (g->f0 == 0)
        return;
    if (g->f4 != 0)
        return;
    sub = &g->fade;
    NNSi_SndFaderUpdate(sub);
    if (g->f48 != 0) {
        if (NNSi_SndFaderIsFinished(sub) != 0) {
            NNSi_SndCaptureStop();
            return;
        }
    }
    v = NNSi_SndFaderGet(sub) >> 8;
    if (v == g->f4c)
        return;
    SND_SetChannelVolume(g->f20, v, 0);
    g->f4c = v;
}

extern "C" int NNS_SndCaptureStartEffect(void *a0, unsigned int a1, int a2, int a3, int a4,
                                         void *a5, void *a6) {
    volatile int z;
    NNS_SndCaptureStopEffect();
    if (data_020a5634.f0 != 0)
        return 0;
    z = 0;
    MI_CpuFill32(a0, z, a1);
    DC_FlushRange(a0, a1);
    return NNSi_SndCaptureStart(1, (int)a0, (int)a0 + (a1 >> 1), a1 >> 1, a2, 0, 0, 1, a3, 0x7f, 0,
                                0x7f, a4, (int)a5, (int)a6);
}

extern "C" void NNS_SndCaptureStopEffect(void) {
    if (data_020a5634.f0 == 0)
        return;
    if (data_020a5634.f4 != 1)
        return;
    NNSi_SndCaptureStop();
}

extern "C" void func_020506fc(int arg) {
    if (data_020a55fc != 0)
        return;
    data_020a55f8 = 0;
    OS_InitMessageQueue(&data_020a5600, data_020a5614, 8);
    OS_CreateThread(&data_020a5684, func_02050038, 0, &sCurrent, 0x400, arg);
    data_020a55fc = 1;
    OS_WakeupThreadDirect(&data_020a5684);
}

extern "C" int NNSi_SndCaptureStart(int a, int b, int c, int d, int p5, int p6, int p7, int p8,
                                    int p9, int p10, int p11, int p12, int p13, int p14, int p15) {
    int mode = 0;
    int align;
    int total;
    int blocks;
    int q;
    int fmt1;
    int fmt2;
    int m;
    int dd;
    int pp8;
    int pp10;
    u32 qlen;
    int stereo;
    int handle;
    int chunk;
    T *g;

    dd = (int)((d) & ~0ULL);
    pp8 = (int)((p8) & ~0ULL);
    pp10 = (int)((p10) & ~0ULL);

    handle = -1;
    g = &data_020a5634;
    DC_FlushRange((void *)b, dd);
    DC_FlushRange((void *)c, dd);
    stereo = p5 == 1;
    align = 0xffb0ff / p9;
    if (p14 != 0) {
        q = dd;
        if (stereo == 0) q = dd >> 1;
        align = (align + 0x10) & ~0x1f;
        blocks = align >> 5;
        q = q / p13;
        total = blocks * q;
        chunk = 0x20;
        if (stereo == 0) chunk = chunk >> 1;
        chunk = chunk * blocks;
    }
    {
        fmt1 = stereo != 0 ? FMT_A : FMT_B;
        fmt2 = stereo != 0 ? FMT_B : FMT_A;
        if (a != 2) mode = 0xa;
        if (p14 != 0) {
            handle = NNS_SndAllocAlarm();
            if (handle < 0) return 0;
        }
        if (NNS_SndLockCapture(3) == 0) {
            if (handle >= 0)
                NNS_SndFreeAlarm(handle);
            return 0;
        }
        if (NNS_SndLockChannel(0xa) == 0) {
            if (handle >= 0)
                NNS_SndFreeAlarm(handle);
            NNS_SndUnlockCapture(3);
            return 0;
        }
        qlen = (u32)dd >> 2;
        SND_SetupChannelPcm(1, fmt1, (void *)b, pp8 != 0 ? 1 : 2, 0, qlen, pp10, 0, align, p11);
        SND_SetupCapture(0, fmt2, (void *)b, qlen, pp8, p6, p7);
        SND_SetupChannelPcm(3, fmt1, (void *)c, pp8 != 0 ? 1 : 2, 0, qlen, pp10, 0, align, p12);
        SND_SetupCapture(1, fmt2, (void *)c, qlen, pp8, p6, p7);
        if (handle >= 0) {
            SND_SetupAlarm(handle, total + chunk, total, AlarmCallback, g);
        }
        if (a == 1) {
            SND_SetOutputSelector(3, 3, 1, 1);
        }
        if (handle >= 0) m = 1 << handle; else m = 0;
        SND_StartTimer(mode, 3, m, 0);
        g->f0 = 1;
        g->f4 = a;
        g->f1c = 0xa;
        g->f20 = mode;
        g->f24 = 3;
        g->f28 = handle;
        g->f8 = p5;
        g->fc = b;
        g->f10 = c;
        g->f14 = dd;
        g->f18 = 0;
        g->f2c = p13;
        g->f30 = (void (*)(int, int, int, int, int))p14;
        g->f34 = p15;
        g->f4c = pp10;
        NNSi_SndFaderInit(&g->fade);
        NNSi_SndFaderSet(&g->fade, pp10 << 8, 1);
        g->f48 = 0;
        return 1;
    }
}

extern "C" void NNSi_SndCaptureStop(void) {
    struct T *d = &data_020a5634;
    int has = (d->f28 >= 0);
    int mask = has ? (1 << d->f28) : 0;

    SND_StopTimer(d->f20, d->f24, mask, 0);
    if (has) {
        u32 x = SND_GetCurrentCommandTag();
        SND_FlushCommand(1);
        SND_WaitForCommandProc(x);
        while (OS_ReceiveMessage(&data_020a5600, 0, 0) != 0) {
        }
    }
    if (d->f24 != 0)
        NNS_SndUnlockCapture(d->f24);
    if (d->f1c != 0)
        NNS_SndUnlockChannel(d->f1c);
    if (has)
        NNS_SndFreeAlarm(d->f28);
    if (d->f4 == 1)
        SND_SetOutputSelector(0, 0, 0, 0);
    d->f0 = 0;
}

extern "C" void NNSi_SndCaptureBeginSleep(void) {
    struct T *G = &data_020a5634;
    int sh;
    int mask;
    u32 p;
    if (G->f0 == 0)
        return;
    sh = G->f28;
    if (sh >= 0)
        mask = 1 << sh;
    else
        mask = 0;
    SND_StopTimer(G->f20, G->f24, mask, 0);
    p = SND_GetCurrentCommandTag();
    SND_FlushCommand(1);
    SND_WaitForCommandProc(p);
}

extern "C" void NNSi_SndCaptureEndSleep(void) {
    T *g = &data_020a5634;
    volatile int a;
    volatile int b;
    if (g->f0 == 0) return;
    g->f18 = 0;
    a = 0;
    MI_CpuFill32((void *)g->fc, a, g->f14);
    b = 0;
    MI_CpuFill32((void *)g->f10, b, g->f14);
    DC_FlushRange((void *)g->fc, g->f14);
    DC_FlushRange((void *)g->f10, g->f14);
    {
        int sh = g->f28;
        int mask = (sh >= 0) ? (1 << sh) : 0;
        SND_StartTimer(g->f20, g->f24, mask, 0);
    }
}

extern "C" void AlarmCallback(void *arg) {
    struct T *strm = (struct T *)arg;
    int quot = strm->f14 / strm->f2c;
    int prod = quot * strm->f18;
    int e6 = strm->fc + prod;
    int e5 = strm->f10 + prod;

    if (data_020a55fc != 0) {
        struct Q *slot = &data_020a5718[data_020a55f8];
        slot->f0 = strm;
        slot->f4 = quot;
        slot->f8 = prod;
        slot->fc = e6;
        slot->f10 = e5;
        OS_SendMessage(&data_020a5600, slot, 0);
        data_020a55f8 = data_020a55f8 + 1;
        if (data_020a55f8 >= 8) data_020a55f8 = 0;
    } else {
        DC_InvalidateRange((void *)e6, quot);
        DC_InvalidateRange((void *)e5, quot);
        strm->f30(e6, e5, quot, strm->f8, strm->f34);
    }
    strm->f18++;
    if (strm->f18 >= strm->f2c)
        strm->f18 = 0;
}

extern "C" void func_02050038(void *arg) {
    struct Q *r;
    void *out;
    for (;;) {
        OS_ReceiveMessage(&data_020a5600, &out, 1);
        r = (struct Q *)out;
        DC_InvalidateRange((void *)r->fc, r->f4);
        DC_InvalidateRange((void *)r->f10, r->f4);
        r->f0->f30(r->fc, r->f10, r->f4, r->f0->f8, r->f0->f34);
    }
}

extern "C" void func_02050008(struct C *o) {
    if (o->f2c == 0) {
        return;
    }
    NNS_SndUnlockChannel(o->f2c);
    o->f2c = 0;
    o->chanCount = 0;
}

void C::func_0204ffcc() {
    SND_StartTimer(f2c, 0, 1 << f28, 0);
    fc |= 2;
}

extern "C" void func_0204ff9c(struct C *o) {
    if ((o->fc << 31) >> 31) {
        func_0204fde8(o);
    }
}

extern "C" void func_0204ff48(struct C *p) {
    if (!((p->fc << 30) >> 31))
        return;
    SND_StopTimer(p->f2c, 0, 1 << p->f28, 0);
    u32 x = SND_GetCurrentCommandTag();
    SND_FlushCommand(1);
    SND_WaitForCommandProc(x);
}

extern "C" void func_0204fed0(struct C *c) {
    if (((c->fc << 30) >> 31) == 0) return;
    while (c->f20 != 0) {
        OSIntrMode saved = OS_DisableInterrupts();
        c->func_0204fce8((void *)1);
        OS_RestoreInterrupts(saved);
    }
    SND_StartTimer(c->f2c, 0, 1 << c->f28, 0);
}

extern "C" void func_0204fe5c(struct C *c, int volume) {
    unsigned char idx;
    int i = 0;
    c->volume = volume;
    if (c->chanCount > 0) {
        do {
            idx = c->chans[i];
            int r = SND_CalcChannelVolume(c->volume + data_020a5578[idx].field4);
            SND_SetChannelVolume(1 << idx, r & 0xff, r >> 8);
            i++;
        } while (i < c->chanCount);
    }
}

extern "C" void func_0204fde8(struct C *obj) {
    if ((obj->fc << 0x1e) >> 0x1f) {
        u32 p;
        int *flags = &obj->fc;
        SND_StopTimer(obj->f2c, 0, 1 << obj->f28, 0);
        *flags &= ~2;
        p = SND_GetCurrentCommandTag();
        SND_FlushCommand(1);
        SND_WaitForCommandProc(p);
    }
    func_0204fda4(obj);
}

extern "C" void func_0204fda4(struct C *c) {
    NNS_SndFreeAlarm(c->f28);
    NNS_FndRemoveListObject(&data_020a552c, c);
    c->fc &= ~1;
    c->fc &= ~2;
}

void C::func_0204fce8(void *arg) {
    /* Kept (#821 r84): bufferAddr added inline (either order) -> bytes differ. */
    int i;
    int q = (u32)this->bufSize / (u32)this->blockCount;
    int scaled = q * this->f20;
    for (i = 0; i < this->chanCount; i++) {
        int bufferAddr = data_020a5578[this->chans[i]].field0;
        data_020a5538[i] = bufferAddr + scaled;
    }

    this->callback(arg, this->chanCount, data_020a5538, q, this->unk8, this->unk1c);
    this->f20++;
    if (this->f20 >= this->blockCount) {
        this->f20 = 0;
    }
}

extern "C" void NNSi_SndPlayerInit(void) {
    int i;
    NNS_FndInitList(&sPrioList, 0x14);
    NNS_FndInitList(&sFreeList, 0x14);
    for (i = 0; i < 16; i++) {
        sSeqPlayer[i].status = 0;
        sSeqPlayer[i].playerNo = (unsigned char)i;
        NNS_FndAppendListObject(&sFreeList, &sSeqPlayer[i]);
    }
    for (i = 0; i < 32; i++) {
        NNS_FndInitList(&sPlayer[i].mLists[0], 0xc);
        NNS_FndInitList(&sPlayer[i].mLists[1], 0);
        sPlayer[i].mPlayableSeqCount = 1;
    }
}

extern "C" void NNSi_SndPlayerMain(void) {
    NNSSndSeqPlayer *node;
    NNSSndSeqPlayer *next;
    unsigned int mask;
    int v;

    mask = SND_GetPlayerStatus();
    node = (NNSSndSeqPlayer *)NNS_FndGetNextListObject(&sPrioList, 0);
    if (node == 0) {
        return;
    }
    do {
        next = (NNSSndSeqPlayer *)NNS_FndGetNextListObject(&sPrioList, node);
        if (node->startFlag == 0) {
            if (SND_IsFinishedCommandTag(node->commandTag)) {
                node->startFlag = 1;
            }
        }
        if (node->startFlag != 0 && (mask & (1 << node->playerNo)) == 0) {
            ShutdownPlayer(node);
        } else {
            int t1, t2, t3;
            NNSi_SndFaderUpdate(&node->fader);
            t1 = data_02086384[node->extVolume];
            t2 = data_02086384[node->initVolume];
            t3 = data_02086384[NNSi_SndFaderGet(&node->fader) >> 8];
            v = t3 + (t2 + t1);
            if (v < -0x2d3) {
                v = -0x2d3;
            } else if (v > 0) {
                v = 0;
            }
            if (v != node->volume) {
                SND_SetPlayerVolume(node->playerNo, v);
                node->volume = v;
            }
            if (node->status == 2) {
                if (NNSi_SndFaderIsFinished(&node->fader)) {
                    ForceStopSeq(node);
                }
            }
        }
        node = next;
    } while (next != 0);
}

void Sound::Player::SetPlayableSeqCount(int index, int count)
{
    sPlayer[index].mPlayableSeqCount = (unsigned short)count;
}

extern "C" int NNS_SndPlayerCreateHeap(int playerNo, struct SndHeap *heap, u32 size) {
    Elem_204f278 *block;
    struct SndHeap *seqHeap;
    block = (Elem_204f278 *)NNS_SndHeapAlloc(heap, size + sizeof(Elem_204f278),
                                             PlayerHeapDisposeCallback, 0, 0);
    if (!block)
        return 0;
    block->player = 0;
    block->playerIndex = playerNo;
    block->heap = 0;
    seqHeap = NNS_SndHeapCreate((u32)(block + 1), size);
    if (!seqHeap)
        return 0;
    block->heap = seqHeap;
    NNS_FndAppendListObject(&sPlayer[playerNo].mLists[1], block);
    return 1;
}

extern "C" int NNS_SndPlayerStopSeq(NNSSndHandle *handle, int fade) {
    /* NNSi_SndPlayerStopSeq is void, so there is no return: the call is a tail call and r0 is the
       callee's. */
    NNSi_SndPlayerStopSeq(handle->player, fade);
}

extern "C" void NNS_SndPlayerStopSeqByPlayerNo(int idx, int fade) {
    int i;
    NNSSndSeqPlayer *p = sSeqPlayer;
    SoundPlayerRecord *player = &sPlayer[idx];
    for (i = 0; i < 16; i++, p++) {
        if (p->status != 0 && p->player == player) {
            NNSi_SndPlayerStopSeq(p, fade);
        }
    }
}

extern "C" void NNS_SndPlayerPauseByPlayerNo(int idx, int val) {
    NNSSndSeqPlayer *cur;
    NNSSndSeqPlayer *next;
    NNSFndList *list = &sPlayer[idx].mLists[0];
    cur = (NNSSndSeqPlayer *)NNS_FndGetNextListObject(list, 0);
    if (cur == 0) return;
    do {
        next = (NNSSndSeqPlayer *)NNS_FndGetNextListObject(list, cur);
        NNSi_SndPlayerPause(cur, val);
        cur = next;
    } while (cur != 0);
}

extern "C" void NNS_SndHandleInit(NNSSndHandle *handle) {
    handle->player = 0;
}

extern "C" void NNS_SndHandleReleaseSeq(NNSSndHandle *handle) {
    NNSSndSeqPlayer *player = handle->player;
    if (player != 0) {
        player->handle = 0;
        handle->player = 0;
    }
}

/* NitroSystem's NNS_SndPlayerSetVolume: if the handle holds a player, set
   its extVolume. This and the setters after it run in reverse snd_player.c
   source order, the order the Sonic Rush Adventure decomp lists them in;
   the handle and player types are nnsys/snd/player.h's. */
extern "C" void NNS_SndPlayerSetVolume(NNSSndHandle *handle, int volume) {
    NNSSndSeqPlayer *player = handle->player;
    if (player)
        player->extVolume = (u8)volume;
}

/* NitroSystem's NNS_SndPlayerSetInitialVolume: if the handle holds a
   player, set its initVolume. */
extern "C" void NNS_SndPlayerSetInitialVolume(NNSSndHandle *handle, int volume) {
    NNSSndSeqPlayer *player = handle->player;
    if (player)
        player->initVolume = (u8)volume;
}

/* NitroSystem's NNS_SndPlayerMoveVolume: unless the player is fading out
   (status 2), fade its volume to targetVolume over `frames` frames
   (NNSi_SndFaderSet, which works in 8.8 fixed point). */
extern "C" void NNS_SndPlayerMoveVolume(NNSSndHandle *handle, int targetVolume, int frames) {
    NNSSndSeqPlayer *player = handle->player;
    if (player == 0)
        return;
    if (player->status == 2)
        return;
    NNSi_SndFaderSet(&player->fader, targetVolume << 8, frames);
}

/* NitroSystem's NNS_SndPlayerSetChannelPriority: SND_SetPlayerChannelPriority
   on the player number. */
extern "C" void NNS_SndPlayerSetChannelPriority(NNSSndHandle *handle, int priority) {
    NNSSndSeqPlayer *player = handle->player;
    if (player)
        SND_SetPlayerChannelPriority(player->playerNo, priority);
}

/* NitroSystem's NNS_SndPlayerSetTrackMute: if the handle holds a player,
   mute (flag 1) or unmute the tracks in trackBitMask through
   SND_SetTrackMute on its player number. */
extern "C" void NNS_SndPlayerSetTrackMute(NNSSndHandle *handle, int trackBitMask, int flag) {
    NNSSndSeqPlayer *player = handle->player;
    if (player)
        SND_SetTrackMute(player->playerNo, trackBitMask, flag);
}

/* NitroSystem's NNS_SndPlayerSetTrackVolume: if the handle holds a player,
   set the tracks in trackBitMask to volume (0..127) through
   SND_SetTrackVolume, converted by SND_CalcDecibel, which
   is the SNDi_DecibelTable lookup data_02086384[volume]. */
extern "C" void NNS_SndPlayerSetTrackVolume(NNSSndHandle *handle, int trackBitMask, int volume) {
    NNSSndSeqPlayer *player = handle->player;
    if (player == 0)
        return;
    SND_SetTrackVolume(player->playerNo, trackBitMask, data_02086384[volume]);
}

/* NitroSystem's NNS_SndPlayerSetTrackPitch: SND_SetTrackPitch
   (SNDTrack.ext_pitch at 0xc) on the player number. The track setters
   below take the same shape; each SND_ callee is one SNDi_SetTrackParam
   write at the SNDTrack offset named. */
extern "C" void NNS_SndPlayerSetTrackPitch(NNSSndHandle *handle, int trackBitMask, int pitch) {
    NNSSndSeqPlayer *player = handle->player;
    if (player)
        SND_SetTrackPitch(player->playerNo, trackBitMask, pitch);
}

/* NitroSystem's NNS_SndPlayerSetTrackPan: SND_SetTrackPan (SNDTrack.ext_pan
   at 9). */
extern "C" void NNS_SndPlayerSetTrackPan(NNSSndHandle *handle, int trackBitMask, int pan) {
    NNSSndSeqPlayer *player = handle->player;
    if (player)
        SND_SetTrackPan(player->playerNo, trackBitMask, pan);
}

/* NitroSystem's NNS_SndPlayerSetTrackModDepth: SND_SetTrackModDepth
   (SNDTrack.mod.depth at 0x1a). The source puts
   NNS_SndPlayerSetTrackPanRange between this and the pan setter; it is
   absent here (the two are adjacent), presumably dead-stripped. */
extern "C" void NNS_SndPlayerSetTrackModDepth(NNSSndHandle *handle, int trackBitMask, int depth) {
    NNSSndSeqPlayer *player = handle->player;
    if (player)
        SND_SetTrackModDepth(player->playerNo, trackBitMask, depth);
}

/* NitroSystem's NNS_SndPlayerSetTrackModSpeed: SND_SetTrackModSpeed
   (SNDTrack.mod.speed at 0x19). */
extern "C" void NNS_SndPlayerSetTrackModSpeed(NNSSndHandle *handle, int trackBitMask, int speed) {
    NNSSndSeqPlayer *player = handle->player;
    if (player)
        SND_SetTrackModSpeed(player->playerNo, trackBitMask, speed);
}

/* NitroSystem's NNS_SndPlayerSetTempoRatio: SND_SetPlayerTempoRatio (an
   SNDi_SetPlayerParam write of SNDPlayer.tempo_ratio at 0x1a). */
extern "C" void NNS_SndPlayerSetTempoRatio(NNSSndHandle *handle, int ratio) {
    NNSSndSeqPlayer *player = handle->player;
    if (player)
        SND_SetPlayerTempoRatio(player->playerNo, ratio);
}

extern "C" void NNS_SndPlayerSetSeqNo(NNSSndHandle *handle, int seqNo) {
    NNSSndSeqPlayer *p = handle->player;
    if (p != 0) {
        p->seqType = 1;
        p = handle->player;
        p->seqNo = seqNo;
    }
}

extern "C" void NNS_SndPlayerSetSeqArcNo(NNSSndHandle *handle, int seqArcNo, int index) {
    if (handle->player == 0)
        return;
    handle->player->seqType = 2;
    handle->player->seqNo = seqArcNo;
    handle->player->seqArcIndex = index;
}

extern "C" NNSSndSeqPlayer *NNSi_SndPlayerAllocSeqPlayer(NNSSndHandle *handle, int playerIdx,
                                                         int prio) {
    SoundPlayerRecord *record = &sPlayer[playerIdx];
    NNSSndSeqPlayer *player;
    if (handle->player != 0)
        NNS_SndHandleReleaseSeq(handle);
    if (record->mLists[0].numObjects >= record->mPlayableSeqCount) {
        NNSSndSeqPlayer *oldest =
            (NNSSndSeqPlayer *)NNS_FndGetNextListObject(&record->mLists[0], 0);
        if (oldest == 0)
            return 0;
        if (prio < oldest->prio)
            return 0;
        ForceStopSeq(oldest);
    }
    player = AllocSeqPlayer(prio);
    if (player == 0)
        return 0;
    InsertPlayerList(&record->mLists[0], player);
    player->handle = handle;
    handle->player = player;
    return player;
}

extern "C" void NNSi_SndPlayerFreeSeqPlayer(NNSSndSeqPlayer *player) {
    ShutdownPlayer(player);
}

extern "C" int NNSi_SndPlayerStartSeq(NNSSndSeqPlayer *player, const void *seqData, u32 seqOffset,
                                      struct SNDBankData *bank) {
    SND_StartSeq(player->playerNo, seqData, seqOffset, bank);
    InitPlayer(player);
    player->commandTag = SND_GetCurrentCommandTag();
    player->status = 1;
    return 1;
}

extern "C" void NNSi_SndPlayerStopSeq(NNSSndSeqPlayer *player, int fadeFrames) {
    if (player == 0)
        return;
    if (player->status == 0)
        return;
    if (fadeFrames == 0) {
        ForceStopSeq(player);
        return;
    }
    NNSi_SndFaderSet(&player->fader, 0, fadeFrames);
    SetPlayerPriority(player, 0);
    player->status = 2;
}

extern "C" void NNSi_SndPlayerPause(NNSSndSeqPlayer *player, int flag) {
    if (player == 0)
        return;
    if (flag == player->pauseFlag)
        return;
    SND_PauseSeq(player->playerNo, flag);
    player->pauseFlag = (unsigned char)flag;
}

extern "C" struct SndHeap *NNSi_SndPlayerAllocHeap(int playerNo, NNSSndSeqPlayer *player) {
    SoundPlayerRecord *record = &sPlayer[playerNo];
    Elem_204f278 *block = (Elem_204f278 *)NNS_FndGetNextListObject(&record->mLists[1], 0);
    if (block == 0)
        return 0;
    NNS_FndRemoveListObject(&record->mLists[1], block);
    block->player = player;
    player->heap = block;
    return block->heap;
}

extern "C" void InitPlayer(NNSSndSeqPlayer *player) {
    player->pauseFlag = 0;
    player->startFlag = 0;
    player->seqType = 0;
    player->volume = 0;
    player->initVolume = 0x7f;
    player->extVolume = 0x7f;
    NNSi_SndFaderInit(&player->fader);
    NNSi_SndFaderSet(&player->fader, 0x7f00, 1);
}

extern "C" void InsertPlayerList(NNSFndList *list, NNSSndSeqPlayer *player) {
    NNSSndSeqPlayer *cur;
    cur = (NNSSndSeqPlayer *)NNS_FndGetNextListObject(list, 0);
    if (cur != 0) {
        for (;;) {
            if (player->prio < cur->prio)
                break;
            cur = (NNSSndSeqPlayer *)NNS_FndGetNextListObject(list, cur);
            if (cur == 0)
                break;
        }
    }
    NNS_FndInsertListObject(list, cur, player);
    player->player = list;
}

extern "C" void InsertPrioList(NNSSndSeqPlayer *self) {
    NNSSndSeqPlayer *cur;
    cur = (NNSSndSeqPlayer *)(NNS_FndGetNextListObject(&sPrioList, 0));
    if (cur != 0) {
        for (;;) {
            if (self->prio < cur->prio)
                break;
            cur = (NNSSndSeqPlayer *)(NNS_FndGetNextListObject(&sPrioList, cur));
            if (cur == 0)
                break;
        }
    }
    NNS_FndInsertListObject(&sPrioList, cur, self);
}

extern "C" void ForceStopSeq(NNSSndSeqPlayer *player) {
    SND_StopSeq(player->playerNo);
    ShutdownPlayer(player);
}

extern "C" NNSSndSeqPlayer *AllocSeqPlayer(int prio) {
    NNSSndSeqPlayer *player = (NNSSndSeqPlayer *)NNS_FndGetNextListObject(&sFreeList, 0);
    if (player == 0) {
        player = (NNSSndSeqPlayer *)NNS_FndGetNextListObject(&sPrioList, 0);
        if (prio < player->prio) {
            return 0;
        }
        ForceStopSeq(player);
    }
    NNS_FndRemoveListObject(&sFreeList, player);
    player->prio = prio;
    InsertPrioList(player);
    return player;
}

extern "C" void ShutdownPlayer(NNSSndSeqPlayer *obj) {
    NNSSndHandle *handle = obj->handle;
    if (handle != 0) {
        handle->player = 0;
        obj->handle = 0;
    }
    SoundPlayerRecord *player = (SoundPlayerRecord *)obj->player;
    NNS_FndRemoveListObject(&player->mLists[0], obj);
    obj->player = 0;
    if (obj->heap != 0) {
        NNS_FndAppendListObject(&player->mLists[1], obj->heap);
        ((Elem_204f278 *)obj->heap)->player = 0;
        obj->heap = 0;
    }
    NNS_FndRemoveListObject(&sPrioList, obj);
    NNS_FndAppendListObject(&sFreeList, obj);
    obj->status = 0;
}

extern "C" void PlayerHeapDisposeCallback(void *mem, u32 size, u32 data1, u32 data2) {
    Elem_204f278 *ph = (Elem_204f278 *)mem;
    struct SndHeap *heap = ph->heap;
    if (heap == 0)
        return;
    NNS_SndHeapDestroy(heap);
    NNSSndSeqPlayer *q = ph->player;
    if (q != 0) {
        q->heap = 0;
        return;
    }
    int idx = ph->playerIndex;
    NNS_FndRemoveListObject(&sPlayer[idx].mLists[1], ph);
}

extern "C" void SetPlayerPriority(NNSSndSeqPlayer *player, int prio) {
    NNSFndList *list = (NNSFndList *)player->player;
    if (list != 0) {
        NNS_FndRemoveListObject(list, player);
        player->player = 0;
    }
    NNS_FndRemoveListObject(&sPrioList, player);
    player->prio = (unsigned char)prio;
    if (list != 0) {
        InsertPlayerList(list, player);
    }
    InsertPrioList(player);
}

extern "C" void NNSi_SndInitResourceMgr(void) {
    sChannelLock = 0;
    sCaptureLock = 0;
    sAlarmLock = 0;
}

extern "C" int NNS_SndLockChannel(unsigned int a) {
    if (a == 0) return 1;
    if (a & sChannelLock)
        return 0;
    SND_LockChannel(a, 0);
    sChannelLock |= a;
    return 1;
}

extern "C" void NNS_SndUnlockChannel(unsigned int mask) {
    if (mask == 0)
        return;
    SND_UnlockChannel(mask, 0);
    sChannelLock &= ~mask;
}

extern "C" int NNS_SndLockCapture(unsigned int mask) {
    if (mask & sCaptureLock)
        return 0;
    sCaptureLock = sCaptureLock | mask;
    return 1;
}

extern "C" void NNS_SndUnlockCapture(int mask) {
    sCaptureLock &= ~mask;
}

extern "C" int NNS_SndAllocAlarm(void) {
    int i;
    int mask = 1;
    unsigned int v = sAlarmLock;
    for (i = 0; i < 8; i++)
    {
        if ((v & mask) == 0)
        {
            sAlarmLock |= mask;
            return i;
        }
        mask <<= 1;
    }
    return -1;
}

extern "C" int NNS_SndFreeAlarm(unsigned int bit) {
    return sAlarmLock &= ~(1 << bit);
}

extern "C" void NNS_SndInit(void) {
    if (data_020a4d44 != 0)
        return;
    data_020a4d44 = 1;
    SND_Init();
    NNSi_SndInitResourceMgr();
    NNSi_SndCaptureInit();
    NNSi_SndPlayerInit();
}

extern "C" void NNS_SndMain(void) {
    while (SND_RecvCommandReply(0) != 0)
        ;
    NNSi_SndPlayerMain();
    NNSi_SndCaptureMain();
    func_020522c4();
    SND_FlushCommand(0);
}

extern "C" void BeginSleep(void) {
    u32 p;
    func_0205227c();
    NNSi_SndCaptureBeginSleep();
    SND_StopTimer(0, 0, 0, 0);
    p = SND_GetCurrentCommandTag();
    SND_FlushCommand(1);
    SND_WaitForCommandProc(p);
}

extern "C" void func_0204efe0(void) {
    NNSi_SndCaptureEndSleep();
    func_02052234();
}

extern "C" int func_0204ef9c(void *p) {
    if (((struct NitroFileHeader *)p)->magic != 0x4352414e)
        return 0;
    if (((struct NitroFileHeader *)p)->byteOrder != 0xfffe)
        return 0;
    return ((struct NitroFileHeader *)p)->version == 0x100;
}

extern "C" int func_0204ee40(struct S82 *self, char *name, char *p) {
    char *fatb;
    char *fntb;
    char *fimg;
    int i;
    char *cur;
    int n;
    char *fimg_off;

    fatb = 0;
    fntb = 0;
    fimg = 0;

    if (func_0204ef9c(p) == 0)
        return 0;

    cur = p + ((struct NitroFileHeader *)p)->headerSize;
    n = ((struct NitroFileHeader *)p)->numBlocks;
    i = 0;
    if (n > 0) {
        do {
            NitroBlockHeader *block = (NitroBlockHeader *)cur;
            unsigned int m = block->kind;
            switch (m) {
            case 0x46415442:
                fatb = cur;
                break;
            case 0x464e5442:
                fntb = cur;
                break;
            case 0x46494d47:
                fimg = cur;
                break;
            }
            cur += block->size;
            i++;
        } while (i < n);
    }

    FS_InitArchive(&self->arc);
    self->narc = p;
    self->f54 = (NarcFatBlock *)fatb;
    fimg_off = fimg + 8;
    self->f58 = (unsigned char *)fimg_off;

    if (FS_RegisterArchiveName(&self->arc, name, strlen(name)) == 0)
        return 0;

    NitroBlockHeader *fatHeader = (NitroBlockHeader *)fatb;
    NitroBlockHeader *fntHeader = (NitroBlockHeader *)fntb;
    if (FS_LoadArchive(&self->arc, (u32)fimg_off, (u32)(fatb + 0xc) - (u32)fimg_off,
                       fatHeader->size - 0xc, (u32)(fntb + 8) - (u32)fimg_off, fntHeader->size - 8,
                       0, 0))
        return 1;

    FS_ReleaseArchiveName(&self->arc);
    return 0;
}

extern "C" int func_0204ee10(struct S82 *self) {
    if (!FS_UnloadArchive(&self->arc))
        return 0;
    FS_ReleaseArchiveName(&self->arc);
    return 1;
}

extern "C" int func_0204ede8(struct S82 *o, unsigned int i) {
    int r = 0;
    if (i < o->f54->fileCount) {
        r = (int)o->f58 + o->f54->files[i].start;
    }
    return r;
}

extern "C" int func_0204eda4(int r0, char *a, u32 idx) {
    int r = 0;
    if (idx < ((struct S82 *)a)->f54->fileCount) {
        FSFileID id;
        id.arc = &((struct S82 *)a)->arc;
        id.file_id = idx;
        r = FS_OpenFileFast((FSFile *)r0, id);
    }
    return r;
}
