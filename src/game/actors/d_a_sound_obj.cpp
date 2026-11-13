//cpp
/**
 * The positional sound emitter (SOUND_OBJ), actor 0x167.
 *
 * One actor per sub-sound: the spawn param (0-6) picks a row of the
 * seven-entry sound table (sound ID, volume, counter limit, loop flag)
 * and a starter callback from the runtime-filled table at
 * data_ov002_0211110c. At most one instance runs: InitResources kills
 * any other SOUND_OBJ while the current sub-sound is a managed one.
 * Behavior re-runs the starter each frame; when it reports done, or
 * the sub-sound moved on, the actor destroys itself and restarts the
 * current sub-sound. While the music ducks below this sound's volume
 * the counter is held at its limit (every param but 6).
 *
 * Starter per param, from the seven {ptr,0} pairs this file copies into
 * data_ov002_0211110c: 0 PlaySecretSound, 1 PlaySmallSecretSound,
 * 2 func_0200f7f0, 3/5/6 func_ov002_020f9468, 4 func_ov002_020f93a8.
 * Slot 7 is unfilled, and row 7 of the sound table is the
 * _ZTV12daSoundObj_c header, so 7 is not a real param even though
 * InitResources only rejects above 7.
 *
 * daSoundObj_c_classInit is reconstructed (RTTI daSoundObj_c, SOUND_OBJ
 * registry). Retail does not store that spelling.
 *
 * Leftover: func_ov002_020f9468 passes mLoop through `*(bool *)&`.
 *   A bool parameter converts the u8 and the function grows from 0x94
 *   to 0xac.
 * Leftover: SoundObjectCallbackOwner stays a shadow: entries 0-2 of
 *   the callback table are arm9 Sound:: starters, not daSoundObj_c
 *   members, and the sinit fills the table as non-polymorphic {ptr,0}
 *   pairs, so the owner cannot be the polymorphic leaf.
 * Leftover: func_ov002_020f9468/020f93a8 keep their linker names; the
 *   cartridge spells no member for them and both are table entries.
 * Leftover: #pragma opt_loop_invariants off is load-bearing for the
 *   FindWithActorID loop (file-global; dropping it un-matches
 *   InitResources).
 * Leftover: g_profile_SOUND_OBJ is ov002 gap data outside this TU
 *   (S14), as is the sound table. The callback table is this file's.
 * Leftover: the data_* externs keep their linker spellings.
 *   gSubSoundID is the current sub-sound ID, owned by
 *   Sound::PlaySub (no verified name); MUSIC_VOLUME_LSL_12 and
 *   MESSAGE_SOUND_VOLUME_LSL_12 are in symbols/verified.tsv and
 *   renaming is symbols.txt.
 */

#include "daSoundObj_c.h"

/* One row of the 0xc-stride sound table: the four members InitResources
 * loads, in table order. */
struct SoundObjRow {
    s32 soundID;
    s32 volume;
    u16 counterLimit;
    u8 loop;
    u8 pad_0b;
};
typedef char SoundObjRow_size_must_be_0x0c[
    sizeof(SoundObjRow) == 0x0c ? 1 : -1];

/* The callback table's owner, kept a forward-declared shadow: the table's
   entries are heterogeneous starters, not members of one class (see the
   deslop Leftover above). */
struct SoundObjectCallbackOwner;

typedef int (SoundObjectCallbackOwner::*SoundObjectCallback)(u16 *);

namespace Sound {
bool PlaySub(u32, u32, u32, int, bool);
}

extern "C" {

extern SoundObjRow data_ov002_0210c080[]; /* 7 rows; row 7 is the _ZTV12daSoundObj_c header */
extern int gSubSoundID;                   /* current sub-sound ID, owned by Sound::PlaySub */
extern int gMusicVolume;                  /* MUSIC_VOLUME_LSL_12 */
extern int gSubVolume;
}
                    /* MESSAGE_SOUND_VOLUME_LSL_12 */
extern int func_0200f7f0(dActor_c *actor, u16 *counter);
extern "C" {

int func_ov002_020f9468(daSoundObj_c *self, u16 *counter);
int func_ov002_020f93a8(daSoundObj_c *self, u16 *counter);
}


namespace Sound {
int PlaySecretSound(dActor_c *actor, u16 *counter);
int PlaySmallSecretSound(dActor_c *actor, u16 *counter);
}

/* {function, this-adjust} records. Retail stores the adjust as 0. The
   bss table is filled from these by the compiler's __sinit, in param order. */
struct SoundObjStarter {
    int func;
    int thisAdjust;
};

SoundObjStarter data_ov002_0210c010 = {(int)&Sound::PlaySmallSecretSound, 0};
SoundObjStarter data_ov002_0210c018 = {(int)&func_ov002_020f9468, 0};
SoundObjStarter data_ov002_0210c020 = {(int)&func_0200f7f0, 0};
SoundObjStarter data_ov002_0210c028 = {(int)&func_ov002_020f9468, 0};
SoundObjStarter data_ov002_0210c030 = {(int)&func_ov002_020f93a8, 0};
SoundObjStarter data_ov002_0210c038 = {(int)&func_ov002_020f9468, 0};
SoundObjStarter data_ov002_0210c040 = {(int)&Sound::PlaySecretSound, 0};

SoundObjectCallback data_ov002_0211110c[7] = {
    *(SoundObjectCallback *)&data_ov002_0210c040, *(SoundObjectCallback *)&data_ov002_0210c010,
    *(SoundObjectCallback *)&data_ov002_0210c020, *(SoundObjectCallback *)&data_ov002_0210c038,
    *(SoundObjectCallback *)&data_ov002_0210c030, *(SoundObjectCallback *)&data_ov002_0210c018,
    *(SoundObjectCallback *)&data_ov002_0210c028,
};

extern "C" daSoundObj_c *daSoundObj_c_classInit()
{
    return new daSoundObj_c();
}

/* opt_loop_invariants is file-global last-wins: this off covers the
   FindWithActorID loop below (dropping it un-matches InitResources),
   and no other member has a loop. Without it the loop's constant
   arguments are hoisted: 0x15c, not 0x14c. */
#pragma opt_loop_invariants off
int daSoundObj_c::InitResources()
{
    dActor_c *actor;
    int current;

    if (param1 > 7)
        return 0;

    mSoundID = data_ov002_0210c080[param1].soundID;
    mVolume = data_ov002_0210c080[param1].volume;
    mCounterLimit = data_ov002_0210c080[param1].counterLimit;
    mLoop = data_ov002_0210c080[param1].loop;

    actor = 0;
    current = gSubSoundID;

    /* 0x21 is tested twice, first and last: both compares are in the ROM,
       and dropping the duplicate un-matches InitResources. */
    if (current == 0x20 || current == 0x29 || current == 0x21 || current == 0x1e || current == 0x50 ||
        (current >= 0x19 && current <= 0x1d) || current == 0x4f || current == 0x22 || current == 0x2a || current == 0x21)
    /* while(1)/break is the ROM's loop shape: for and
       assignment-in-condition forms both un-match InitResources. */
    while (1) {
        actor = FindWithActorID(0x167, actor);
        if (actor == 0)
            break;
        if (actor != this) {
            actor->MarkForDestruction();
            Sound::PlaySub(current, 0x7f, 0, 0x7f000, 0);
        }
    }

    mCounter = 0;
    mAreaId = -1;
    return 1;
}

int daSoundObj_c::Behavior()
{
    SoundObjectCallbackOwner *owner = (SoundObjectCallbackOwner *)this;
    int done = (owner->*data_ov002_0211110c[param1])(&mCounter);
    if (done != 0 || mSoundID != gSubSoundID || (mCounter > 0xa && gSubVolume <= 0x7f)) {
        MarkForDestruction();
        if (gSubSoundID != 0x22) {
            Sound::PlaySub(gSubSoundID, 0x7f, 0, 0x7f000, 0);
        }
    }
    if (param1 != 6) {
        if (gMusicVolume < mVolume)
            mCounter = mCounterLimit;
    }
    return 1;
}

/* kPoolDist is the tree's existing spelling for this literal, not coined here:
   src/runtime/actor/d_actor.cpp (func_0200f7f0 and the two Sound::Play*SecretSound members) gives the same
   0x8777 the same name. Its sibling 0xcb33 in func_ov002_020f93a8 has no such
   precedent and is left bare. */
static const int kPoolDist = 0x8777;

extern "C" int func_ov002_020f9468(daSoundObj_c *self, u16 *counter) {
    /* The starter shape PlaySecretSound, PlaySmallSecretSound and
       func_0200f7f0 share: the pool fallback is an else-if, so the
       steady-state branch falls through to the one `return 0`. */
    if (*counter < self->mCounterLimit - 0xf) {
        int vol = self->mVolume;
        Sound::PlaySub(self->mSoundID, vol, 0x7f, (0x7f - vol) << 0xc, *(bool *)&self->mLoop);
        *counter += 1;
    } else if (Sound::PlaySub(self->mSoundID, 0x7f, 0, kPoolDist, *(bool *)&self->mLoop)) {
        return 1;
    }
    return 0;
}

extern "C" int func_ov002_020f93a8(daSoundObj_c *self, u16 *counter)
{
    if (counter[0] < self->mCounterLimit) {
        int vol = self->mVolume;
        if (Sound::PlaySub(self->mSoundID, vol, 0x7f, ((0x7f - vol) << 12) / 5, 1) != 0)
            self->mCounterLimit = 1;
        counter[0]++;
    } else if (self->mCounterLimit == 0) {
        if (Sound::PlaySub(self->mSoundID, 0x7f, 0, 0xcb33, 1) != 0)
            return 1;
    }
    return 0;
}
