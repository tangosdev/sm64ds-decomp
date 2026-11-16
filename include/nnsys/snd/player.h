#ifndef NNSYS_SND_PLAYER_H
#define NNSYS_SND_PLAYER_H

/*
 * NitroSystem's sequence-player handle and the player it points at, laid
 * out as the Sonic Rush Adventure decomp spells NNS snd_player's types.
 * SoundEngine.cpp's sixteen players at sSeqPlayer are this type (fader
 * at 0x1c, status at 0x2c, player number at 0x3c, initial volume at 0x40).
 * The record types it nests are opaque here past their size.
 */

#include "types.h"
#include "nnsys/fnd.h"
#include "nnsys/snd/fader.h"

#ifdef __cplusplus
struct Elem;
struct ElemTable;
struct SNDBankData;
namespace Sound {
struct InfoSequenceEntry;
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NNSSndSeqPlayer {
    struct NNSSndHandle *handle; /* 0x00 */
    void *player;                /* 0x04 -- NNSSndPlayer */
    void *heap;                  /* 0x08 -- NNSSndPlayerHeap */
    NNSFndLink playerLink;       /* 0x0c */
    NNSFndLink prioLink;         /* 0x14 */
    NNSSndFader fader;           /* 0x1c */
    u8 status;                   /* 0x2c */
    u8 startFlag;                /* 0x2d */
    u8 pauseFlag;                /* 0x2e */
    u8 prepareFlag;              /* 0x2f */
    u32 commandTag;              /* 0x30 */
    u16 seqType;                 /* 0x34 */
    u16 pad2;                    /* 0x36 */
    u16 seqNo;                   /* 0x38 */
    u16 seqArcIndex;             /* 0x3a */
    u8 playerNo;                 /* 0x3c */
    u8 prio;                     /* 0x3d */
    s16 volume;                  /* 0x3e */
    u8 initVolume;               /* 0x40 */
    u8 extVolume;                /* 0x41 */
    u16 pad3_;                   /* 0x42 */
#ifdef __cplusplus
    void func_0204f214(int);
    void func_0204f2d4();
    void func_0204f3e0();
    void func_0204f400();
    void func_0204f4bc();
    void func_0204f558(int);
    void func_0204f5a0(int);
    int func_0204f600(const void *, u32, struct SNDBankData *);
    void func_0204f630();
#endif
} NNSSndSeqPlayer;

typedef char NNSSndSeqPlayer_size_must_be_0x44[sizeof(NNSSndSeqPlayer) == 0x44 ? 1 : -1];

typedef struct NNSSndHandle {
    NNSSndSeqPlayer *player;
#ifdef __cplusplus
    void func_02012860(int, int, int, int);
    void func_020123c8(int, int, const Vector3 *);
    int func_02011dcc(int);
    void func_02013524(int, int);
    int func_02051a98(int, unsigned int, int, struct Elem *, struct ElemTable *, int, int);
    int func_02051bd0(int, unsigned int, int, Sound::InfoSequenceEntry *, int);
    int func_02051e60(int, int, int, unsigned int, int);
    int func_02051f1c(u32, int);
    int func_02051fb4(unsigned int);
    NNSSndSeqPlayer *func_0204f63c(int, int);
    void func_0204f6f8(int, int);
    void func_0204f720(int);
#endif
} NNSSndHandle;

/* The snd_player.c setters SoundEngine.cpp defines. The SDK spells
   trackBitMask u16; these take int, as the byte-matched bodies do. */
void NNS_SndPlayerSetVolume(NNSSndHandle *handle, int volume);
void NNS_SndPlayerSetInitialVolume(NNSSndHandle *handle, int volume);
void NNS_SndPlayerMoveVolume(NNSSndHandle *handle, int targetVolume, int frames);
void NNS_SndPlayerSetChannelPriority(NNSSndHandle *handle, int priority);
void NNS_SndPlayerSetTrackMute(NNSSndHandle *handle, int trackBitMask, int flag);
void NNS_SndPlayerSetTrackVolume(NNSSndHandle *handle, int trackBitMask, int volume);
void NNS_SndPlayerSetTrackPitch(NNSSndHandle *handle, int trackBitMask, int pitch);
void NNS_SndPlayerSetTrackPan(NNSSndHandle *handle, int trackBitMask, int pan);
void NNS_SndPlayerSetTrackModDepth(NNSSndHandle *handle, int trackBitMask, int depth);
void NNS_SndPlayerSetTrackModSpeed(NNSSndHandle *handle, int trackBitMask, int speed);
void NNS_SndPlayerSetTempoRatio(NNSSndHandle *handle, int ratio);

#ifdef __cplusplus
}
#endif

#endif
