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
 * Starter per param, from __sinit_ov002_02107f88's seven {ptr,0} pairs:
 * 0 PlaySecretSound, 1 PlaySmallSecretSound, 2 func_0200f7f0,
 * 3/5/6 func_ov002_020f9468, 4 func_ov002_020f93a8. Slot 7 is unfilled,
 * and row 7 of the sound table is the _ZTV12daSoundObj_c header, so 7
 * is not a real param even though InitResources only rejects above 7.
 *
 * daSoundObj_c_classInit is reconstructed (RTTI daSoundObj_c, SOUND_OBJ
 * registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: Sound::PlaySub stays mangled -- Fix12<int> by value (wall
 *   6az); the namespace spelling homes an 8-byte stack slot (measured
 *   in d_a_sld_mng.cpp). loop stays int: the row byte passes through
 *   unconverted.
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
 *   (S14), as are the sound table and the callback table.
 * Leftover: the data_* externs keep their linker spellings.
 *   data_0208e430 is the current sub-sound ID, owned by
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

extern "C" {
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 soundID, u32 volume, u32 pan, Fix12i distance, int loop);
extern SoundObjectCallback data_ov002_0211110c[];
extern SoundObjRow data_ov002_0210c080[];  /* 7 rows; row 7 is the _ZTV12daSoundObj_c header */
extern int data_0208e430;                 /* current sub-sound ID, owned by Sound::PlaySub */
extern int data_0209b490;                 /* MUSIC_VOLUME_LSL_12 */
extern int data_0209b49c;                 /* MESSAGE_SOUND_VOLUME_LSL_12 */
}

// @symbol daSoundObj_c_classInit
extern "C" daSoundObj_c *daSoundObj_c_classInit()
{
    return new daSoundObj_c();
}

// @symbol _ZN12daSoundObj_c13InitResourcesEv
/* opt_loop_invariants is file-global last-wins: this off covers the
   FindWithActorID loop below (dropping it un-matches InitResources),
   and no other member has a loop. */
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
    current = data_0208e430;

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
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(current, 0x7f, 0, 0x7f000, 0);
        }
    }

    mCounter = 0;
    mAreaId = -1;
    return 1;
}

// @symbol _ZN12daSoundObj_c8BehaviorEv
int daSoundObj_c::Behavior()
{
    SoundObjectCallbackOwner *owner = (SoundObjectCallbackOwner *)this;
    int done = (owner->*data_ov002_0211110c[param1])(&mCounter);
    if (done != 0 || mSoundID != data_0208e430
        || (mCounter > 0xa && data_0209b49c <= 0x7f)) {
        MarkForDestruction();
        if (data_0208e430 != 0x22) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(data_0208e430, 0x7f, 0, 0x7f000, 0);
        }
    }
    if (param1 != 6) {
        if (data_0209b490 < mVolume)
            mCounter = mCounterLimit;
    }
    return 1;
}

/* kPoolDist is the tree's existing spelling for this literal, not coined here:
   src/func_0200f7f0.c and the two Sound::Play*SecretSound files give the same
   0x8777 the same name. Its sibling 0xcb33 in func_ov002_020f93a8 has no such
   precedent and is left bare. */
static const int kPoolDist = 0x8777;

// @symbol func_ov002_020f9468
extern "C" int func_ov002_020f9468(daSoundObj_c *self, u16 *counter)
{
    /* ret/goto done is this starter family's shape -- PlaySecretSound,
       PlaySmallSecretSound and func_0200f7f0 all share it -- and early
       returns un-match this function. */
    int ret = 0;
    if (*counter < self->mCounterLimit - 0xf) {
        int vol = self->mVolume;
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(self->mSoundID, vol, 0x7f, (0x7f - vol) << 0xc, self->mLoop);
        *counter += 1;
        goto done;
    }
    if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(self->mSoundID, 0x7f, 0, kPoolDist, self->mLoop))
        return 1;
done:
    return ret;
}

// @symbol func_ov002_020f93a8
extern "C" int func_ov002_020f93a8(daSoundObj_c *self, u16 *counter)
{
    if (counter[0] < self->mCounterLimit) {
        int vol = self->mVolume;
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(self->mSoundID, vol, 0x7f, ((0x7f - vol) << 12) / 5, 1) != 0)
            self->mCounterLimit = 1;
        counter[0]++;
    } else if (self->mCounterLimit == 0) {
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(self->mSoundID, 0x7f, 0, 0xcb33, 1) != 0)
            return 1;
    }
    return 0;
}
