//cpp
/* Sound -- the game's public sound API and the state block behind it.
 *
 * Run 0x02011654..0x02013548: the 16-bucket sound-object registry at
 * data_0209b53c (find/alloc/link/unlink/sweep), the voice-group loaders,
 * the banked Play wrappers, the music-layer state machine, and the boot
 * init and per-frame update entry points func_020133bc/func_020132d8.
 * include/Sound.h models the namespace.
 *
 * leftovers:
 * - Sound::Play is defined void; PlayBank0's caller harvests r0 through a
 *   cast of its address (the same return-split the shards carried).
 *   Play2D returns Player_PlaySoundEffect's result, which its tail call
 *   leaves in r0.
 * - ChangeMusicVolume is Sound::ChangeMusicVolume(unsigned int, int). The
 *   second value is a 0x1000-scaled volume, spelled Fix12i in the header.
 * - `int dd = (int)(((long long)d))' in func_02012468 is a no-op tower of
 *   unknown load-bearing status.
 */
#include "types.h"
#include "Sound.h"
#include "daPly_c.h"
#include "private/watchdog_alarm.h"
#include "Memory.h"
#include "math/Approach.h"
#include "nnsys/snd/player.h"
#include "private/sound_positional.h"

/* SoundEngine's sound heap, opaque here. */
struct SndHeap;
typedef void (*SndHeapDisposeCallback)(void *mem, u32 size, u32 data1, u32 data2);

extern "C" {

/* Foreign callees. */
extern int func_02051f1c(void *a, u32 b, int c);
extern int func_02051e60(NNSSndHandle *handle, int b, int c, int d, unsigned int e, int f);
extern u32 func_02051074(struct SndHeap *heap);
extern void func_020510a4(struct SndHeap *heap, int level);
extern void *func_020511d4(struct SndHeap *heap, u32 size, SndHeapDisposeCallback callback,
                           u32 data1, u32 data2);
extern int func_0203d974(void);
extern s32 func_0205117c(struct SndHeap *heap);
extern int func_0203d7d4(int a);
extern void func_0204f9c4(int a, int b);
extern void func_0204f958(int a, int b);
extern int func_0204fa2c(NNSSndHandle *handle, int z);
extern void func_0204f94c(void *p);
extern void func_0204f070(void);
extern struct SndHeap *func_0205130c(u32 addr, u32 size);
extern void func_02050f34(void *arc, const char *path, struct SndHeap *heap, int loadSymbols);
extern int func_02052008(struct SndHeap *heap);
extern void func_020506fc(int arg);
extern void func_02049cd8(void *a, int b, int mode);
extern int func_02051918(unsigned int id, struct SndHeap *heap);
extern int func_02051fb4(void *g, unsigned int x);
extern void func_02049764(void);
extern void func_0204921c(NNSSndHandle *handle, int v);
extern void func_020494cc(void *p, int s);
extern void func_020490b0(void *p);
extern void func_0204f03c(void);
extern void *func_02050cdc(int a, int idx);
extern void func_02049c48(int self);
extern void func_0204efe0(void);
extern void func_0204effc(void);

/* In-TU functions, declared once so callers ahead of their definitions link. */
extern void func_02013524(NNSSndHandle *handle, int vol, int mode);
extern int func_020134d8(void *a, struct SndHeap *heap);
extern void func_020133bc(void);
extern int func_02013078(void);
extern void func_02012d64(void);
extern void func_02012860(NNSSndHandle *handle, int flags, int c, int d, int e);
extern void func_020127ec(int a0, int a1, int a2, int a3, int a4, int a5);
extern int func_020126e8(int a);
extern void func_020123c8(NNSSndHandle *handle, int type, int code, const Vector3 *arg);
extern int func_02011dcc(NNSSndHandle *handle, int x);
extern void func_02011c54(void);
extern void func_02011b7c(void);
extern void func_02011b38(void *c, int a1, int a2, int a3, short a5);
extern void func_02011af8(void *c, int a1, int a2, int a3, const Vector3 *s0, short s1);
extern void func_02011ac4(void *c, int a1, int a2, int a3, const Vector3 *s0, int s1, short s2);
extern int func_02011aa0(void *c);
struct SndRegTable;
struct SndRegEntry;
extern void func_02011a28(SndRegTable *c);
extern void func_020119c8(void *base);
extern void *func_02011934(SndRegTable *table, int id);
extern int func_020116c4(SndRegTable *c, int a1, int a2, const Vector3 *a3, int s0, short s1);
extern void func_02011698(SndRegTable *table, SndRegEntry *n);
extern int func_02011654(char *table, char *n);
extern int Player_PlaySoundEffect(NNSSndHandle *handle, int arg1, int arg2);
extern int Sound_PlayIfNotActive(int a, int b, int c, short d);

/* Module state. Volumes are 0..0x7f; the current ones are 20.12 fixed point
   and ramp toward their targets by one shared step (func_02012d64,
   func_020132d8). The "sub" channel is the second sequence Sound::PlaySub
   starts on its own handle, ducking the music while it plays. */

/* While no volumes are saved, -1. Sound::PauseMusic and func_02012e78 save
   both targets here and duck them to 0x40; Sound::UnpauseMusic and
   func_02012e1c put them back. */
static signed char sSavedMusicVolume = -1;
static signed char sSavedSubVolume = -1;

/* The bank Sound::LoadGroupAndSetBank last set; Player_PlaySoundEffect
   starts a type-3 sequence with it. */
static signed char sCurrentBank = -1;

static signed char sMusicVolumeTarget = 0x7f;

/* The sequence Sound::PlaySub started on sSubHandle, -1 when none. The player
   and daSoundObj_c read it. */
int gSubSoundID = -1;

/* While not -1, func_02013078 uses it as sublevel 8's track mode;
   daMugenBGM_c sets it. */
int gMusicTrackModeOverride = -1;

/* The track mode func_02013078 last picked for the music handle (the mode
   func_0204921c and func_020494cc take). */
static int sMusicTrackMode = -1;

/* The music each layer's LoadAndSetMusic asked for, -1 when none. */
static int sLayer1Music = -1;
static int sLayer2Music = -1;
static int sLayer3Music = -1;

static signed char sSubVolumeTarget;

/* Raised and dropped again by the lid-close handler func_02019ac4. While it
   is set, func_02013524 moves every volume to 0 at once. */
unsigned char gSoundMuted;

/* The voice group func_02011f7c loaded, 0 when none. */
static unsigned char sPlayerVoiceGroup;

/* The group Sound::LoadGroupAndSetBank loaded. */
static unsigned char sLoadedGroup;

/* Set once func_020133bc has brought sound up. */
unsigned char gSoundInitialized;

static int data_0209b484;
static int data_0209b488;
static int data_0209b48c;
int gMusicVolume;
static int sVolumeRampStep;

/* The sound heap func_020133bc carves out of a 0x100000-byte block. */
static struct SndHeap *sSndHeap;

int gSubVolume;

/* Sequence handles. */
static NNSSndHandle sMusicHandle;
static NNSSndHandle data_0209b4a4;

static int data_0209b4a8;

/* The music now playing on sMusicHandle; daSetSE_c reads it. */
int gCurrentMusicID;

static NNSSndHandle sSubHandle;

/* The sound archive func_020133bc opens. */
static char sSndArc[0x88];

extern int data_0209baa0;
/* "/data/sound_data.sdat", the archive path func_020133bc opens. Most likely
   a string literal in the original: mwcc emits literals after a TU's
   size-sorted named objects, which is where retail has it (after
   sPlayerSeqLimits). It stays extern because it ends 2 bytes short of the
   next 4-aligned object, and that tail fill cannot be claimed yet. */
extern char data_0208e498[];
extern unsigned char gLocalPlayerId;
extern signed char gCurrentSublevel;
extern void *gPlayers[];
extern signed char gCurrentAreaId;
extern const unsigned char data_02075250[8];
extern unsigned char data_02075258[];

}

typedef struct {
    int playerID;
    int maxSeq;
} SoundPlayerDefault;

/* What func_02011c8c restores and DisableSoundPlayersForCredits zeroes:
   each sound player's playable sequence count. */
static SoundPlayerDefault sPlayerSeqLimits[10] = {
    {9, 6}, {10, 3}, {15, 1}, {17, 5}, {18, 1}, {19, 1}, {28, 1}, {13, 3}, {2, 2}, {3, 1},
};

/* The sound-object registry: a counter/cursor header, a fixed 0x40-entry pool
 * of 0x14-byte nodes at +0x8, and 16 hash-bucket heads at +0x508 chaining the
 * live entries by id&0xf. */
typedef struct SndRegEntry {
    int id;                        /* unique id; 0 marks a free slot */
    short fadeFrames;              /* 0x04 - fade-out passed to func_0204fa2c */
    unsigned char inUse;           /* in-use flag checked by the sweeps */
    unsigned char pad7[1];
    NNSSndHandle handle;      /* 0x08 */
    struct SndRegEntry *prev; /* 0x0c - bucket chain, toward the head */
    struct SndRegEntry *next; /* 0x10 - bucket chain, away from the head */
} SndRegEntry;

extern "C" void func_02011a5c(void *vc);

typedef struct SndRegTable {
    int counter;                 /* 0x00 - handed out as the unique id */
    unsigned char cursor;        /* 0x04 - round-robin cursor into entries */
    unsigned char pad5[3];
    SndRegEntry entries[0x40];   /* 0x08 */
    SndRegEntry *buckets[16];    /* 0x508 - chain heads by id&0xf */

    int func_0201179c(int a1, int a2, const Vector3 *a3, short s1);
    int func_0201186c(int a1, int a2, short a3);

    SndRegTable() {
        func_02011a5c(this);
    }
} SndRegTable;

SndRegTable data_0209b53c;

namespace Sound {

void LoadAndSetMusic_Layer1(int j)
{
    sMusicTrackMode = -1;
    gMusicTrackModeOverride = -1;
    data_0209b48c = 0;
    if (j < 0) {
        func_0204fa2c(&sMusicHandle, 0);
    } else {
        if (!(gSubSoundID >= 0 && gCurrentMusicID >= 0 && sLayer1Music == j))
            func_02011dcc(&sMusicHandle, j);
        func_02013524(&sMusicHandle, 0x7f, 0);
        func_0204fa2c(&sSubHandle, 0);
    }
    gCurrentMusicID = j;
    sLayer1Music = j;
    func_02011b7c();
}

}

extern "C" int func_02013078(void)
{
    daPly_c *obj = (daPly_c *)gPlayers[gLocalPlayerId];
    int state;
    int flag;
    int result;

    if (obj == 0)
        return -1;
    flag = (obj->mIsUnderwater != 0);
    state = gCurrentSublevel;
    result = -1;
    if (state == 8) {
        result = gMusicTrackModeOverride;
        if (result < 0) {
            if (flag)
                result = 0;
            else
                result = 1;
        }
    } else if (state == 9) {
        result = flag ? 1 : 2;
    } else if (state == 0x12) {
        if (gCurrentAreaId == 0)
            result = 1;
        else
            result = flag ? 0 : 2;
    } else if (state == 0xc) {
        if (gCurrentAreaId == 0)
            data_0209b48c = Sound_PlayIfNotActive(data_0209b48c, 3, 0x18d, 0);
        result = (gCurrentAreaId == 0) ? 4 : 3;
    } else if (state == 0xd) {
        result = (obj->mPosY < (int)0xffe3e000) ? 5 : 6;
    } else if (state == 0x15) {
        result = (obj->mPosY < 0) ? 5 : 6;
    }
    if (result >= 0 && result != sMusicTrackMode) {
        sMusicTrackMode = result;
        return result;
    }
    return -1;
}

namespace Sound {

void StopLoadedMusic_Layer1(unsigned int arg)
{
    gCurrentMusicID = ~0u;
    func_0204fa2c(&sMusicHandle, arg);
}

void PauseMusic(void)
{
    int state = gCurrentSublevel;
    if (state == 2 || state == 5 || state == 4 || state == 0x32) {
        if (sSavedMusicVolume < 0) {
            sSavedMusicVolume = sMusicVolumeTarget;
            sSavedSubVolume = sSubVolumeTarget;
            sMusicVolumeTarget = 0x40;
            sSubVolumeTarget = 0x40;
            sVolumeRampStep = 0xc999;
        }
    } else {
        func_0204f958(0, 1);
        func_0204f958(1, 1);
    }
    func_02011c54();
    func_0204f9c4(0xd, 1);
}

void UnpauseMusic(void)
{
    signed char s = gCurrentSublevel;
    if (s == 2 || s == 5 || s == 4 || s == 0x32) {
        signed char saved = sSavedMusicVolume;
        if (saved < 0) return;
        sMusicVolumeTarget = saved;
        signed char saved2 = sSavedSubVolume;
        sSubVolumeTarget = saved2;
        sSavedMusicVolume = -1;
        return;
    }
    func_0204f958(0, 0);
    func_0204f958(1, 0);
}

}

extern "C" void func_02012e78(void)
{
    if (sSavedMusicVolume >= 0)
        return;
    sSavedMusicVolume = sMusicVolumeTarget;
    sSavedSubVolume = sSubVolumeTarget;
    sMusicVolumeTarget = 0x40;
    sSubVolumeTarget = 0x40;
    sVolumeRampStep = 0xc999;
}

extern "C" void func_02012e1c(void)
{
    signed char v = sSavedMusicVolume;
    if (v < 0) return;
    sMusicVolumeTarget = v;
    sSubVolumeTarget = sSavedSubVolume;
    sSavedMusicVolume = -1;
}

extern "C" void func_02012dd0(void *c)
{
    func_0204f9c4(0, (int)c);
    func_0204f9c4(1, (int)c);
    func_0204f9c4(0xe, (int)c);
    func_0204f9c4(0x15, (int)c);
    func_0204f9c4(0x1f, (int)c);
}

extern "C" void func_02012dbc(int a)
{
    func_0204f9c4(0xe, a);
}

extern "C" void func_02012d64(void)
{
    ApproachLinear(gMusicVolume, (s32)sMusicVolumeTarget << 12, sVolumeRampStep);
    func_02013524(&sMusicHandle, gMusicVolume >> 12, 0);
}

namespace Sound {
int ChangeMusicVolume(unsigned int a, int b) {
    sMusicVolumeTarget = a;
    sVolumeRampStep = b;
    return gMusicVolume == (sMusicVolumeTarget << 12);
}
}

namespace Sound {
bool PlaySub(unsigned int a, unsigned int b, unsigned int c, int d, bool e) {
    int x;
    if (c == 0) {
        /* e is bool. A plain test compiles to ldrb; retail loads the whole word. */
        if (!*(int *)&e) {
            x = gSubSoundID;
            if ((x >= 0 && x != (int)a) ||
                (gSubVolume == 0 && sMusicVolumeTarget == (int)b && gMusicVolume == (int)(b << 12)))
                return true;
        }
    } else {
        x = gSubSoundID;
        if (x < 0 || *(int *)&e) { /* same full-word load as above */
            if (x != (int)a) {
                gSubSoundID = a;
                if (a != 0x35 && a != 0x36) {
                    if (a != 0x2e)
                        gSubVolume = c & 0xff;
                }
                func_02011dcc(&sSubHandle, a);
            }
        } else if (x != (int)a) {
            return false;
        }
    }
    sMusicVolumeTarget = (signed char)b;
    sSubVolumeTarget = (signed char)c;
    sVolumeRampStep = d;
    return gMusicVolume == (int)(b << 12) && gSubVolume == (int)(c << 12);
}
}

namespace Sound {

void LoadAndSetMusic_Layer3(unsigned int musicId)
{
    sLayer3Music = musicId;
    func_02011dcc(&sMusicHandle, musicId);
}

void StopLoadedMusic_Layer3(void)
{
    int a = gCurrentMusicID;
    if (a < 0) {
        if (sLayer3Music < 0)
            return;
    }
    sLayer3Music = -1;
    int b = sLayer1Music;
    if (a == b) {
        int e440 = sLayer2Music;
        if (e440 >= 0) {
            func_02011dcc(&sMusicHandle, e440);
            return;
        }
    }
    if (a < 0 || b < 0)
        func_0204fa2c(&sMusicHandle, 0);
    else
        func_02011dcc(&sMusicHandle, a);
}

void SetMusic(unsigned int j1, unsigned int j2)
{
    if (j1 != gLocalPlayerId)
        return;
    if (gCurrentMusicID < 0) {
        if (sLayer1Music >= 0)
            return;
    }
    int e = sLayer3Music;
    gCurrentMusicID = j2;
    if (e >= 0) return;
    func_02011dcc(&sMusicHandle, j2);
}

/* ONE parameter, although this symbol used to be spelled with two (`...Ejj`).
   The second was never read here, and every ROM call site passes a single
   argument -- `LDRB r0, [...]; BL`, r1 untouched. The extra `j` kept the symbol
   unspellable from C++: a truthful declaration mangled to ...Ej and matched
   nothing, so callers had to hand-spell the two-parameter name and pass one
   argument. The symbol is now ...Ej and include/Sound.h declares it. */
void EndMusic(unsigned int playerNo) {
    if (gCurrentMusicID < 0)
        return;
    if (playerNo != gLocalPlayerId)
        return;
    int e43c = sLayer1Music;
    int e444 = sLayer3Music;
    gCurrentMusicID = e43c;
    if (e444 >= 0) return;
    int e440 = sLayer2Music;
    if (e440 >= 0) {
        func_02011dcc(&sMusicHandle, e440);
        return;
    }
    if (e43c >= 0) {
        func_02011dcc(&sMusicHandle, e43c);
        return;
    }
    func_0204fa2c(&sMusicHandle, 0);
}

void LoadAndSetMusic_Layer2(unsigned int j)
{
    int a = gCurrentMusicID;
    int b = sLayer1Music;
    sLayer2Music = j;
    if (a != b) return;
    func_02011dcc(&sMusicHandle, j);
}

void StopLoadedMusic_Layer2(void)
{
    int a = gCurrentMusicID;
    if (a < 0) {
        if (sLayer2Music < 0)
            return;
    }
    sLayer2Music = -1;
    if (a < 0) {
        func_0204fa2c(&sMusicHandle, 0);
        return;
    }
    func_02011dcc(&sMusicHandle, a);
}

}

extern "C" void func_02012860(NNSSndHandle *handle, int flags, int c, int d, int e) {
    if (flags & 1)
        NNS_SndPlayerSetTrackVolume(handle, 0xffff, c);
    if (flags & 2)
        NNS_SndPlayerSetTrackPitch(handle, 0xffff, d);
    if (flags & 4)
        NNS_SndPlayerSetTrackPan(handle, 0xffff, e - 0x40);
}

namespace Sound {

unsigned int Play2D(unsigned int j1, unsigned int j2) {
    return Player_PlaySoundEffect(&data_0209b4a4, j1, j2);
}
}

extern "C" void func_020127ec(int a0, int a1, int a2, int a3, int a4, int a5)
{
    Player_PlaySoundEffect(&data_0209b4a4, a0, a1);
    func_02012860(&data_0209b4a4, a2, a3, a4, a5);
}

extern "C" void func_020127a4(int r0, int r1, int r2, int r3)
{
    Player_PlaySoundEffect(&data_0209b4a4, r0, r1);
    NNS_SndPlayerSetTrackPan(&data_0209b4a4, r2, r3);
}

extern "C" unsigned int func_02012790(unsigned int a)
{
    return Sound::Play2D(2, a);
}

extern "C" unsigned int func_0201277c(unsigned int a)
{
    return Sound::Play2D(3, a);
}

namespace Sound {

unsigned int PlayBank3_2D(unsigned int a)
{
    return Play2D(3, a);
}

unsigned int PlayBank2_2D(unsigned int a)
{
    return Play2D(2, a);
}

}

extern "C" void func_02012718(int a, int b)
{
    func_020127ec(2, a, 4, 0, 0, func_020126e8(b));
}

extern "C" int func_020126e8(int a)
{
    int r = a * 0x4c / 0x100000 + 0x1a;
    if (r < 0x1a) return 0x1a;
    if (r > 0x66) return 0x66;
    return r;
}

extern "C" void func_020126ac(int a0, int a1, int a2, int a3, int s0)
{
    func_020127ec(2, a0, a1, a2, a3, s0);
}

extern "C" void func_02012694(unsigned int id, const Vector3 *v)
{
    Sound::Play(3, id, *v);
}

extern "C" void func_0201267c(unsigned int id, const Vector3 *v)
{
    Sound::Play(3, id, *v);
}

namespace Sound {

void PlayBank3(u32 id, const Vector3 &pos)
{
    Play(3, id, pos);
}

void PlayBank0(u32 id, const Vector3 &pos)
{
    Play(0, id, pos);
}

void Play(unsigned int j1, unsigned int j2, const Vector3 &v)
{
    char *s = (char *)func_02050cdc(j1, j2);
    int t = (unsigned char)s[5];
    if (t == 9 || t == 2) {
        SoundSlot *slot = func_02048720(&v, j1, j2);
        if (slot == 0)
            return;
        Player_PlaySoundEffect(&slot->handle, j1, j2);
        func_02048908(slot, &v);
        return;
    }
    if (func_02048a1c(&v, j1, j2) == 0)
        return;
    Player_PlaySoundEffect(&data_0209b4a4, j1, j2);
    func_02048d80(&data_0209b4a4, &v);
}

}

extern "C" void func_0201251c(int a, int b, const Vector3 *c, int d) {
    int r4 = func_02048a18(b, d);
    if (func_02048a1c(c, a, r4) == 0)
        return;
    Player_PlaySoundEffect(&data_0209b4a4, a, r4);
    func_02048af4(&data_0209b4a4, c, d);
}

extern "C" int Sound_PlayIfNotActive(int a, int b, int c, short d) {
    SndRegEntry *o = (SndRegEntry *)func_02011934(&data_0209b53c, a);
    if (o) {
        o->inUse = 1;
        return o->id;
    }
    return data_0209b53c.func_0201186c(b, c, d);
}

extern "C" int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h)
{
    int dd = (int)(((long long)d));
    int x;
    SndRegEntry *p;

    x = Sound_PlayIfNotActive(a, b, c, h);
    p = (SndRegEntry *)func_02011934(&data_0209b53c, x);
    if (p != 0)
        func_02012860(&p->handle, dd, e, f, g);
    return x;
}

extern "C" void func_020123c8(NNSSndHandle *handle, int type, int code, const Vector3 *arg) {
    if (type == 3 && code >= 0x100 && code <= 0x103) {
        func_020485f0(handle, arg);
        return;
    }
    if (type == 3 && code == 0x18d) {
        func_02048528(handle, arg);
        return;
    }
    if (type == 3 && code == 0x81) {
        func_020485f0(handle, arg);
        return;
    }
    func_02048d80(handle, arg);
}

namespace Sound {

int PlayLong(u32 handle, u32 a, u32 b, const Vector3 &pos, s16 e)
{
    if (func_02048a1c(&pos, a, b) == 0)
        return 0;

    SndRegEntry *slot = (SndRegEntry *)func_02011934(&data_0209b53c, handle);
    if (slot) {
        slot->inUse = 1;
        func_020123c8(&slot->handle, a, b, &pos);
        return slot->id;
    }

    return data_0209b53c.func_0201179c(a, b, &pos, e);
}

}

extern "C" int func_02012310(int handle, int sound, short arg) {
    return Sound_PlayIfNotActive(handle, 2, sound, arg);
}

extern "C" int func_0201226c(int a0, int a1, int a2, const Vector3 *a3, int a4, short a5) {
    SndRegEntry *p;
    if (func_02048a1c(a3, a1, a2) == 0)
        return 0;
    p = (SndRegEntry *)func_02011934(&data_0209b53c, a0);
    if (p != 0) {
        p->inUse = 1;
        func_02048c30(&p->handle, a3, a4);
        return p->id;
    }
    return func_020116c4(&data_0209b53c, a1, a2, a3, a4, a5);
}

extern "C" int func_02012194(int handle, int a1, int a2, int a3, int s0, const Vector3 *s1,
                             short s2) {
    SndRegEntry *e;
    if (func_02048a1c(s1, a1, a2) == 0)
        return 0;
    e = (SndRegEntry *)func_02011934(&data_0209b53c, handle);
    if (e != 0) {
        NNSSndHandle *slot = &e->handle;
        e->inUse = 1;
        func_02048d80(slot, s1);
        NNS_SndPlayerSetTrackPitch(slot, a3, s0);
        return e->id;
    } else {
        int n = data_0209b53c.func_0201179c(a1, a2, s1, s2);
        SndRegEntry *e2 = (SndRegEntry *)func_02011934(&data_0209b53c, n);
        if (e2 != 0)
            NNS_SndPlayerSetTrackPitch(&e2->handle, a3, s0);
        return n;
    }
}

extern "C" unsigned int func_02012174(unsigned int a, unsigned int b)
{
    return Sound::Play2D(1, b + data_02075250[a]);
}

namespace Sound {

unsigned int PlayCharVoice(unsigned int a, unsigned int b, const Vector3 &v)
{
    return ((unsigned int (*)(unsigned int, unsigned int, const Vector3 &))Play)(
        1, b + data_02075250[a], v);
}

}

extern "C" int func_02012120(unsigned int p0, unsigned int p1, unsigned int p2, const Vector3 *p3,
                             short p4) {
    p2 += data_02075250[p1];
    return Sound::PlayLong(p0, 1, p2, *p3, p4);
}

namespace Sound {

void LoadGroupAndSetBank(int a, int b)
{
    if (func_0203d974() != 0) {
        if (a != 0x2f) return;
        func_0203d7d4(func_020134d8((void *)a, sSndHeap));
        sCurrentBank = b;
        sLoadedGroup = a;
        return;
    }
    if (a == 0) {
        func_020134d8((void *)a, sSndHeap);
        data_0209b4a8 = func_0205117c(sSndHeap);
    } else if (sLoadedGroup != a) {
        func_020510a4(sSndHeap, data_0209b4a8);
        func_020134d8((void *)a, sSndHeap);
        data_0209b484 = func_0205117c(sSndHeap);
    }
    sCurrentBank = b;
    sLoadedGroup = a;
    sPlayerVoiceGroup = 0;
}

void LoadInitialGroup(int group)
{
    LoadGroupAndSetBank(group, 0x20);
}

void UnsetPlayerVoiceGroup(void)
{
    sPlayerVoiceGroup = 0;
}

}

extern "C" void func_02011f7c(void *self)
{
    if (!func_0203d974()) {
        func_020510a4(sSndHeap, data_0209b484);
        func_020134d8(self, sSndHeap);
        data_0209b488 = func_0205117c(sSndHeap);
    }
    sPlayerVoiceGroup = (unsigned char)(int)self;
}

namespace Sound {

void ResetPlayerVoiceGroup(void)
{
    if (func_0203d974() == 0) {
        func_020510a4(sSndHeap, data_0209b484);
    }
    sPlayerVoiceGroup = 0;
}

}

extern "C" int func_02011ee4(void *c)
{
    int x = func_0203d974();
    if (x != 0) return x;
    func_020510a4(sSndHeap, data_0209b488);
    return func_020134d8(c, sSndHeap);
}

extern "C" void *func_02011e94(int *out) {
    int r = func_02051074(sSndHeap);
    void *block = func_020511d4(sSndHeap, r, 0, 0, 0);
    if (out != 0)
        *out = r;
    return block;
}

extern "C" int Player_PlaySoundEffect(NNSSndHandle *handle, int arg1, int arg2) {
    if (gSoundInitialized == 0)
        return 1;
    if (arg1 != 3)
        return func_02051f1c(handle, arg1, arg2);
    return func_02051e60(handle, -1, sCurrentBank, -1, arg1, arg2);
}

extern "C" int func_02011dcc(NNSSndHandle *handle, int x) {
    int r4 = func_02051fb4(handle, x);
    if (r4 != 0 && handle == &sMusicHandle) {
        func_02049764();
        func_02013078();
        if (sMusicTrackMode >= 0) {
            func_0204921c(handle, sMusicTrackMode);
        }
    }
    return r4;
}

extern "C" void func_02011dc0(void)
{
    func_0204effc();
}

extern "C" void func_02011db4(void)
{
    func_0204efe0();
}

extern "C" void func_02011d5c(int self)
{
    if (gSoundInitialized == 0)
        return;
    switch (self) {
    case 1: func_02048e94(); break;
    case 2: func_02048e74(); break;
    default: func_02048e54(); break;
    }
    func_02049c48(self);
}

extern "C" void func_02011d50(void)
{
    func_02048f24();
}

extern "C" void func_02011d44(void)
{
    func_02048f14();
}

extern "C" void func_02011d38(void)
{
    func_02048f04();
}

extern "C" void func_02011d2c(void)
{
    func_02048ef4();
}

extern "C" void func_02011d20(void)
{
    Sound::Func_02048ee4();
}

extern "C" void func_02011d14(void)
{
    func_02048ed4();
}

extern "C" void func_02011d08(void)
{
    Sound::Func_02048ec4();
}

extern "C" void func_02011cfc(void)
{
    Sound::Func_02048eb4();
}

extern "C" void DisableSoundPlayersForCredits(void)
{
    u32 i;
    s32 zero;
    zero = 0;
    for (i = 0; i < 10; i++)
        Sound::Player::SetPlayableSeqCount(sPlayerSeqLimits[i].playerID, zero);
}

extern "C" void func_02011c8c(void)
{
    u32 i;
    for (i = 0; i < 10; i++)
        Sound::Player::SetPlayableSeqCount(sPlayerSeqLimits[i].playerID,
                                           sPlayerSeqLimits[i].maxSeq);
}

extern "C" void func_02011c54(void)
{
    unsigned int i;
    unsigned char *p = data_02075258;
    for (i = 0; i < 11; i++) {
        func_0204f9c4(*p, 1);
        p = p + 1;
    }
}

extern "C" void func_02011c24(void)
{
    func_0204f958(0, 1);
    func_0204f958(1, 1);
    func_02011c54();
}

extern "C" void func_02011b7c(void)
{
    int v = gSubSoundID;
    gMusicVolume = 0x7f000;
    sMusicVolumeTarget = 0x7f;
    sVolumeRampStep = 0x7f000;
    if (v >= 0) {
        gSubSoundID = -1;
        func_0204fa2c(&sSubHandle, 0);
    }
    sLayer3Music = -1;
    sLayer2Music = -1;
    gSubVolume = 0;
    sSubVolumeTarget = 0;
    sSavedMusicVolume = -1;
}

extern "C" void func_02011b38(void *c, int a1, int a2, int a3, short a5)
{
    SndRegEntry *e = (SndRegEntry *)c;
    e->id = a1;
    e->fadeFrames = a5;
    e->inUse = 1;
    e->prev = 0;
    e->next = 0;
    Player_PlaySoundEffect(&e->handle, a2, a3);
}

extern "C" void func_02011af8(void *c, int a1, int a2, int a3, const Vector3 *s0, short s1) {
    func_02011b38(c, a1, a2, a3, s1);
    func_020123c8(&((SndRegEntry *)c)->handle, a2, a3, s0);
}

extern "C" void func_02011ac4(void *c, int a1, int a2, int a3, const Vector3 *s0, int s1,
                              short s2) {
    func_02011b38(c, a1, a2, a3, s2);
    func_02048c30(&((SndRegEntry *)c)->handle, s0, s1);
}

extern "C" int func_02011aa0(void *c)
{
    SndRegEntry *e = (SndRegEntry *)c;
    func_0204fa2c(&e->handle, e->fadeFrames);
    e->id = 0;
    return 0;
}

extern "C" void func_02011a5c(void *vc)
{
    SndRegTable *c = (SndRegTable *)vc;
    int i;
    SndRegEntry *p = c->entries;
    do {
        p->id = 0;
        p++;
    } while (p != c->entries + 0x40);
    c->counter = 0;
    c->cursor = 0;
    for (i = 0; i < 16; i++) c->buckets[i] = 0;
}

extern "C" void func_02011a28(SndRegTable *c) {
    SndRegEntry *p = c->entries;
    int i;
    for (i = 0; i < 0x40; i++) {
        func_0204f94c(&p->handle);
        p++;
    }
}

extern "C" void func_020119c8(void *vbase)
{
    SndRegTable *base = (SndRegTable *)vbase;
    SndRegEntry *e = base->entries;
    int i;
    for (i = 0; i < 0x40; i++) {
        if (e->id) {
            if (e->inUse == 1) {
                e->inUse = 0;
            } else {
                func_02011654((char *)base, (char *)e);
                base->cursor = (unsigned char)i;
            }
        }
        e++;
    }
}

extern "C" void func_02011974(void *vbase)
{
    SndRegTable *base = (SndRegTable *)vbase;
    SndRegEntry *e = base->entries;
    int i;
    for (i = 0; i < 0x40; i++) {
        if (e->id) {
            e->inUse = 0;
            func_02011654((char *)base, (char *)e);
            base->cursor = (unsigned char)i;
        }
        e++;
    }
}

extern "C" void *func_02011934(SndRegTable *table, int id) {
    if (id == 0) return 0;
    SndRegTable *reg = table;
    int idx = id & 0xf;
    SndRegEntry *node = reg->buckets[idx];
    while (node) {
        if (id == node->id)
            return node;
        node = node->next;
    }
    return 0;
}

int SndRegTable::func_0201186c(int a1, int a2, short a3) {
    SndRegTable *reg = this;
    int i = 0;
    do {
        if (reg->entries[reg->cursor].id == 0) {
            reg->counter += 1;
            if (reg->counter == 0) {
                reg->counter += 1;
            }
            func_02011b38(&reg->entries[reg->cursor], reg->counter, a1, a2, a3);
            func_02011698(this, &reg->entries[reg->cursor]);
            return reg->counter;
        }
        reg->cursor = (reg->cursor + 1) % 0x40;
    } while (++i < 0x40);
    return 0;
}

int SndRegTable::func_0201179c(int a1, int a2, const Vector3 *a3, short s1) {
    SndRegTable *reg = this;
    int i;
    for (i = 0; i < 0x40; i++) {
        if (reg->entries[reg->cursor].id == 0) {
            reg->counter += 1;
            if (reg->counter == 0)
                reg->counter += 1;
            func_02011af8(&reg->entries[reg->cursor], reg->counter, a1, a2, a3, s1);
            func_02011698(this, &reg->entries[reg->cursor]);
            return reg->counter;
        }
        reg->cursor = (reg->cursor + 1) % 0x40;
    }
    return 0;
}

extern "C" int func_020116c4(SndRegTable *c, int a1, int a2, const Vector3 *a3, int s0, short s1) {
    SndRegTable *reg = c;
    int i;
    for (i = 0; i < 0x40; i++) {
        if (reg->entries[reg->cursor].id == 0) {
            reg->counter = reg->counter + 1;
            if (reg->counter == 0)
                reg->counter = reg->counter + 1;
            func_02011ac4(&reg->entries[reg->cursor], reg->counter, a1, a2, a3, s0, s1);
            func_02011698(c, &reg->entries[reg->cursor]);
            return reg->counter;
        }
        reg->cursor = (u8)((reg->cursor + 1) % 0x40);
    }
    return 0;
}

extern "C" void func_02011698(SndRegTable *table, SndRegEntry *n) {
    SndRegTable *reg = table;
    SndRegEntry *e = n;
    int idx = e->id & 0xf;
    SndRegEntry *head = reg->buckets[idx];
    if (head == 0) {
        reg->buckets[idx] = e;
    } else {
        head->prev = e;
        e->next = reg->buckets[idx];
        reg->buckets[idx] = e;
    }
}

extern "C" int func_02011654(char *table, char *n)
{
    SndRegTable *reg = (SndRegTable *)table;
    SndRegEntry *e = (SndRegEntry *)n;
    SndRegEntry *prev = e->prev;
    SndRegEntry *next = e->next;
    if (prev == 0) {
        int idx = e->id & 0xf;
        reg->buckets[idx] = next;
    } else {
        prev->next = next;
    }
    if (next) {
        next->prev = prev;
    }
    return func_02011aa0(n);
}

#pragma defer_codegen off

extern "C" void func_020132d8(void)
{
    if (gSoundInitialized == 0)
        return;
    func_02012d64();
    if (gSubSoundID >= 0) {
        ApproachLinear(gSubVolume, sSubVolumeTarget << 0xc, sVolumeRampStep);
        func_02013524(&sSubHandle, gSubVolume >> 0xc, 0);
        if (gSubVolume == 0) {
            gSubSoundID = -1;
            func_0204fa2c(&sSubHandle, 0);
        }
    }
    int s = func_02013078();
    if (s >= 0) {
        func_020494cc(&sMusicHandle, s);
    }
    func_020490b0(&sMusicHandle);
    func_0204f03c();
    func_020119c8(&data_0209b53c);
}

#pragma opt_propagation off
extern "C" void func_020133bc(void)
{
    char *anim;
    func_0204f070();
    sSndHeap = func_0205130c((u32)Memory::Allocate(0x100000), 0x100000);
    anim = data_0208e498;
    func_0201a9fc(&data_0209d574);
    func_02050f34(&sSndArc, anim, sSndHeap, 0);
    func_0201a9fc(&data_0209d574);
    func_02052008(func_0203d974() == 0 ? sSndHeap : 0);
    if (func_0203d974() == 0) {
        func_0201a9fc(&data_0209d574);
        func_020134d8((void *)1, sSndHeap);
        func_0201a9fc(&data_0209d574);
    }
    func_0204f94c(&sMusicHandle);
    func_0204f94c(&sSubHandle);
    func_0204f94c(&data_0209b4a4);
    func_02011a28(&data_0209b53c);
    func_02048f34(&sSndArc);
    func_020506fc(2);
    func_02049cd8(&data_0209baa0, 0x1000, 0);
    gSoundInitialized = 1;
}
/* opt_propagation is file-global last-wins under deferred codegen in mwccarm
 * 2004/b56, and the deferred queue's codegen binds when the next function
 * parses. The defer_codegen brackets emit func_020132d8 and func_020133bc at
 * parse position -- func_020132d8 first, flushing the queued functions under
 * on -- so func_020133bc alone sees off and keeps `anim` in r4 like the ROM. */
#pragma opt_propagation on
#pragma defer_codegen on

extern "C" void func_02013524(NNSSndHandle *handle, int b, int c) {
    if (gSoundMuted != 0) {
        c = b = 0;
    }
    NNS_SndPlayerMoveVolume(handle, b, c);
}

extern "C" int func_020134d8(void *a, struct SndHeap *heap) {
    int s = func_0201a9ec(&data_0209d574);
    int r = func_02051918((u32)a, heap);
    func_0201a9fc(&data_0209d574);
    func_0201a96c(&data_0209d574, s);
    return r;
}

extern "C" void func_020134c8(void)
{
    /* arity split (banked): caller passes 0, def is (void) */
    ((void (*)(int))func_020133bc)(0);
}
