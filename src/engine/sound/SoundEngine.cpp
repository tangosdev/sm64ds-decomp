//cpp
/* SoundEngine, the sequencer half of the sound module.
 * arm9 .text 0x0204eda4..0x020524e4. The 32-record player table is
 * data_020a4d6c. The public Sound:: calls in the earlier sound TU
 * land in this range. No RTTI names a class for these workers.
 *
 * leftovers:
 * - The address-named workers stay func_0204* and func_0205*. There
 *   is no class to hang them on, and each one keeps the layout it is
 *   handed (Obj2, Elem, the HeapAllocator views). Those layouts disagree.
 * - IRQ::Disable and IRQ::Restore stay namespace-scope externs so
 *   they mangle as the IRQ TU defines them.
 * - Channel and heap records stay address arithmetic. A guessed
 *   sequencer member is not what these bodies share.
 */
#include "types.h"
#include "Sound.h"
#include "SoundPlayerRecord.h"
#include "NestedHeapIterator.h"
#include "HeapAllocator.h"
#include "SolidHeapAllocator.h"
#include "Heap.h"
#include "cstd.h"
#include "CP15.h"
#include "nnsys/snd/player.h"

namespace IRQ {
    /* Defined at 0x02059d1c/0x02059d30 (the IRQ TU); namespace-scope externs
       mangle exactly like the ROM's IRQ::Disable/IRQ::Restore. */
    u32 Disable(void);
    u32 Restore(u32 state);
}

struct Bank_205179c;
struct Elem_204f278;
struct Elem_2051f1c;
struct Entry_20516d4;
struct G_20503a4;
struct HeapAllocator_204f400;
struct HeapAllocator_204f460;
struct HeapAllocator_204f504;
struct HeapAllocator_205212c;
struct Node_204f63c;
struct Obj_2050fd4;
struct Obj_205212c;
struct S_204f720;

typedef short s16;
typedef void (*Fn)(void *a, int b, int c, int d);
typedef struct HeapOwner {
    HeapAllocator     *heap;
    NestedHeapIterator iter;
    void func_02051244();
    void func_020510a4(int);
} HeapOwner;
typedef struct AllocNode {
    u32 unk0;
    u32 unk4;
    u32 unk8;
    u32 unkc;
    u32 unk10;
    u32 unk14;
} AllocNode;
typedef int s32;
typedef struct {
    int f0;
    int pad4[2];
    int* dstc;
    int* dst10;
    int len;
    int f18;
    int pad1c;
    u32 f20;
    int f24;
    int f28;
} G_t;
typedef void (*FP)(void *, int, int *, int, int, int);
typedef struct {
    char pad[0xc];
    NestedHeapIterator iter; 
    char pad2[4];             
} IndexEntry;
typedef signed char s8;
/* A linear ramp: f0 start, f4 target, f8 step, fc step count (func_02052458). */
struct Obj2 { int f0; int f4; int f8; int fc; };
struct Elem {
    int first;   
    int x4;      
    int x8;      
};
struct Obj {
    char pad[0x1c];
    int count;           
    struct Elem arr[1];   
};
struct Ent {
  char pad[0xf0];
  int b0 : 1;
  int flag : 1;
  int brest : 30;
  char pad2[0x12c - 0xf4];
};
struct Bank { int f0; u16 ids[4]; };
struct A4 { int f0; u16 pad4; u8 b6; u8 b7; };
struct Entry7 {
    unsigned int f0;
    unsigned short ids[6];
};
struct P4 {
    int f0;
    short pad;
    unsigned char f6;
    unsigned char f7;
};
struct P5 {
    char pad[0x18];
    int off18;
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
struct Seq { int f0; u16 f4; };
struct Node {
    char pad8[8];
    int f8;      
    Fn fc;       
    int f10;     
    int f14;     
    char pad[8];
    char body;   
};
struct SolidHeapWithIter {
    void* allocator;       
    void* nhi_first;       
    void* nhi_last;        
    u16 id;                
    u16 nhi_offset;        
};
struct TwoInt { int a, b; };
/* The sound archive data_020a5bb8 points at, opened by func_02050f34.
   0x00..0x30 is the archive file's own header, read from offset 0; its
   (offset, size) pairs locate the blocks func_02050d54 loads. */
struct data_020a5bb8_t {
    /* 0x00 */ char pad_00[0x10];
    /* 0x10 */ int unk80Offset;
    /* 0x14 */ int unk80Size;
    /* 0x18 */ int infoOffset;
    /* 0x1c */ int infoSize;
    /* 0x20 */ int fileTableOffset;
    /* 0x24 */ int fileTableSize;
    /* 0x28 */ char pad_28[8];
    /* 0x30 */ char file[0x44];         /* NitroSDK FSFile */
    /* 0x74 */ struct TwoInt fileId;    /* filled from the path by func_0205d644 */
    /* 0x7c */ struct Hdr84 *fileTable; /* per-file records (func_020509b0, func_020509d8) */
    /* 0x80 */ void *unk80;             /* loaded only when func_02050d54's flag is set */
    /* 0x84 */ void *info;              /* the Info*Entry::GetWithID tables */
};
/* The INFO block data_020a5bb8_t::info points at. After its 8-byte block
   header come the offsets, from the block start, of its record tables; each
   lookup below reads one, bounds-checks the id against the table's count and
   returns the record at the id's offset (0 for none). */
struct SndInfoBlock {
    char header[8];
    int seqTable;     /* 0x08: Sound::InfoSequenceEntry::GetWithID */
    int seqArcTable;  /* 0x0c: func_02050c14 */
    int bankTable;    /* 0x10: Sound::InfoInstrumentBankEntry::GetWithID */
    int waveArcTable; /* 0x14: func_02050b4c; a bank's four ids index it */
    int playerTable;  /* 0x18: func_02050ae8; func_02052008 reads each record's
                         playable-seq count from its first byte */
    int groupTable;   /* 0x1c: func_02050a84 */
};
/* One file record: func_020509d8 reads `size` bytes from `offset` into the
   SDAT; func_02051634 sizes its buffer from func_02050a5c's `size`, and
   func_02050994 caches the loaded copy that func_020509b0 hands back. */
struct FatEntry {
    int offset; /* 0x0 */
    int size;   /* 0x4 */
    int loaded; /* 0x8 */
    int pad_c;
};
struct Hdr84 {
    char pad[8];
    int count;
    struct FatEntry entries[1]; /* 0xc, count of them */
};
struct G {
    s32 f0;        
    s32 f4;        
    char pad8[0x30];
    s32 f38;       
    char pad3c[0x20 - 0x3c + 0x38];  
};
enum Fmt {
    FMT_A = 0,
    FMT_B = 1
};
struct D {
    int f0;
    int f4;
    char p8[0x1c - 8];
    int f1c;
    int f20;
    int f24;
    int f28;
};
struct Q { int f0, f4, f8, fc, f10; };
struct T {
    int f0, f4;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
    int f1c, f20, f24, f28;
    int f2c;
    void (*f30)(int, int, int, int, int);
    int f34;
    void func_020500a4();
};
struct Inner {
    char pad8[8];
    int h8;
    char pad30[0x30 - 0xc];
    void (*h30)(unsigned int, unsigned int, unsigned int, int, int);
    int h34;
};
struct Outer {
    struct Inner *p0;
    int h4;
    char pad[4];
    int hc;
    int h10;
};
struct Obj1 { char pad[0x2c]; void *p; void *q; };
struct Obj0 { char pad[0xc]; int flag : 1; };
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
    /* 0x28 */ int f28;     /* alarm number from func_0204f0d8 */
    /* 0x2c */ int f2c;     /* first argument to SND_StartTimer / SND_StopTimer */
    /* 0x30 */ int chanCount;
    /* 0x34 */ unsigned char chans[1]; /* chanCount channel numbers */
    void func_0204fce8(void *);
};
struct E
{
  int field0;
  int field4;
};
struct LL {
  int lo;
  int hi;
};
struct E1 {
    /* 0x00 */ unsigned char pad[0x1c];
    /* 0x1c */ struct Obj2 fade;      /* volume ramp, 0x7f00 when reset */
    /* 0x2c */ unsigned char f2c;     /* 0 free, 1 started, 2 fading out */
    /* 0x2d */ unsigned char started; /* set once cmdTag has finished */
    /* 0x2e */ unsigned char unk2e;   /* last value sent with SND_PauseSeq */
    /* 0x2f */ unsigned char pad_2f;
    /* 0x30 */ u32 cmdTag; /* SND_GetCurrentCommandTag(), polled with SND_IsFinishedCommandTag */
    /* 0x34 */ unsigned short unk34;
    /* 0x36 */ unsigned char pad_36[6];
    /* 0x3c */ unsigned char f3c; /* player number */
    /* func_0204f364's key; func_0204f400 keeps the list sorted by it */
    /* 0x3d */ unsigned char priority;
    /* 0x3e */ short lastVolume;    /* last value sent with SND_SetPlayerVolume */
    /* 0x40 */ unsigned char unk40; /* 0..0x7f, indexes data_02086384 */
    /* 0x41 */ unsigned char unk41; /* 0..0x7f, indexes data_02086384 */
    /* 0x42 */ unsigned char pad_42[2];
    int func_0204f600(int, int, int);
    void func_0204f5a0(int);
};
struct E2 {
    unsigned char a[0xc];
    unsigned char b[0xc];
    int f18;
};
struct S {
    char pad0[4];
    u8 *ptr;
    char pad8[0x24];
    u8 active;
    char pad2d[0x17];
};
struct Inner83 {
    char pad[0x34];
    short h34;
    short pad36;
    short h38;
    short h3a;
};
struct HeapObj {
    /* 0x00 */ HeapAllocator *unk0;
    /* 0x04 */ HeapAllocator *heap;
    /* 0x08 */ HeapAllocator *unk8;
    /* 0x0c */ char pad_0c[0x20];
    /* 0x2c */ u8 state; /* 0 once released */
};
/* NitroSDK FSArchive (0x50 bytes, laid out in src/engine/fs/FS_Archive.cpp);
   this file only passes its address on. */
typedef struct FSArchive {
    char opaque[0x50];
} FSArchive;
typedef int (*FS_ARCHIVE_READ_FUNC)(FSArchive *p_arc, void *dst, u32 pos, u32 size);
typedef int (*FS_ARCHIVE_WRITE_FUNC)(FSArchive *p_arc, const void *src, u32 pos, u32 size);
struct S82 {
    /* 0x00 */ FSArchive arc;
    /* 0x50 */ char *narc;         /* the file func_0204ef9c checked for 'NARC' */
    /* 0x54 */ unsigned char *f54; /* its FATB block */
    /* 0x58 */ unsigned char *f58; /* its FIMG data */
};
struct Pair { void* a; u32 idx; };
struct HeapAllocator_205212c {
    char pad0[8];
    void *owner_8;    
};
struct Obj_205212c {
    /* 0x000 */ char pad0[0x44];
    /* 0x044 */ char file[0xc]; /* NitroSDK FSFile; u50 is its stat word */
    /* 0x050 */ u32 u50;
    /* 0x054 */ char pad1[0xc8 - 0x54];
    /* volume ramp func_020522c4 steps; its value >> 8 indexes data_02086384 */
    /* 0x0c8 */ struct Obj2 fade;
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
};
struct Elem_2051f1c { u8 pad0[4]; u16 f4; u8 pad6[2]; u8 f8; u8 f9; };
struct Bank_205179c { int f0; u16 f4; };
struct Entry_20516d4 {
    int field0;
    unsigned short ids[4];
};
struct Obj_2050fd4 {
    struct SolidHeapAllocator *heapAlloc;
    struct NestedHeapIterator iter;
};
struct G_20503a4 {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20, f24, f28, f2c, f30, f34;
    char f38[0x10];
    int f48, f4c;
};
struct S_204f720 {
    char pad34[0x34];
    short a;
    short pad36;
    short b;
};
struct Node_204f63c { unsigned short f0[4]; unsigned int f10[2]; unsigned int f18; };
struct HeapAllocator_204f504 {
    int unk0;
    int unk4;
    void *unk8;
    void *unkc;
};
struct HeapAllocator_204f460 {
    char pad[0x04];
    NestedHeapIterator *owner;
    char pad2[0x35];
    unsigned char sortKey;
    void func_0204f214(int);
};
struct HeapAllocator_204f400 {
    char pad[0x3d];
    unsigned char sortKey;
};
struct Elem_204f278 {
    /* 0x00 */ char pad_00[8];
    /* 0x08 */ int *heap;       /* func_0205130c's heap, freed by func_020512f0 */
    /* 0x0c */ int *owner;      /* claimant; its word 2 points back here */
    /* 0x10 */ int playerIndex; /* data_020a4d6c slot whose list holds this */
    void func_0204f278();
};
typedef struct Node Node;

extern "C" {
/* C-linkage callback types used by foreign decls. */
/* Foreign callees and data. */
extern struct Elem;
extern int FS_CloseFile(char *self);
extern void FS_InitFile(void *s);
extern int FS_ReadFile(int a, int b, int c);
extern struct HeapAllocator;
extern void MultiStore_Int(int val, int *dst, int len);
extern struct NestedHeapIterator;
extern struct Obj;
extern struct Obj1;
/* local extern: these callers run the constructor over storage that already
   exists (and func_02051064 returns its r0), which C++ cannot spell without a
   placement new the tree does not have. */
extern int _ZN18NestedHeapIteratorC1Ej(int a, int b);

extern int __aeabi_idiv(int a, int b);
extern unsigned int __aeabi_uidiv(unsigned int a, unsigned int b);
extern s16 data_02086384[];
extern int data_020a4d44;
extern unsigned int data_020a4d48;
extern unsigned int data_020a4d4c;
extern unsigned int data_020a4d50;
extern char data_020a4d54[];
extern NestedHeapIterator data_020a4d60;
extern SoundPlayerRecord data_020a4d6c[];
extern struct E1 data_020a50ec[];
extern int data_020a552c[];
extern int data_020a5538[];
extern struct E data_020a5578[];
extern int data_020a55f8;
extern int data_020a55fc;
extern int data_020a5600;
extern void *data_020a5614[]; /* the message array of data_020a5600 */
extern struct D data_020a5634;
extern int data_020a5684;
extern struct Q data_020a5718[];
extern char *data_020a5bb8;
extern NestedHeapIterator data_020a5bbc;
extern NestedHeapIterator data_020a5bc8;
extern struct Ent data_020a5bd4[];
extern void func_0204ebb8(void *alloc);
/* local extern: the sound thread is untyped storage here (void *) */
extern void OS_WakeupThreadDirect(void *thread);
/* local extern: as above */
extern void OS_CreateThread(void *thread, void *func, void *arg, void *stack,
                            unsigned int stackSize, unsigned int prio);
extern int OS_ReceiveMessage(void *mq, void **msg, int flags);
extern int OS_SendMessage(void *mq, void *msg, int flags);
extern void OS_InitMessageQueue(void *mq, void **msgArray, int msgCount);
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
extern void SND_SetupAlarm(s32 alarm, u32 tick, u32 period, void (*cb)(void *userData),
                           void *userData);
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
extern int FS_UnloadArchive(void *thiz);
extern int FS_LoadArchive(FSArchive *p_arc, u32 base, u32 fat, u32 fat_size, u32 fnt, u32 fnt_size,
                          FS_ARCHIVE_READ_FUNC read_func, FS_ARCHIVE_WRITE_FUNC write_func);
extern void FS_ReleaseArchiveName(void *thiz);
extern int FS_RegisterArchiveName(FSArchive *p_arc, const char *name, int name_len);
extern void FS_InitArchive(void *o);
extern int func_0205d368(void *o, int r1, int sel);
extern int func_0205d568(Node *node, int b, ...);
extern int func_0205d644(void *out_file_id, const char *path);

/* In-span forward declarations. */
extern int func_0204eda4(int r0, char* a, u32 idx);
extern int func_0204ede8(struct S82 *o, unsigned int i);
extern int func_0204ee10(void *thiz);
extern int func_0204ee40(struct S82 *self, char *name, char *p);
extern int func_0204ef9c(void* p);
extern void func_0204efe0(s8 levelID);
extern void func_0204effc(void);
extern void func_0204f03c(void);
extern void func_0204f070(void);
extern int func_0204f0b8(unsigned int bit);
extern int func_0204f0d8(void);
extern void func_0204f11c(int mask);
extern int func_0204f138(unsigned int mask);
extern void func_0204f15c(void *node);
extern int func_0204f194(unsigned int a);
extern void func_0204f1e8(void);
extern void _ZN12Elem_204f27813func_0204f278Ev(Elem_204f278 *); /* local extern: PMF -> int */
extern void func_0204f2d4(HeapObj* obj);
extern char* func_0204f364(int key);
extern void func_0204f3e0(unsigned char *c);
extern void func_0204f400(HeapAllocator_204f400* self);
extern void func_0204f460(NestedHeapIterator* self, HeapAllocator_204f460* node);
extern void func_0204f4bc(struct E1 *obj);
extern void* func_0204f504(int index, void* unk);
extern void func_0204f558(struct E1 *self, int val);
extern void _ZN2E113func_0204f5a0Ei(struct E1 *, int); /* local extern: PMF -> int */
extern void func_0204f630(void);
extern void *func_0204f63c(void **p, int idx, int r7);
extern void func_0204f6f8(struct Inner83 **o, short a, short b);
extern void func_0204f720(struct S_204f720 **pp, short v);
extern void func_0204f934(int **p);
extern void func_0204f94c(int *p);
extern void func_0204f958(int idx, int val);
extern void func_0204f9c4(int idx, int arg1);
extern int func_0204fa2c(int *p, int fade);
extern int func_0204fa3c(int owner, void *b, u32 c);
extern void func_0204fafc(void);
extern void func_0204fc40(void);
extern void func_0204fda4(struct C *c);
extern void func_0204fde8(struct C *obj);
extern void func_0204fe5c(char *obj, int arg1);
extern void func_0204fed0(struct C *c);
extern void func_0204ff48(struct C *p);
extern void func_0204ff9c(struct Obj0 *o);
extern void func_0204ffcc(char *self);
extern void func_02050008(struct Obj1 *o);
extern void func_02050038(void);
extern void _ZN1T13func_020500a4Ev(struct T *); /* local extern: PMF -> int */
extern void func_020501b8(void);
extern void func_02050258(void);
extern void func_020502b8(void);
extern int func_020503a4(int a, int b, int c, int d, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15);
extern void func_020506fc(int arg);
extern void func_02050798(void);
extern int func_020507e0(int a0, unsigned int a1, int a2, int a3, int a4, int a5, int a6);
extern void func_020508a0(void);
extern void func_02050950(void);
extern void func_02050970(int a0, int a1, int *p);
extern void func_0205097c(int a0, int a1, int *p);
extern void func_02050988(int a0, int a1, int *p);
extern void func_02050994(int i, int val);
extern int func_020509b0(unsigned int i);
extern int func_020509d8(unsigned int idx, int a1, unsigned int len, unsigned int off);
extern int func_02050a5c(unsigned int i);
extern int func_02050a84(unsigned int id);
extern int func_02050ae8(unsigned int id);
extern int func_02050b4c(unsigned int id);
extern int func_02050c14(unsigned int id);
extern void *func_02050cdc(int a, int idx);
extern int func_02050d2c(void);
extern int func_02050d3c(int v);
extern int func_02050d54(data_020a5bb8_t *obj, void *owner, int flag);
extern void func_02050f34(data_020a5bb8_t *self, int a1, int a2, int a3);
extern void func_02050fb0(void);
extern s32 func_02050fd4(struct Obj_2050fd4 *self);
extern s32 func_02051024(struct SolidHeapWithIter* self, void* allocator);
extern int func_02051064(int a);
extern u32 func_02051074(SolidHeapAllocator **pHeap);
extern s32 func_0205117c(struct SolidHeapWithIter* self);
extern void *func_020511d4(HeapOwner *owner, u32 elemSize, u32 param2, u32 param3, u32 param4);
extern void func_020512f0(int *p);
extern void *func_0205130c(unsigned int addr, unsigned int size);
extern void func_020513a8(char *a, int b, int c, int d);
extern void func_020513e4(char *a, int b, int c, int d);
extern void func_02051420(char *a, int b, int c, int d);
extern void func_02051454(int a, int b, unsigned int c);
extern int func_020514b4(int a, int b);
extern int func_02051514(int a, int b);
extern int func_02051574(int a, int b);
extern int func_020515d4(int a, int b);
extern void* func_02051634(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, void* owner);
extern int func_020516d4(unsigned int id, int flags, int c);
extern int func_0205179c(unsigned int id, int flags, int arg2);
extern int func_020518a0(void *a, void *b);
extern int func_020518dc(void *a, void *b);
extern int func_02051918(unsigned int id, int arg);
extern void *func_02051a28(int index, void *unk);
extern void func_02051a48(unsigned char *c, int off);
extern void func_02051a68(unsigned char *c, int off);
extern int func_02051a88(int a, int b);
extern int func_02051a98(void *p0, void *p1, unsigned int p2, void *p3, struct P4 *p4, struct P5 *p5, int p6, int p7);
extern int func_02051bd0(void **p, int a1, unsigned int a2, int a3, struct A4 *a4, int a5);
extern int func_02051e60(int a, int b, int c, int d, unsigned int arg5, int arg6);
extern int func_02051f1c(void *a0, u32 a1, int a2);
extern int func_02051fb4(void *thiz, unsigned int id);
extern int func_02052008(int arg);
extern void func_020520a4(HeapAllocator *node);
extern void func_020520dc(Obj_205212c *self);
extern void func_0205212c(Obj_205212c *self);
extern void func_02052208(void *obj);
extern void func_02052234(void);
extern void func_0205227c(void);
extern void func_020522c4(void);
extern struct Elem* func_020523e0(struct Obj* obj, int idx);
extern int func_02052420(int *o);
extern void func_02052438(char *self);
extern int func_02052458(int *p);
extern void func_02052494(struct Obj2 *o, int a, int b);
extern void func_020524c4(int *r0);
}

// @symbol func_020524c4
extern "C" void func_020524c4(int *r0){
  r0[1]=0;
  r0[0]=r0[1];
  r0[3]=0;
  r0[2]=r0[3];
}

// @symbol func_02052494
extern "C" void func_02052494(struct Obj2 *o, int a, int b) {
    o->f0 = ((extern int(*)(struct Obj2 *o, int a, int b))func_02052458)(o, a, b);
    o->f4 = a;
    o->fc = b;
    o->f8 = 0;
}

// @symbol func_02052458
extern "C" int func_02052458(int *p) {
    int den = p[3];
    int num = p[2];
    if (num >= den) return p[1];
    {
        int lo = p[0];
        int d = p[1] - lo;
        return lo + num * d / den;
    }
}

// @symbol func_02052438
extern "C" void func_02052438(char *self)
{
    struct Obj2 *ramp = (struct Obj2 *)self;
    if (ramp->f8 < ramp->fc) {
        ramp->f8 += 1;
    }
}

// @symbol func_02052420
extern "C" int func_02052420(int *o) {
    return o[2] >= o[3];
}

// @symbol func_020523e0
extern "C" struct Elem* func_020523e0(struct Obj* obj, int idx) {
    struct Elem* e;
    if (idx < 0) return 0;
    if ((u32)idx >= (u32)obj->count) return 0;
    e = &obj->arr[idx];
    if (e->first == -1) return 0;
    return e;
}

// @symbol func_020522c4
extern "C" void func_020522c4(void)
{
    int i;
    int t1;
    char *r = ((char*)data_020a5bd4);
    for (i = 0; i < 4; i++) {
        int flags = ((Obj_205212c *)r)->uf0;
        if ((flags << 31) >> 31) {
            if (((Obj_205212c *)r)->stopPending != 0) {
                func_02052208((void *)(r));
            } else {
                if ((flags << 29) >> 31) {
                    if (((Obj_205212c *)r)->startReady != 0) {
                        func_0204ffcc(r);
                        {
                            int *flagsp = (int *)&((Obj_205212c *)r)->uf0;
                            *flagsp |= 2;
                            *flagsp &= ~4;
                        }
                    }
                }
                if (((int)((Obj_205212c *)r)->uf0 << 30) >> 31) {
                    unsigned long t2;
                    func_02052438((char *)&((Obj_205212c *)r)->fade);
                    t1 = data_02086384[((Obj_205212c *)r)->volumeIndex];
                    t2 = data_02086384[func_02052458((int *)&((Obj_205212c *)r)->fade) >> 8];
                    t1 = t2 + t1;
                    if (t1 != ((Obj_205212c *)r)->lastVolume) {
                        func_0204fe5c(r, t1);
                        ((Obj_205212c *)r)->lastVolume = t1;
                    }
                    if (((int)((Obj_205212c *)r)->uf0 << 28) >> 31) {
                        if (func_02052420((int *)&((Obj_205212c *)r)->fade)) {
                            func_02052208((void *)(r));
                        }
                    }
                }
            }
        }
        r += 0x12c;
    }
}

// @symbol func_0205227c
extern "C" void func_0205227c(void){
  int i;
  for (i = 0; i < 4; i++){
    if (data_020a5bd4[i].flag) {
      ((void(*)(void* p))func_0204ff48)(&data_020a5bd4[i]);
    }
  }
}

// @symbol func_02052234
extern "C" void func_02052234(void){
  int i;
  for (i = 0; i < 4; i++){
    if (data_020a5bd4[i].flag) {
      ((void(*)(void* p))func_0204fed0)(&data_020a5bd4[i]);
    }
  }
}

// @symbol func_02052208
extern "C" void func_02052208(void *obj) {
    int v = ((Obj_205212c *)obj)->uf0;
    if ((v << 30) >> 31) ((void(*)(void *o))func_0204ff9c)(obj);
    ((void(*)(void *o))func_0205212c)(obj);
}

// @symbol func_0205212c
extern "C" void func_0205212c(Obj_205212c *self) {
    if (self->u114 != 0) {
        *self->u114 = 0;
        self->u114 = 0;
    }
    ((void(*)(void *self))func_020520dc)(self);
    int b = (int)((self->u50 & 0x10) != 0);
    if (b != 0) {
        ((void (*)(void *p))FS_CloseFile)(self->file);
    }
    u32 irq = IRQ::Disable();
    HeapAllocator_205212c *node = (HeapAllocator_205212c*)((HeapAllocator_205212c*)(data_020a5bc8.Next(0)));
    while (node != 0) {
        HeapAllocator_205212c *next = (HeapAllocator_205212c*)((HeapAllocator_205212c*)(data_020a5bc8.Next((HeapAllocator*)(node))));
        if (node->owner_8 == self) {
            data_020a5bc8.Remove((HeapAllocator*)(node));
            ((void(*)(HeapAllocator_205212c *node))func_020520a4)(node);
        }
        node = next;
    }
    IRQ::Restore(irq);
    u32 *p = &self->uf0;
    *p &= ~1u;
    *p &= ~4u;
    *p &= ~2u;
}

// @symbol func_020520dc
extern "C" void func_020520dc(Obj_205212c *self) {
    if (self->useCount == 0)
        return;

    self->useCount -= 1;

    if (self->useCount != 0)
        return;

    func_02050008((struct Obj1 *)self);
}

// @symbol func_020520a4
extern "C" void func_020520a4(HeapAllocator *node)
{
    u32 state;

    state = IRQ::Disable();
    ((NestedHeapIterator*)(&data_020a5bbc))->AddLast((HeapAllocator*)(node));
    IRQ::Restore(state);
}

// @symbol func_02052008
extern "C" int func_02052008(int arg)
{
    int i;
    func_02050d2c();
    for (i = 0; i < 0x20; i++) {
        unsigned char *p = ((unsigned char *(*)(unsigned int id))func_02050ae8)(i);
        if (p == 0)
            continue;
        Sound::Player::SetPlayableSeqCount(i, p[0]);
        if (*(int *)(p + 4) == 0)
            continue;
        if (arg == 0)
            continue;
        {
            int j;
            for (j = 0; j < p[0]; j++) {
                if (((int(*)(int a, int b, int c))func_0204fa3c)(i, arg, *(int *)(p + 4)) == 0)
                    return 0;
            }
        }
    }
    return 1;
}

// @symbol func_02051fb4
extern "C" int func_02051fb4(void *thiz, unsigned int id) {
    Sound::InfoSequenceEntry *e = (Sound::InfoSequenceEntry*)Sound::InfoSequenceEntry::GetWithID(id);
    if (!e) return 0;
    return ((int(*)(void *thiz, unsigned int a, unsigned int b, unsigned int c, void *entry, unsigned int id))func_02051bd0)(thiz, e->playerNumber, e->bankId, e->playerPriority, e, id);
}

// @symbol func_02051f1c
extern "C" int func_02051f1c(void *a0, u32 a1, int a2)
{
    int *r;
    void *obj;
    struct Elem_2051f1c *e;
    r = ((int *(*)(u32 id))func_02050c14)(a1);
    if (r == 0) return 0;
    obj = ((void *(*)(int x))func_020509b0)(r[0]);
    if (obj == 0) return 0;
    e = ((struct Elem_2051f1c *(*)(void *obj, int idx))func_020523e0)(obj, a2);
    if (e == 0) return 0;
    return ((int(*)(void *a, int b, int c, int d, void *e, void *f, int g, int h))func_02051a98)(a0, e->f9, e->f4, e->f8, e, obj, (int)a1, a2);
}

// @symbol func_02051e60
extern "C" int func_02051e60(int a, int b, int c, int d, unsigned int arg5, int arg6)
{
    int *p;
    void *elem;
    int r4;
    p = (int*)func_02050c14(arg5);
    if (p == 0) return 0;
    r4 = func_020509b0(p[0]);
    if (r4 == 0) return 0;
    elem = ((void *(*)(void *obj, int idx))func_020523e0)((void*)r4, arg6);
    if (elem == 0) return 0;
    if (d < 0) d = *(unsigned char*)((char*)elem + 8);
    if (c < 0) c = *(unsigned short*)((char*)elem + 4);
    if (b < 0) b = *(unsigned char*)((char*)elem + 9);
    return ((int(*)(int a, int b, int c, int d, void *elem, int r4, int arg5, int arg6))func_02051a98)(a, b, c, d, elem, r4, arg5, arg6);
}

// @symbol func_02051bd0
extern "C" int func_02051bd0(void **p, int a1, unsigned int a2, int a3, struct A4 *a4, int a5)
{
    struct Bank *bank;
    void *node;
    int r7;
    void *r6 = 0;
    int r5;
    int i;
    void *np;

    bank = (struct Bank *)Sound::InfoInstrumentBankEntry::GetWithID(a2);
    if (bank == 0) return 0;

    r7 = ((int(*)(void **p, int idx, int r7))func_0204f63c)(p, a1, a3);
    if (r7 == 0) return 0;

    r5 = func_020509b0(bank->f0);
    if (r5 == 0) {
        r6 = func_02051a28(a1, (void *)r7);
        if (r6 == 0) { ((void(*)(int r7))func_0204f630)(r7); return 0; }
        r5 = (int)((void *(*)(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, void *owner))func_02051634)(bank->f0, (unsigned int)&func_02051a68, 0, 0, r6);
        if (r5 == 0) { ((void(*)(int r7))func_0204f630)(r7); return 0; }
    }

    for (i = 0; i < 4; i++) {
        u16 v = bank->ids[i];
        if (v != 0xffff) {
            int q;
            node = ((void *(*)(unsigned int id))func_02050b4c)(v);
            if (node == 0) { ((void(*)(int r7))func_0204f630)(r7); return 0; }
            q = func_020509b0(*(unsigned int *)node);
            if (q == 0) {
                if (r6 == 0) {
                    r6 = func_02051a28(a1, (void *)r7);
                    if (r6 == 0) { ((void(*)(int r7))func_0204f630)(r7); return 0; }
                }
                q = (int)((void *(*)(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, void *owner))func_02051634)(*(unsigned int *)node, (unsigned int)&func_02051a48, 0, 0, r6);
                if (q == 0) { ((void(*)(int r7))func_0204f630)(r7); return 0; }
            }
            SND_AssignWaveArc((struct SNDBankData *)r5, i, (struct SNDWaveArc *)q);
        }
    }

    np = (void *)func_020509b0((unsigned int)a4->f0);
    if (np == 0) {
        if (r6 == 0) {
            r6 = func_02051a28(a1, (void *)r7);
            if (r6 == 0) { ((void(*)(int r7))func_0204f630)(r7); return 0; }
        }
        np = ((void *(*)(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, void *owner))func_02051634)((unsigned int)a4->f0, (unsigned int)&func_02051a88, 0, 0, r6);
        if (np == 0) { ((void(*)(int r7))func_0204f630)(r7); return 0; }
    }

    ((struct E1 *)r7)->func_0204f600((int)((char *)np + ((struct P5 *)np)->off18), 0, r5);
    NNS_SndPlayerSetInitialVolume((NNSSndHandle *)p, a4->b6);
    NNS_SndPlayerSetChannelPriority((NNSSndHandle *)p, a4->b7);
    ((void(*)(void *pp, int v))func_0204f720)(p, a5);
    return 1;
}

// @symbol func_02051a98
extern "C" int func_02051a98(void *p0, void *p1, unsigned int p2, void *p3,
                  struct P4 *p4, struct P5 *p5, int p6, int p7)
{
    struct Entry7 *e7;
    void *r6;
    void *r5;
    int i;

    e7 = (struct Entry7*)Sound::InfoInstrumentBankEntry::GetWithID(p2);
    if (e7 == 0)
        return 0;
    r6 = ((void *(*)(void *p, void *idx, void *arg))func_0204f63c)(p0, p1, p3);
    if (r6 == 0)
        return 0;
    r5 = ((void *(*)(unsigned int i))func_020509b0)(e7->f0);
    if (r5 == 0) {
        ((void(*)(void *p))func_0204f630)(r6);
        return 0;
    }
    for (i = 0; i < 4; i++) {
        unsigned int id = e7->ids[i];
        unsigned int *ent;
        void *node;
        if (id != 0xffff) {
            ent = ((unsigned int *(*)(unsigned int id))func_02050b4c)(id);
            if (ent == 0) {
                ((void(*)(void *p))func_0204f630)(r6);
                return 0;
            }
            node = ((void *(*)(unsigned int i))func_020509b0)(*ent);
            if (node == 0) {
                ((void(*)(void *p))func_0204f630)(r6);
                return 0;
            }
            SND_AssignWaveArc((struct SNDBankData *)r5, i, (struct SNDWaveArc *)node);
        }
    }
    ((struct E1 *)r6)->func_0204f600((int)((char *)p5 + p5->off18), p4->f0, (int)r5);
    NNS_SndPlayerSetInitialVolume((NNSSndHandle *)p0, p4->f6);
    NNS_SndPlayerSetChannelPriority((NNSSndHandle *)p0, p4->f7);
    ((void(*)(void *o, int a, int b))func_0204f6f8)(p0, p6, p7);
    return 1;
}

// @symbol func_02051a88
extern "C" int func_02051a88(int a, int b)
{
    return ((int (*)(int a, int b))SND_InvalidateSeqData)(a, a + b);
}

// @symbol func_02051a68
extern "C" void func_02051a68(unsigned char *c, int off)
{
    SND_InvalidateBankData(c, c + off);
    SND_DestroyBank((struct SNDBankData *)c);
}

// @symbol func_02051a48
extern "C" void func_02051a48(unsigned char *c, int off)
{
    SND_InvalidateWaveData(c, c + off);
    SND_DestroyWaveArc((struct SNDWaveArc *)c);
}

// @symbol func_02051a28
extern "C" void *func_02051a28(int index, void *unk)
{
    void *p = ((void *(*)(int index, void *unk))func_0204f504)(index, unk);
    if (p) {
        ((HeapOwner *)p)->func_02051244();
    }
    return p;
}

// @symbol func_02051918
extern "C" int func_02051918(unsigned int id, int arg)
{
    struct List *l;
    unsigned int i;
    struct Entry *e;

    l = (struct List *)func_02050a84(id);
    if (!l)
        return 0;

    i = 0;
    if (i < l->count) {
        e = l->entries;
        do {
            switch (e->kind) {
            case 0:
                if (func_0205179c(e->w, e->b1, arg) == 0)
                    return 0;
                break;
            case 3:
                if (func_020518dc((void *)e->w, (void *)arg) == 0)
                    return 0;
                break;
            case 1:
                if (((int(*)(unsigned int id, int flags, int arg2))func_020516d4)(e->w, e->b1, arg) == 0)
                    return 0;
                break;
            case 2:
                if (func_020518a0((void *)e->w, (void *)arg) == 0)
                    return 0;
                break;
            }
            i++;
            e++;
        } while (i < l->count);
    }

    return 1;
}

// @symbol func_020518dc
extern "C" int func_020518dc(void *a, void *b) {
    void *p = ((void *(*)(void *a))func_02050c14)(a);
    if (p == 0) return 0;
    return ((int(*)(void *a, void *b))func_02051574)(*(void **)p, b) != 0;
}

// @symbol func_020518a0
extern "C" int func_020518a0(void *a, void *b) {
    void *p = ((void *(*)(void *a))func_02050b4c)(a);
    if (p == 0) return 0;
    return ((int(*)(void *a, void *b))func_020514b4)(*(void **)p, b) != 0;
}

// @symbol func_0205179c
extern "C" int func_0205179c(unsigned int id, int flags, int arg2)
{
    struct Seq* seq;
    struct Bank_205179c* bank;
    int r = 0;
    int i;

    seq = (struct Seq*)Sound::InfoSequenceEntry::GetWithID(id);
    if (!seq) return r;
    bank = (struct Bank_205179c*)Sound::InfoInstrumentBankEntry::GetWithID(seq->f4);
    if (!bank) return r;

    if (flags & 2) {
        r = func_02051514(bank->f0, arg2);
        if (!r) return 0;
    }
    if (flags & 4) {
        for (i = 0; i < 4; i++) {
            u16 id2 = (&bank->f4)[i];
            if (id2 != 0xffff) {
                void* p = ((void*(*)(unsigned int id))func_02050b4c)(id2);
                int q;
                if (!p) return 0;
                q = func_020514b4(*(int*)p, arg2);
                if (!q) return 0;
                if (r != 0)
                    SND_AssignWaveArc((struct SNDBankData *)r, i, (struct SNDWaveArc *)q);
            }
        }
    }
    if (flags & 1) {
        int s = func_020515d4(seq->f0, arg2);
        if (!s) return 0;
    }
    return 1;
}

// @symbol func_020516d4
extern "C" int func_020516d4(unsigned int id, int flags, int c) {
    struct Entry_20516d4 *entry;
    int base;
    int i;

    base = 0;
    entry = (struct Entry_20516d4*)Sound::InfoInstrumentBankEntry::GetWithID(id);
    if (entry == 0) return 0;

    if (flags & 2) {
        base = func_02051514(entry->field0, c);
        if (base == 0) return 0;
    }

    if (flags & 4) {
        for (i = 0; i < 4; i++) {
            if (entry->ids[i] != 0xffff) {
                int *p;
                int arg;
                p = ((int *(*)(unsigned int id))func_02050b4c)(entry->ids[i]);
                if (p == 0) return 0;
                arg = func_020514b4(*p, c);
                if (arg == 0) return 0;
                if (base != 0) {
                    SND_AssignWaveArc((struct SNDBankData *)base, i, (struct SNDWaveArc *)arg);
                }
            }
        }
    }
    return 1;
}

// @symbol func_02051634
extern "C" void* func_02051634(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, void* owner)
{
    int r5 = ((extern int(*)(unsigned int i))func_02050a5c)(a0);
    void* r4;
    if (r5 == 0) return 0;
    r4 = ((extern void*(*)(void *owner, unsigned int elemSize, unsigned int p2, unsigned int p3, unsigned int p4))func_020511d4)(owner, r5 + 0x20, a1, a2, a3);
    if (r4 == 0) return 0;
    if (r5 != ((extern int(*)(unsigned int a, void* b, int c, int d))func_020509d8)(a0, r4, r5, 0)) return 0;
    CP15::FlushDataCache((unsigned int)r4, r5);
    return r4;
}

// @symbol func_020515d4
extern "C" int func_020515d4(int a, int b)
{
    int v = func_020509b0(a);
    if (v == 0) {
        v = ((int(*)(int a, void *cb, int d, int a2, int b))func_02051634)(a, (void *)func_02051420, func_02050d2c(), a, b);
        if (v != 0) {
            func_02050994(a, v);
        }
    }
    return v;
}

// @symbol func_02051574
extern "C" int func_02051574(int a, int b)
{
    int v = func_020509b0(a);
    if (v == 0) {
        v = ((int(*)(int a, void *cb, int d, int a2, int b))func_02051634)(a, (void *)func_02051420, func_02050d2c(), a, b);
        if (v != 0) {
            func_02050994(a, v);
        }
    }
    return v;
}

// @symbol func_02051514
extern "C" int func_02051514(int a, int b)
{
    int v = func_020509b0(a);
    if (v == 0) {
        v = ((int(*)(int a, void *cb, int d, int a2, int b))func_02051634)(a, (void *)func_020513e4, func_02050d2c(), a, b);
        if (v != 0) {
            func_02050994(a, v);
        }
    }
    return v;
}

// @symbol func_020514b4
extern "C" int func_020514b4(int a, int b)
{
    int v = func_020509b0(a);
    if (v == 0) {
        v = ((int(*)(int a, void *cb, int d, int a2, int b))func_02051634)(a, (void *)func_020513a8, func_02050d2c(), a, b);
        if (v != 0) {
            func_02050994(a, v);
        }
    }
    return v;
}

// @symbol func_02051454
extern "C" void func_02051454(int a, int b, unsigned int c) {
    int t;
    unsigned int irq;
    irq = IRQ::Disable();
    t = func_02050d3c(b);
    if (a == func_020509b0(c)) {
        func_02050994(c, 0);
    }
    func_02050d3c(t);
    IRQ::Restore(irq);
}

// @symbol func_02051420
extern "C" void func_02051420(char *a, int b, int c, int d)
{
    ((void(*)(char *a, int c, int d))func_02051454)(a, c, d);
    SND_InvalidateSeqData(a, a + b);
}

// @symbol func_020513e4
extern "C" void func_020513e4(char *a, int b, int c, int d) {
    ((extern void(*)(void *a, int c, int d))func_02051454)(a, c, d);
    SND_InvalidateBankData(a, a + b);
    SND_DestroyBank((struct SNDBankData *)a);
}

// @symbol func_020513a8
extern "C" void func_020513a8(char *a, int b, int c, int d) {
    ((extern void(*)(void *a, int c, int d))func_02051454)(a, c, d);
    SND_InvalidateWaveData(a, a + b);
    SND_DestroyWaveArc((struct SNDWaveArc *)a);
}

// @symbol func_0205130c
extern "C" void *func_0205130c(unsigned int addr, unsigned int size)
{
    unsigned int end = addr + size;
    unsigned int aligned = (addr + 3) & ~3u;
    void *alloc;
    if (aligned > end)
        return 0;
    /* The SolidHeapWithIter header func_02051024 fills sits at aligned; the
       solid heap takes the rest. */
    if (end - aligned < sizeof(struct SolidHeapWithIter))
        return 0;
    alloc = mHeap::Heap_t::CreateSolidHeapAllocator(
        (void *)(aligned + sizeof(struct SolidHeapWithIter)),
        (end - aligned) - sizeof(struct SolidHeapWithIter), 0);
    if (alloc == 0)
        return 0;
    if (((int(*)(void *self, void *alloc))func_02051024)((void *)aligned, alloc) != 0)
        return (void *)aligned;
    func_0204ebb8(alloc);
    return 0;
}

// @symbol func_020512f0
extern "C" void func_020512f0(int *p) {
    ((HeapOwner *)p)->func_02051244();
    ((void(*)(void *p))func_0204ebb8)((void*)*p);
}

// @symbol _ZN9HeapOwner13func_02051244Ev
void HeapOwner::func_02051244() {
    struct Node *r7;
    struct Node *r6;
    r7 = (struct Node *)(this->iter.Previous(0));
    if (r7 == 0) goto end;
    do {
        r6 = (struct Node*)(((NestedHeapIterator*)(r7))->Previous(0));
        while (r6 != 0) {
            if (r6->fc != 0) {
                r6->fc(&r6->body, r6->f8, r6->f10, r6->f14);
            }
            r6 = (struct Node*)(((NestedHeapIterator*)(r7))->Previous((HeapAllocator*)(r6)));
        }
        this->iter.Remove((HeapAllocator *)(r7));
        r7 = (struct Node *)(this->iter.Previous(0));
    } while (r7 != 0);
end:
    ((SolidHeapAllocator *)(this->heap))->Reset(3);
    func_02050fb0();
    ((void (*)(void *thiz))func_02050fd4)(this);
}

// @symbol func_020511d4
extern "C" void *func_020511d4(HeapOwner *owner, u32 elemSize, u32 param2, u32 param3, u32 param4)
{
    AllocNode *node;
    NestedHeapIterator *iter;

    node = (AllocNode *)((SolidHeapAllocator*)(owner->heap))->Allocate(((elemSize + 0x1f) & ~0x1fu) + 0x20, 0x20);
    if (node == 0)
        return 0;

    iter = (NestedHeapIterator*)(((NestedHeapIterator*)(&owner->iter))->Previous(0));
    node->unk8  = elemSize;
    node->unkc  = param2;
    node->unk10 = param3;
    node->unk14 = param4;
    ((NestedHeapIterator*)(iter))->AddLast((HeapAllocator *)node);

    return (void *)((u32)node + 0x20);
}

// @symbol func_0205117c
extern "C" s32 func_0205117c(struct SolidHeapWithIter* self)
{
    u16 id = self->id;
    void* alloc = self->allocator;
    if (!((SolidHeapAllocator*)(alloc))->SaveState((u32)id)) {
        return ~0;
    }
    if (((s32(*)(struct SolidHeapWithIter* self))func_02050fd4)(self)) {
        return (s32)((u32)self->id - 1);
    }
    ((SolidHeapAllocator*)(self->allocator))->LoadState(0);
    return ~0;
}

// @symbol _ZN9HeapOwner13func_020510a4Ei
void HeapOwner::func_020510a4(int n) {
    struct Node *a;
    struct Node *first;

    a = 0;
    if (n == 0) {
        this->func_02051244();
        return;
    }
    if (n < this->iter.mCount) {
        do {
            first = (struct Node *)(this->iter.Previous(0));
            a = (struct Node*)(((NestedHeapIterator*)(first))->Previous((HeapAllocator*)(a)));
            while (a != 0) {
                if (a->fc != 0) {
                    a->fc(&a->body, a->f8, a->f10, a->f14);
                }
                a = (struct Node*)(((NestedHeapIterator*)(first))->Previous((HeapAllocator*)(a)));
            }
            this->iter.Remove((HeapAllocator *)(first));
        } while (n < this->iter.mCount);
    }
    ((SolidHeapAllocator *)(this->heap))->LoadState(n);
    func_02050fb0();
    ((SolidHeapAllocator *)(this->heap))->SaveState(this->iter.mCount);
    ((void (*)(void *thiz))func_02050fd4)(this);
}

// @symbol func_02051074
extern "C" u32 func_02051074(SolidHeapAllocator **pHeap)
{
    u32 mem;
    mem = ((SolidHeapAllocator*)(*pHeap))->MemoryLeft(0x20);
    if (mem < 0x20)
        return 0;
    mem -= 0x20;
    mem &= ~0x1fu;
    return mem;
}

// @symbol func_02051064
extern "C" int func_02051064(int a)
{
    return _ZN18NestedHeapIteratorC1Ej(a, 0);
}

// @symbol func_02051024
extern "C" s32 func_02051024(struct SolidHeapWithIter* self, void* allocator)
{
    ((void(*)(void* self, u32 offset))_ZN18NestedHeapIteratorC1Ej)(&self->nhi_first, 0xc);
    self->allocator = allocator;
    if (((s32(*)(struct SolidHeapWithIter* self))func_02050fd4)(self)) {
        return 1;
    }
    return 0;
}

// @symbol func_02050fd4
extern "C" s32 func_02050fd4(struct Obj_2050fd4 *self) {
    struct HeapAllocator *r4 = (struct HeapAllocator *)((SolidHeapAllocator*)(self->heapAlloc))->Allocate(0x14, 4);
    if (!r4) {
        return 0;
    }
    ((extern void(*)(struct HeapAllocator *alloc))func_02051064)(r4);
    ((NestedHeapIterator*)(&self->iter))->AddLast((HeapAllocator*)(r4));
    return 1;
}

// @symbol func_02050fb0
extern "C" void func_02050fb0(void)
{
    u32 r = SND_GetCurrentCommandTag();
    SND_FlushCommand(1);
    SND_WaitForCommandProc(r);
}

// @symbol func_02050f34
extern "C" void func_02050f34(data_020a5bb8_t *self, int a1, int a2, int a3) {
    self->info = 0;
    self->fileTable = 0;
    self->unk80 = 0;
    ((int (*)(void *p, int b))func_0205d644)(&self->fileId, a1);
    FS_InitFile(self->file);
    ((void (*)(void *p, struct TwoInt v))func_0205d568)(self->file, self->fileId);
    ((void (*)(void *dst, void *src, int n))FS_ReadFile)(self->file, self, 0x30);
    if (a2 != 0)
        ((void(*)(void *self, int a, int b))func_02050d54)(self, a2, a3);
    data_020a5bb8 = (char *)self;
}

// @symbol func_02050d54
extern "C" int func_02050d54(data_020a5bb8_t *obj, void *owner, int flag) {
    unsigned int sz;
    int r;
    obj->info = ((void *(*)(void *owner, unsigned int elemSize, void *cb, void *obj,
                            unsigned int p4))func_020511d4)(owner, obj->infoSize,
                                                            (void *)func_02050988, obj, 0);
    if (obj->info == 0)
        return 0;
    obj->fileTable = (struct Hdr84 *)((void *(*)(void *owner, unsigned int elemSize, void *cb,
                                                 void *obj, unsigned int p4))func_020511d4)(
        owner, obj->fileTableSize, (void *)func_0205097c, obj, 0);
    if (obj->fileTable == 0)
        return 0;
    if (flag != 0 && (sz = obj->unk80Size) != 0) {
        obj->unk80 =
            ((void *(*)(void *owner, unsigned int elemSize, void *cb, void *obj,
                        unsigned int p4))func_020511d4)(owner, sz, (void *)func_02050970, obj, 0);
        if (obj->unk80 == 0)
            return 0;
    }
    if (func_0205d368(obj->file, obj->infoOffset, 0) == 0)
        return 0;
    r = ((int (*)(void *o, int a, int b))FS_ReadFile)(obj->file, (int)obj->info, obj->infoSize);
    if (r != obj->infoSize)
        return 0;
    if (func_0205d368(obj->file, obj->fileTableOffset, 0) == 0)
        return 0;
    r = ((int (*)(void *o, int a, int b))FS_ReadFile)(obj->file, (int)obj->fileTable,
                                                      obj->fileTableSize);
    if (r != obj->fileTableSize)
        return 0;
    if (flag != 0 && obj->unk80Size != 0) {
        if (func_0205d368(obj->file, obj->unk80Offset, 0) == 0)
            return 0;
        r = ((int (*)(void *o, int a, int b))FS_ReadFile)(obj->file, (int)obj->unk80,
                                                          obj->unk80Size);
        if (r != obj->unk80Size)
            return 0;
    }
    return 1;
}

// @symbol func_02050d3c
extern "C" int func_02050d3c(int v) { int old = ((int)data_020a5bb8); *(int*)(&data_020a5bb8) = (v); return old; }

// @symbol func_02050d2c
extern "C" int func_02050d2c(void) { return ((int)data_020a5bb8); }

// @symbol func_02050cdc
extern "C" void *func_02050cdc(int a, int idx)
{
    int *p;
    struct Obj *o;
    struct Elem *e;

    p = ((int *(*)(int a))func_02050c14)(a);
    if (p == 0)
        return 0;
    o = ((struct Obj *(*)(unsigned int i))func_020509b0)((unsigned int)*p);
    if (o == 0)
        return 0;
    e = ((struct Elem *(*)(struct Obj *obj, int idx))func_020523e0)(o, idx);
    if (e == 0)
        return 0;
    return (char *)e + 4;
}

// @symbol _ZN5Sound17InfoSequenceEntry9GetWithIDEj
Sound::InfoSequenceEntry *Sound::InfoSequenceEntry::GetWithID(u32 id)
{
    char* root = data_020a5bb8;
    char *sub = (char *)((data_020a5bb8_t *)root)->info;
    int off = ((struct SndInfoBlock *)sub)->seqTable;
    char* tbl = (off == 0) ? 0 : sub + off;
    if (tbl == 0) return 0;
    if ((int)id < 0) return 0;
    if (id >= *(unsigned*)tbl) return 0;
    int v = ((int*)(tbl + id * 4))[1];
    char *base = (char *)((data_020a5bb8_t *)root)->info;
    return (v == 0) ? 0 : (InfoSequenceEntry *)(base + v);
}

// @symbol func_02050c14
extern "C" int func_02050c14(unsigned int id)
{
    char* root = data_020a5bb8;
    char *sub = (char *)((data_020a5bb8_t *)root)->info;
    int off = ((struct SndInfoBlock *)sub)->seqArcTable;
    char* tbl = (off == 0) ? 0 : sub + off;
    if (tbl == 0) return 0;
    if ((int)id < 0) return 0;
    if (id >= *(unsigned*)tbl) return 0;
    int v = ((int*)(tbl + id * 4))[1];
    char *base = (char *)((data_020a5bb8_t *)root)->info;
    return (v == 0) ? 0 : (int)(base + v);
}

// @symbol _ZN5Sound23InfoInstrumentBankEntry9GetWithIDEj
Sound::InfoInstrumentBankEntry *Sound::InfoInstrumentBankEntry::GetWithID(u32 id)
{
    char* root = data_020a5bb8;
    char *sub = (char *)((data_020a5bb8_t *)root)->info;
    int off = ((struct SndInfoBlock *)sub)->bankTable;
    char* tbl = (off == 0) ? 0 : sub + off;
    if (tbl == 0) return 0;
    if ((int)id < 0) return 0;
    if (id >= *(unsigned*)tbl) return 0;
    int v = ((int*)(tbl + id * 4))[1];
    char *base = (char *)((data_020a5bb8_t *)root)->info;
    return (v == 0) ? 0 : (InfoInstrumentBankEntry *)(base + v);
}

// @symbol func_02050b4c
extern "C" int func_02050b4c(unsigned int id)
{
    char* root = data_020a5bb8;
    char *sub = (char *)((data_020a5bb8_t *)root)->info;
    int off = ((struct SndInfoBlock *)sub)->waveArcTable;
    char* tbl = (off == 0) ? 0 : sub + off;
    if (tbl == 0) return 0;
    if ((int)id < 0) return 0;
    if (id >= *(unsigned*)tbl) return 0;
    int v = ((int*)(tbl + id * 4))[1];
    char *base = (char *)((data_020a5bb8_t *)root)->info;
    return (v == 0) ? 0 : (int)(base + v);
}

// @symbol func_02050ae8
extern "C" int func_02050ae8(unsigned int id)
{
    char* root = data_020a5bb8;
    char *sub = (char *)((data_020a5bb8_t *)root)->info;
    int off = ((struct SndInfoBlock *)sub)->playerTable;
    char* tbl = (off == 0) ? 0 : sub + off;
    if (tbl == 0) return 0;
    if ((int)id < 0) return 0;
    if (id >= *(unsigned*)tbl) return 0;
    int v = ((int*)(tbl + id * 4))[1];
    char *base = (char *)((data_020a5bb8_t *)root)->info;
    return (v == 0) ? 0 : (int)(base + v);
}

// @symbol func_02050a84
extern "C" int func_02050a84(unsigned int id)
{
    char* root = data_020a5bb8;
    char *sub = (char *)((data_020a5bb8_t *)root)->info;
    int off = ((struct SndInfoBlock *)sub)->groupTable;
    char* tbl = (off == 0) ? 0 : sub + off;
    if (tbl == 0) return 0;
    if ((int)id < 0) return 0;
    if (id >= *(unsigned*)tbl) return 0;
    int v = ((int*)(tbl + id * 4))[1];
    char *base = (char *)((data_020a5bb8_t *)root)->info;
    return (v == 0) ? 0 : (int)(base + v);
}

// @symbol func_02050a5c
extern "C" int func_02050a5c(unsigned int i) {
    struct Hdr84 *h = ((data_020a5bb8_t *)data_020a5bb8)->fileTable;
    if (i >= (unsigned int)h->count) return 0;
    return h->entries[i].size;
}

// @symbol func_020509d8
extern "C" int func_020509d8(unsigned int idx, int a1, unsigned int len, unsigned int off)
{
    char *G = (char *)data_020a5bb8;
    struct Hdr84 *T = ((data_020a5bb8_t *)G)->fileTable;
    struct FatEntry *entry;
    unsigned int avail;

    if (idx >= (unsigned int)T->count)
        return -1;

    entry = &T->entries[idx];
    avail = (unsigned int)entry->size - off;
    if (len > avail)
        len = avail;

    if (((int (*)(int *o, int r1, int sel))func_0205d368)((int *)((data_020a5bb8_t *)G)->file,
                                                          entry->offset + off, 0) == 0)
        return -1;

    return ((int (*)(int *o, int a1, unsigned int len))FS_ReadFile)(
        (int *)((data_020a5bb8_t *)G)->file, a1, len);
}

// @symbol func_020509b0
extern "C" int func_020509b0(unsigned int i) {
    struct Hdr84 *h = ((data_020a5bb8_t *)data_020a5bb8)->fileTable;
    if (i >= (unsigned int)h->count) return 0;
    return h->entries[i].loaded;
}

// @symbol func_02050994
extern "C" void func_02050994(int i, int val) {
    ((data_020a5bb8_t *)data_020a5bb8)->fileTable->entries[i].loaded = val;
}

// @symbol func_02050988
extern "C" void func_02050988(int a0, int a1, int *p)
{
    p[33] = 0;
}

// @symbol func_0205097c
extern "C" void func_0205097c(int a0, int a1, int *p)
{
    p[31] = 0;
}

// @symbol func_02050970
extern "C" void func_02050970(int a0, int a1, int *p)
{
    p[32] = 0;
}

// @symbol func_02050950
extern "C" void func_02050950(void) { data_020a55fc = 0; *(int*)(&data_020a5634) = (0); }

// @symbol func_020508a0
extern "C" void func_020508a0(void)
{
    G_20503a4 *g = ((G_20503a4 *)&data_020a5634);
    s32 *sub;
    int v;
    if (g->f0 == 0)
        return;
    if (g->f4 != 0)
        return;
    sub = (s32 *)g->f38;
    ((void(*)(s32 *p))func_02052438)(sub);
    if (g->f48 != 0) {
        if (((int(*)(s32 *p))func_02052420)(sub) != 0) {
            func_020502b8();
            return;
        }
    }
    v = ((int(*)(s32 *p))func_02052458)(sub) >> 8;
    if (v == g->f4c)
        return;
    SND_SetChannelVolume(g->f20, v, 0);
    g->f4c = v;
}

// @symbol func_020507e0
extern "C" int func_020507e0(int a0, unsigned int a1, int a2, int a3, int a4, int a5, int a6)
{
    volatile int z;
    func_02050798();
    if (((int*)&data_020a5634)[0] != 0) return 0;
    z = 0;
    MultiStore_Int(z, (int *)a0, a1);
    CP15::FlushAndInvalidateDataCache((unsigned int)a0, a1);
    return ((int(*)(int a0, int a1, int a2, int a3, int s0, int s1, int s2, int s3, int s4, int s5, int s6, int s7, int s8, int s9, int s10))func_020503a4)(1, a0, a0 + (a1 >> 1), a1 >> 1,
                         a2, 0, 0, 1, a3, 0x7f, 0, 0x7f, a4, a5, a6);
}

// @symbol func_02050798
extern "C" void func_02050798(void){
  if (((int*)&data_020a5634)[0] == 0) return;
  if (((int*)&data_020a5634)[1] != 1) return;
  func_020502b8();
}

// @symbol func_020506fc
extern "C" void func_020506fc(int arg)
{
    if (data_020a55fc != 0)
        return;
    data_020a55f8 = 0;
    OS_InitMessageQueue(&data_020a5600, data_020a5614, 8);
    OS_CreateThread(&data_020a5684, (void *)func_02050038, 0, &data_020a5bb8, 0x400, arg);
    data_020a55fc = 1;
    OS_WakeupThreadDirect(&data_020a5684);
}

// @symbol func_020503a4
extern "C" int func_020503a4(int a, int b, int c, int d, int p5, int p6, int p7, int p8,
                  int p9, int p10, int p11, int p12, int p13, int p14, int p15)
{
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
    G_20503a4 *g;

    dd = (int)((d) & ~0ULL);
    pp8 = (int)((p8) & ~0ULL);
    pp10 = (int)((p10) & ~0ULL);

    handle = -1;
    g = ((G_20503a4*)&data_020a5634);
    CP15::FlushAndInvalidateDataCache(b, dd);
    CP15::FlushAndInvalidateDataCache(c, dd);
    stereo = (p5 == 1) ? 1 : 0;
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
        fmt1 = (int)(stereo != 0 ? FMT_A : FMT_B);
        fmt2 = (int)(stereo != 0 ? FMT_B : FMT_A);
        if (a != 2) mode = 0xa;
        if (p14 != 0) {
            handle = func_0204f0d8();
            if (handle < 0) return 0;
        }
        if (((int(*)(int a))func_0204f138)(3) == 0) {
            if (handle >= 0) ((void(*)(int a))func_0204f0b8)(handle);
            return 0;
        }
        if (((int(*)(int a))func_0204f194)(0xa) == 0) {
            if (handle >= 0) ((void(*)(int a))func_0204f0b8)(handle);
            ((void(*)(int a))func_0204f11c)(3);
            return 0;
        }
        qlen = (u32)dd >> 2;
        SND_SetupChannelPcm(1, fmt1, (void *)b, pp8 != 0 ? 1 : 2, 0, qlen, pp10, 0, align, p11);
        SND_SetupCapture(0, fmt2, (void *)b, qlen, pp8, p6, p7);
        SND_SetupChannelPcm(3, fmt1, (void *)c, pp8 != 0 ? 1 : 2, 0, qlen, pp10, 0, align, p12);
        SND_SetupCapture(1, fmt2, (void *)c, qlen, pp8, p6, p7);
        if (handle >= 0) {
            ((void (*)(int a, int b, int c, int d, void *e))SND_SetupAlarm)(
                handle, total + chunk, total, (int)_ZN1T13func_020500a4Ev, g);
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
        g->f30 = p14;
        g->f34 = p15;
        g->f4c = pp10;
        ((void(*)(void *a))func_020524c4)(g->f38);
        ((void(*)(void *a, int b, int c))func_02052494)(g->f38, pp10 << 8, 1);
        g->f48 = 0;
        return 1;
    }
}

// @symbol func_020502b8
extern "C" void func_020502b8(void)
{
    struct D *d = &data_020a5634;
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
    if (d->f24 != 0) func_0204f11c(d->f24);
    if (d->f1c != 0) func_0204f15c((void *)d->f1c);
    if (has) func_0204f0b8(d->f28);
    if (d->f4 == 1)
        SND_SetOutputSelector(0, 0, 0, 0);
    d->f0 = 0;
}

// @symbol func_02050258
extern "C" void func_02050258(void)
{
    struct D *G = &data_020a5634;
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

// @symbol func_020501b8
extern "C" void func_020501b8(void)
{
    G_t* g = ((G_t*)&data_020a5634);
    volatile int a;
    volatile int b;
    if (g->f0 == 0) return;
    g->f18 = 0;
    a = 0;
    ((extern void(*)(int val, int *dst, int len))MultiStore_Int)(a, g->dstc, g->len);
    b = 0;
    ((extern void(*)(int val, int *dst, int len))MultiStore_Int)(b, g->dst10, g->len);
    ((extern void (*)(unsigned int a, unsigned int b))CP15::FlushAndInvalidateDataCache)(
        (unsigned int)g->dstc, g->len);
    ((extern void (*)(unsigned int a, unsigned int b))CP15::FlushAndInvalidateDataCache)(
        (unsigned int)g->dst10, g->len);
    {
        int sh = g->f28;
        int mask = (sh >= 0) ? (1 << sh) : 0;
        SND_StartTimer(g->f20, g->f24, mask, 0);
    }
}

// @symbol _ZN1T13func_020500a4Ev
void T::func_020500a4() {
    int quot = this->f14 / this->f2c;
    int prod = quot * this->f18;
    int e6 = this->fc + prod;
    int e5 = this->f10 + prod;

    if (data_020a55fc != 0) {
        struct Q *slot = &data_020a5718[data_020a55f8];
        slot->f0 = (int)this;
        slot->f4 = quot;
        slot->f8 = prod;
        slot->fc = e6;
        slot->f10 = e5;
        OS_SendMessage(&data_020a5600, slot, 0);
        data_020a55f8 = data_020a55f8 + 1;
        if (data_020a55f8 >= 8) data_020a55f8 = 0;
    } else {
        CP15::InvalidateDataCache(e6, quot);
        CP15::InvalidateDataCache(e5, quot);
        this->f30(e6, e5, quot, this->f8, this->f34);
    }
    {
        int *pf18 = (int *)((int)&this->f18);
        *pf18 = *pf18 + 1;
        if (this->f18 >= this->f2c)
            this->f18 = 0;
    }
}

// @symbol func_02050038
extern "C" void func_02050038(void)
{
    struct Outer *r;
    void *out;
    for (;;) {
        OS_ReceiveMessage(&data_020a5600, &out, 1);
        r = (struct Outer *)out;
        ((void (*)(unsigned int a, unsigned int b))CP15::InvalidateDataCache)((unsigned int)r->hc,
                                                                              (unsigned int)r->h4);
        ((void (*)(unsigned int a, unsigned int b))CP15::InvalidateDataCache)((unsigned int)r->h10,
                                                                              (unsigned int)r->h4);
        r->p0->h30((unsigned int)r->hc, (unsigned int)r->h10, (unsigned int)r->h4, r->p0->h8, r->p0->h34);
    }
}

// @symbol func_02050008
extern "C" void func_02050008(struct Obj1 *o) {
    if (o->p == 0) {
        return;
    }
    ((extern void(*)(void *p))func_0204f15c)(o->p);
    o->p = 0;
    o->q = 0;
}

// @symbol func_0204ffcc
extern "C" void func_0204ffcc(char *self)
{
    struct C *c = (struct C *)self;
    SND_StartTimer(c->f2c, 0, 1 << c->f28, 0);
    c->fc |= 2;
}

// @symbol func_0204ff9c
extern "C" void func_0204ff9c(struct Obj0 *o) {
    if (o->flag) {
        ((extern void(*)(struct Obj0 *o))func_0204fde8)(o);
    }
}

// @symbol func_0204ff48
extern "C" void func_0204ff48(struct C *p) {
    if (!((p->fc << 30) >> 31))
        return;
    SND_StopTimer(p->f2c, 0, 1 << p->f28, 0);
    u32 x = SND_GetCurrentCommandTag();
    SND_FlushCommand(1);
    SND_WaitForCommandProc(x);
}

// @symbol func_0204fed0
extern "C" void func_0204fed0(struct C *c) {
    if (((c->fc << 30) >> 31) == 0) return;
    while (c->f20 != 0) {
        unsigned int saved = IRQ::Disable();
        c->func_0204fce8((void *)1);
        IRQ::Restore(saved);
    }
    SND_StartTimer(c->f2c, 0, 1 << c->f28, 0);
}

// @symbol func_0204fe5c
extern "C" void func_0204fe5c(char *obj, int arg1)
{
    struct C *c = (struct C *)obj;
    unsigned char new_var;
    int i = 0;
    c->volume = arg1;
    if (c->chanCount > 0) {
        if (1) {
        }
        do {
            unsigned char idx = new_var = c->chans[i];
            int r = SND_CalcChannelVolume(c->volume + data_020a5578[idx].field4);
            SND_SetChannelVolume(1 << idx, r & 0xff, r >> 8);
            i++;
        } while (i < c->chanCount);
    }
}

// @symbol func_0204fde8
extern "C" void func_0204fde8(struct C *obj) {
    if ((int)(obj->fc << 0x1e) >> 0x1f) {
        u32 p;
        int *flags = &obj->fc;
        SND_StopTimer(obj->f2c, 0, 1 << obj->f28, 0);
        *flags &= ~2;
        p = SND_GetCurrentCommandTag();
        SND_FlushCommand(1);
        SND_WaitForCommandProc(p);
    }
    ((void (*)(struct C *obj))func_0204fda4)(obj);
}

// @symbol func_0204fda4
extern "C" void func_0204fda4(struct C *c) {
    int* p;
    func_0204f0b8(c->f28);
    ((NestedHeapIterator*)&data_020a552c)->Remove((HeapAllocator*)(c));
    p = &c->fc;
    *p &= ~1;
    *p &= ~2;
}

// @symbol _ZN1C13func_0204fce8EPv
void C::func_0204fce8(void *arg) {
    int new_var;
    int i;
    int *pcount;
    int q =
        ((int (*)(unsigned int a, unsigned int b))__aeabi_uidiv)(this->bufSize, this->blockCount);
    int scaled = q * this->f20;
    for (i = 0; i < this->chanCount; i++) {
        new_var = ((LL *)data_020a5578)[this->chans[i]].lo;
        data_020a5538[i] = new_var + scaled;
    }

    this->callback(arg, this->chanCount, data_020a5538, q, this->unk8, this->unk1c);
    pcount = &this->f20;
    (*pcount)++;
    if (this->f20 >= this->blockCount) {
        this->f20 = 0;
    }
}

// @symbol func_0204fc40
extern "C" void func_0204fc40(void)
{
    int i;
    ((void(*)(void *thiz, unsigned int n))_ZN18NestedHeapIteratorC1Ej)(((char*)&data_020a4d60), 0x14);
    ((void(*)(void *thiz, unsigned int n))_ZN18NestedHeapIteratorC1Ej)(data_020a4d54, 0x14);
    for (i = 0; i < 0x10; i++) {
        data_020a50ec[i].f2c = 0;
        data_020a50ec[i].f3c = (unsigned char)i;
        ((NestedHeapIterator*)&data_020a4d54)->AddLast((HeapAllocator*)(&data_020a50ec[i]));
    }
    for (i = 0; i < 0x20; i++) {
        ((void(*)(void *thiz, unsigned int n))_ZN18NestedHeapIteratorC1Ej)(&((struct E2*)data_020a4d6c)[i], 0xc);
        ((void(*)(void *thiz, unsigned int n))_ZN18NestedHeapIteratorC1Ej)(((struct E2*)data_020a4d6c)[i].b, 0);
        ((struct E2*)data_020a4d6c)[i].f18 = 1;
    }
}

// @symbol func_0204fafc
extern "C" void func_0204fafc(void) {
    struct E1 *node;
    struct E1 *next;
    unsigned int mask;
    int v;

    mask = SND_GetPlayerStatus();
    node = (struct E1 *)(((NestedHeapIterator *)(((char *)&data_020a4d60)))->Next(0));
    if (node == 0) {
        return;
    }
    do {
        next = (struct E1 *)(((NestedHeapIterator *)(((char *)&data_020a4d60)))
                                 ->Next((HeapAllocator *)(node)));
        if (node->started == 0) {
            if (SND_IsFinishedCommandTag(node->cmdTag)) {
                node->started = 1;
            }
        }
        if (node->started != 0 && (mask & (1 << node->f3c)) == 0) {
            ((extern void (*)(char *p))func_0204f2d4)((char *)node);
        } else {
            int t1, t2, t3;
            ((extern int (*)(int *p))func_02052438)((int *)&node->fade);
            t1 = data_02086384[node->unk41];
            t2 = data_02086384[node->unk40];
            t3 = data_02086384[((extern int (*)(int *p))func_02052458)((int *)&node->fade) >> 8];
            v = t3 + (t2 + t1);
            if (v < -0x2d3) {
                v = -0x2d3;
            } else if (v > 0) {
                v = 0;
            }
            if (v != node->lastVolume) {
                SND_SetPlayerVolume(node->f3c, v);
                node->lastVolume = v;
            }
            if (node->f2c == 2) {
                if (((extern int (*)(int *o))func_02052420)((int *)&node->fade)) {
                    ((extern void (*)(unsigned char *c))func_0204f3e0)((unsigned char *)node);
                }
            }
        }
        node = next;
    } while (next != 0);
}

// @symbol _ZN5Sound6Player19SetPlayableSeqCountEii
void Sound::Player::SetPlayableSeqCount(int index, int count)
{
    data_020a4d6c[index].mPlayableSeqCount = (unsigned short)count;
}

// @symbol func_0204fa3c
extern "C" int func_0204fa3c(int owner, void *b, u32 c)
{
    char *r4;
    void *res;
    r4 = (char *)((void *(*)(void *a, u32 elemSize, u32 p2, u32 p3, u32 p4))func_020511d4)(
        b, c + 0x14, (u32)_ZN12Elem_204f27813func_0204f278Ev, 0, 0);
    if (!r4) return 0;
    ((Elem_204f278 *)r4)->owner = 0;
    ((Elem_204f278 *)r4)->playerIndex = owner;
    ((Elem_204f278 *)r4)->heap = 0;
    res = func_0205130c((unsigned int)(r4 + 0x14), c);
    if (!res) return 0;
    ((Elem_204f278 *)r4)->heap = (int *)res;
    data_020a4d6c[owner].mLists[1].AddLast((HeapAllocator *)(r4));
    return 1;
}

// @symbol func_0204fa2c
extern "C" int func_0204fa2c(int *p, int fade)
{
    return ((int (*)(u8 *thiz, int fade))_ZN2E113func_0204f5a0Ei)((u8 *)*p, fade);
}

// @symbol func_0204f9c4
extern "C" void func_0204f9c4(int idx, int arg1) {
    int i;
    struct S *p = ((struct S*)data_020a50ec);
    u8 *thiz = &((u8*)data_020a4d6c)[idx * 0x1c];
    for (i = 0; i < 16; i++, p++) {
        if (p->active != 0 && p->ptr == thiz) {
            ((struct E1 *)p)->func_0204f5a0(arg1);
        }
    }
}

// @symbol func_0204f958
extern "C" void func_0204f958(int idx, int val) {
    u8 *cur;
    u8 *next;
    char *iter = &((char*)data_020a4d6c)[idx * 0x1c];
    cur = (u8*)(((NestedHeapIterator*)(iter))->Next(0));
    if (cur == 0) return;
    do {
        next = (u8*)(((NestedHeapIterator*)(iter))->Next((HeapAllocator*)(cur)));
        ((void(*)(u8 *thiz, int val))func_0204f558)(cur, val);
        cur = next;
    } while (cur != 0);
}

// @symbol func_0204f94c
extern "C" void func_0204f94c(int *p)
{
    p[0] = 0;
}

// @symbol func_0204f934
extern "C" void func_0204f934(int **p) {
    int *q = *p;
    if (q != 0) {
        *q = 0;
        *p = 0;
    }
}

/* NitroSystem's NNS_SndPlayerSetVolume: if the handle holds a player, set
   its extVolume. This and the setters after it run in reverse snd_player.c
   source order, the order the Sonic Rush Adventure decomp lists them in;
   the handle and player types are nnsys/snd/player.h's. */
// @symbol NNS_SndPlayerSetVolume
extern "C" void NNS_SndPlayerSetVolume(NNSSndHandle *handle, int volume) {
    NNSSndSeqPlayer *player = handle->player;
    if (player)
        player->extVolume = (u8)volume;
}

/* NitroSystem's NNS_SndPlayerSetInitialVolume: if the handle holds a
   player, set its initVolume. */
// @symbol NNS_SndPlayerSetInitialVolume
extern "C" void NNS_SndPlayerSetInitialVolume(NNSSndHandle *handle, int volume) {
    NNSSndSeqPlayer *player = handle->player;
    if (player)
        player->initVolume = (u8)volume;
}

/* NitroSystem's NNS_SndPlayerMoveVolume: unless the player is fading out
   (status 2), fade its volume to targetVolume over `frames` frames
   (func_02052494, NNSi_SndFaderSet by shape, works in 8.8 fixed point). */
// @symbol NNS_SndPlayerMoveVolume
extern "C" void NNS_SndPlayerMoveVolume(NNSSndHandle *handle, int targetVolume, int frames) {
    NNSSndSeqPlayer *player = handle->player;
    if (player == 0)
        return;
    if (player->status == 2)
        return;
    ((void (*)(void *o, int a, int b))func_02052494)(&player->fader, targetVolume << 8, frames);
}

/* NitroSystem's NNS_SndPlayerSetChannelPriority: SND_SetPlayerChannelPriority
   on the player number. */
// @symbol NNS_SndPlayerSetChannelPriority
extern "C" void NNS_SndPlayerSetChannelPriority(NNSSndHandle *handle, int priority) {
    NNSSndSeqPlayer *player = handle->player;
    if (player)
        SND_SetPlayerChannelPriority(player->playerNo, priority);
}

/* NitroSystem's NNS_SndPlayerSetTrackMute: if the handle holds a player,
   mute (flag 1) or unmute the tracks in trackBitMask through
   SND_SetTrackMute on its player number. */
// @symbol NNS_SndPlayerSetTrackMute
extern "C" void NNS_SndPlayerSetTrackMute(NNSSndHandle *handle, int trackBitMask, int flag) {
    NNSSndSeqPlayer *player = handle->player;
    if (player)
        SND_SetTrackMute(player->playerNo, trackBitMask, flag);
}

/* NitroSystem's NNS_SndPlayerSetTrackVolume: if the handle holds a player,
   set the tracks in trackBitMask to volume (0..127) through
   SND_SetTrackVolume, converted by SND_CalcDecibel, which
   is the SNDi_DecibelTable lookup data_02086384[volume]. */
// @symbol NNS_SndPlayerSetTrackVolume
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
// @symbol NNS_SndPlayerSetTrackPitch
extern "C" void NNS_SndPlayerSetTrackPitch(NNSSndHandle *handle, int trackBitMask, int pitch) {
    NNSSndSeqPlayer *player = handle->player;
    if (player)
        SND_SetTrackPitch(player->playerNo, trackBitMask, pitch);
}

/* NitroSystem's NNS_SndPlayerSetTrackPan: SND_SetTrackPan (SNDTrack.ext_pan
   at 9). */
// @symbol NNS_SndPlayerSetTrackPan
extern "C" void NNS_SndPlayerSetTrackPan(NNSSndHandle *handle, int trackBitMask, int pan) {
    NNSSndSeqPlayer *player = handle->player;
    if (player)
        SND_SetTrackPan(player->playerNo, trackBitMask, pan);
}

/* NitroSystem's NNS_SndPlayerSetTrackModDepth: SND_SetTrackModDepth
   (SNDTrack.mod.depth at 0x1a). The source puts
   NNS_SndPlayerSetTrackPanRange between this and the pan setter; it is
   absent here (the two are adjacent), presumably dead-stripped. */
// @symbol NNS_SndPlayerSetTrackModDepth
extern "C" void NNS_SndPlayerSetTrackModDepth(NNSSndHandle *handle, int trackBitMask, int depth) {
    NNSSndSeqPlayer *player = handle->player;
    if (player)
        SND_SetTrackModDepth(player->playerNo, trackBitMask, depth);
}

/* NitroSystem's NNS_SndPlayerSetTrackModSpeed: SND_SetTrackModSpeed
   (SNDTrack.mod.speed at 0x19). */
// @symbol NNS_SndPlayerSetTrackModSpeed
extern "C" void NNS_SndPlayerSetTrackModSpeed(NNSSndHandle *handle, int trackBitMask, int speed) {
    NNSSndSeqPlayer *player = handle->player;
    if (player)
        SND_SetTrackModSpeed(player->playerNo, trackBitMask, speed);
}

/* NitroSystem's NNS_SndPlayerSetTempoRatio: SND_SetPlayerTempoRatio (an
   SNDi_SetPlayerParam write of SNDPlayer.tempo_ratio at 0x1a). */
// @symbol NNS_SndPlayerSetTempoRatio
extern "C" void NNS_SndPlayerSetTempoRatio(NNSSndHandle *handle, int ratio) {
    NNSSndSeqPlayer *player = handle->player;
    if (player)
        SND_SetPlayerTempoRatio(player->playerNo, ratio);
}

// @symbol func_0204f720
extern "C" void func_0204f720(struct S_204f720 **pp, short v) {
    struct S_204f720 *p = *pp;
    if (p != 0) {
        p->a = 1;
        p = *pp;
        p->b = v;
    }
}

// @symbol func_0204f6f8
extern "C" void func_0204f6f8(struct Inner83 **o, short a, short b) {
    if (*o == 0) return;
    (*o)->h34 = 2;
    (*o)->h38 = a;
    (*o)->h3a = b;
}

// @symbol func_0204f63c
extern "C" void *func_0204f63c(void **p, int idx, int r7)
{
    SoundPlayerRecord *node = &data_020a4d6c[idx];
    void *prev = p[0];
    void **r5;
    if (prev != 0) ((void(*)(void **p))func_0204f934)(p);
    if (node->mLists[0].mCount >= node->mPlayableSeqCount) {
        unsigned char *it = (unsigned char *)(node->mLists[0].Next(0));
        if (it == 0) return 0;
        if (r7 < (int)((struct E1 *)it)->priority)
            return 0;
        func_0204f3e0(it);
    }
    r5 = (void**)((void *(*)(int x))func_0204f364)(r7);
    if (r5 == 0) return 0;
    ((void(*)(void *node, void *r5))func_0204f460)(node, r5);
    r5[0] = p;
    p[0] = r5;
    return r5;
}

// @symbol func_0204f630
extern "C" void func_0204f630(void) {
    ((void(*)(void))func_0204f2d4)();
}

// @symbol _ZN2E113func_0204f600Eiii
int E1::func_0204f600(int seqData, int seqOffset, int bank) {
    SND_StartSeq(this->f3c, (const void *)seqData, seqOffset, (struct SNDBankData *)bank);
    ((void (*)(void *thiz))func_0204f4bc)(this);
    this->cmdTag = SND_GetCurrentCommandTag();
    this->f2c = 1;
    return 1;
}

// @symbol _ZN2E113func_0204f5a0Ei
void E1::func_0204f5a0(int arg1) {
    struct E1 *e = (struct E1 *)this;
    if (this == 0)
        return;
    if (e->f2c == 0)
        return;
    if (arg1 == 0) {
        ((void (*)(void *c))func_0204f3e0)(this);
        return;
    }
    func_02052494(&e->fade, 0, arg1);
    ((HeapAllocator_204f460 *)this)->func_0204f214(0);
    e->f2c = 2;
}

// @symbol func_0204f558
extern "C" void func_0204f558(struct E1 *self, int val) {
    if (self == 0)
        return;
    if (val == self->unk2e)
        return;
    SND_PauseSeq(self->f3c, val);
    self->unk2e = (unsigned char)val;
}

// @symbol func_0204f504
extern "C" void* func_0204f504(int index, void* unk) {
    IndexEntry* entry = &((IndexEntry*)data_020a4d6c)[index];
    HeapAllocator_204f504* first = (HeapAllocator_204f504*)(((NestedHeapIterator*)(&entry->iter))->Next(0));
    if (first == 0)
        return 0;
    ((NestedHeapIterator*)(&entry->iter))->Remove((HeapAllocator*)(first));
    first->unkc = unk;
    ((HeapAllocator_204f504**)((char*)unk))[2] = first; /* unk[8] = first */
    return first->unk8;
}

// @symbol func_0204f4bc
extern "C" void func_0204f4bc(struct E1 *obj) {
    obj->unk2e = 0;
    obj->started = 0;
    obj->unk34 = 0;
    obj->lastVolume = 0;
    obj->unk40 = 0x7f;
    obj->unk41 = 0x7f;
    ((void (*)(void *p))func_020524c4)(&obj->fade);
    ((void (*)(void *o, int a, int b))func_02052494)(&obj->fade, 0x7f00, 1);
}

// @symbol func_0204f460
extern "C" void func_0204f460(NestedHeapIterator* self, HeapAllocator_204f460* node) {
    HeapAllocator_204f460* cur;
    cur = (HeapAllocator_204f460*)(((NestedHeapIterator*)(self))->Next(0));
    if (cur != 0) {
        for (;;) {
            if (node->sortKey < cur->sortKey)
                break;
            cur = (HeapAllocator_204f460*)(((NestedHeapIterator*)(self))->Next((HeapAllocator*)(cur)));
            if (cur == 0)
                break;
        }
    }
    ((NestedHeapIterator*)(self))->AddAt((HeapAllocator*)(cur), (HeapAllocator*)(node));
    node->owner = self;
}

// @symbol func_0204f400
extern "C" void func_0204f400(HeapAllocator_204f400* self) {
    HeapAllocator_204f400* cur;
    cur = (HeapAllocator_204f400*)(((NestedHeapIterator*)(((NestedHeapIterator*)&data_020a4d60)))->Next(0));
    if (cur != 0) {
        for (;;) {
            if (self->sortKey < cur->sortKey)
                break;
            cur = (HeapAllocator_204f400*)(((NestedHeapIterator*)(((NestedHeapIterator*)&data_020a4d60)))->Next((HeapAllocator*)(cur)));
            if (cur == 0)
                break;
        }
    }
    ((NestedHeapIterator*)(((NestedHeapIterator*)&data_020a4d60)))->AddAt((HeapAllocator*)(cur), (HeapAllocator*)(self));
}

// @symbol func_0204f3e0
extern "C" void func_0204f3e0(unsigned char *c)
{
    SND_StopSeq(((struct E1 *)c)->f3c);
    ((void(*)(void *c))func_0204f2d4)(c);
}

// @symbol func_0204f364
extern "C" char* func_0204f364(int key)
{
    char* node = (char*)(((NestedHeapIterator*)&data_020a4d54)->Next(0));
    if (node == 0) {
        node = (char*)(((NestedHeapIterator*)(((char*)&data_020a4d60)))->Next(0));
        if (key < ((struct E1 *)node)->priority) {
            return 0;
        }
        ((extern void(*)(unsigned char* c))func_0204f3e0)((unsigned char*)node);
    }
    ((NestedHeapIterator*)&data_020a4d54)->Remove((HeapAllocator*)(node));
    ((struct E1 *)node)->priority = key;
    ((extern void(*)(char* self))func_0204f400)(node);
    return node;
}

// @symbol func_0204f2d4
extern "C" void func_0204f2d4(HeapObj* obj) {
    HeapAllocator* p0 = obj->unk0;
    if (p0 != 0) {
        *(HeapAllocator**)p0 = 0;
        obj->unk0 = 0;
    }
    HeapAllocator* heap = obj->heap;
    ((NestedHeapIterator*)(heap))->Remove((HeapAllocator*)obj);
    obj->heap = 0;
    if (obj->unk8 != 0) {
        ((NestedHeapIterator*)((HeapAllocator*)((u8*)heap + 0xc)))->AddLast((HeapAllocator*)(obj->unk8));
        *(HeapAllocator**)((u8*)obj->unk8 + 0xc) = 0;
        obj->unk8 = 0;
    }
    ((NestedHeapIterator*)(((HeapAllocator*)&data_020a4d60)))->Remove((HeapAllocator*)obj);
    ((NestedHeapIterator*)(((HeapAllocator*)data_020a4d54)))->AddLast((HeapAllocator*)obj);
    obj->state = 0;
}

// @symbol _ZN12Elem_204f27813func_0204f278Ev
void Elem_204f278::func_0204f278() {
    int *p = this->heap;
    if (p == 0) return;
    func_020512f0(p);
    int *q = this->owner;
    if (q != 0) {
        q[2] = 0;
        return;
    }
    int idx = this->playerIndex;
    data_020a4d6c[idx].mLists[1].Remove((HeapAllocator *)(this));
}

// @symbol _ZN21HeapAllocator_204f46013func_0204f214Ei
void HeapAllocator_204f460::func_0204f214(int b) {
    void *node = this->owner;
    if (node != 0) {
        ((NestedHeapIterator *)(node))->Remove((HeapAllocator *)(this));
        this->owner = 0;
    }
    ((NestedHeapIterator *)(((char *)&data_020a4d60)))->Remove((HeapAllocator *)(this));
    this->sortKey = (unsigned char)b;
    if (node != 0) {
        ((void (*)(void *node, void *self))func_0204f460)(node, this);
    }
    ((void (*)(void *self))func_0204f400)(this);
}

// @symbol func_0204f1e8
extern "C" void func_0204f1e8(void) {
    data_020a4d50 = 0;
    data_020a4d48 = 0;
    data_020a4d4c = 0;
}

// @symbol func_0204f194
extern "C" int func_0204f194(unsigned int a) {
    if (a == 0) return 1;
    if (a & data_020a4d50) return 0;
    SND_LockChannel(a, 0);
    data_020a4d50 |= a;
    return 1;
}

// @symbol func_0204f15c
extern "C" void func_0204f15c(void *node)
{
    if (node == 0)
        return;
    SND_UnlockChannel((u32)node, 0);
    data_020a4d50 &= ~(unsigned int)node;
}

// @symbol func_0204f138
extern "C" int func_0204f138(unsigned int mask)
{
    if (mask & data_020a4d48)
        return 0;
    data_020a4d48 = data_020a4d48 | mask;
    return 1;
}

// @symbol func_0204f11c
extern "C" void func_0204f11c(int mask) {
    data_020a4d48 &= ~mask;
}

// @symbol func_0204f0d8
extern "C" int func_0204f0d8(void)
{
    int i;
    int mask = 1;
    unsigned int v = data_020a4d4c;
    for (i = 0; i < 8; i++)
    {
        if ((v & mask) == 0)
        {
            data_020a4d4c |= mask;
            return i;
        }
        mask <<= 1;
    }
    return -1;
}

// @symbol func_0204f0b8
extern "C" int func_0204f0b8(unsigned int bit)
{
    return data_020a4d4c &= ~(1 << bit);
}

// @symbol func_0204f070
extern "C" void func_0204f070(void){
  if (data_020a4d44 != 0) return;
  data_020a4d44 = 1;
  SND_Init();
  func_0204f1e8();
  func_02050950();
  func_0204fc40();
}

// @symbol func_0204f03c
extern "C" void func_0204f03c(void)
{
    while (SND_RecvCommandReply(0) != 0)
        ;
    func_0204fafc();
    func_020508a0();
    func_020522c4();
    SND_FlushCommand(0);
}

// @symbol func_0204effc
extern "C" void func_0204effc(void) {
    u32 p;
    func_0205227c();
    func_02050258();
    SND_StopTimer(0, 0, 0, 0);
    p = SND_GetCurrentCommandTag();
    SND_FlushCommand(1);
    SND_WaitForCommandProc(p);
}

// @symbol func_0204efe0
extern "C" void func_0204efe0(s8 levelID)
{
    s8 courseID = ((s8(*)(s8 levelID))func_020501b8)(levelID);
    ((void(*)(s8 courseID))func_02052234)(courseID);
}

/* The Nitro archive file header func_0204ef9c validates ('NARC', byte-order
   mark 0xfffe, version 1.0). func_0204ee40 walks numBlocks blocks starting
   headerSize bytes in. */
struct NarcHeader {
    u32 magic;      /* 0x00 */
    u16 byteOrder;  /* 0x04 */
    u16 version;    /* 0x06 */
    u32 fileSize;   /* 0x08 */
    u16 headerSize; /* 0x0c */
    u16 numBlocks;  /* 0x0e */
};

// @symbol func_0204ef9c
extern "C" int func_0204ef9c(void* p) {
    if (((struct NarcHeader *)p)->magic != 0x4352414e)
        return 0;
    if (((struct NarcHeader *)p)->byteOrder != 0xfffe)
        return 0;
    return ((struct NarcHeader *)p)->version == 0x100;
}

// @symbol func_0204ee40
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

    if (((int(*)(void *p))func_0204ef9c)(p) == 0)
        return 0;

    cur = p + ((struct NarcHeader *)p)->headerSize;
    n = ((struct NarcHeader *)p)->numBlocks;
    i = 0;
    if (n > 0) {
        do {
            unsigned int m = *(unsigned int *)cur;
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
            cur += *(int *)(cur + 4);
            i++;
        } while (i < n);
    }

    FS_InitArchive(self);
    self->narc = p;
    self->f54 = (unsigned char *)fatb;
    fimg_off = fimg + 8;
    self->f58 = (unsigned char *)fimg_off;

    if (FS_RegisterArchiveName(&self->arc, name, cstd::strlen(name)) == 0)
        return 0;

    if (FS_LoadArchive(&self->arc, (u32)fimg_off, (u32)(fatb + 0xc) - (u32)fimg_off,
                       *(u32 *)(fatb + 4) - 0xc, (u32)(fntb + 8) - (u32)fimg_off,
                       *(u32 *)(fntb + 4) - 8, 0, 0))
        return 1;

    FS_ReleaseArchiveName(self);
    return 0;
}

// @symbol func_0204ee10
extern "C" int func_0204ee10(void *thiz)
{
    if (!FS_UnloadArchive(thiz))
        return 0;
    FS_ReleaseArchiveName(thiz);
    return 1;
}

// @symbol func_0204ede8
extern "C" int func_0204ede8(struct S82 *o, unsigned int i) {
    unsigned char *base = o->f54;
    int r = 0;
    if (i < *(unsigned short *)(base + 8)) {
        r = (int)o->f58 + *(int *)(base + i * 8 + 0xc);
    }
    return r;
}

// @symbol func_0204eda4
extern "C" int func_0204eda4(int r0, char* a, u32 idx) {
    int r = 0;
    if (idx < *(u16 *)(((struct S82 *)a)->f54 + 8)) {
        struct Pair p;
        p.a = a;
        p.idx = idx;
        r = ((int(*)(int r0, struct Pair p))func_0205d568)(r0, p);
    }
    return r;
}

