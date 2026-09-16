#ifndef SOUND_H
#define SOUND_H

/* The sound namespace. The tree already spells `namespace Sound { ... }` for
 * its free functions -- PauseMusic, LoadGroupAndSetBank and a dozen others --
 * but the classes nested inside it have had nowhere to be declared, so every
 * one of their methods still hand-spells its own mangled name. This header is
 * where they go.
 *
 * Whether `Sound` is a namespace or a class cannot be read off the mangling:
 * _ZN5Sound6Player... is identical either way. Namespace is what the existing
 * sources chose, and this header keeps that choice rather than reopening it.
 *
 * NO FIELDS, deliberately, in the SharedFilePtr.h sense. Sound::Player's only
 * recovered method is static and reaches a file-scope table, so nothing here
 * evidences an instance layout -- and inventing one would silently retype
 * whatever else turns out to live at those addresses.
 */

#ifdef __cplusplus

#include "types.h"

/* Global scope on purpose: inside `namespace Sound` this would declare
   Sound::dActor_c, which is not the type any caller is holding. */
struct dActor_c;

namespace Sound {

/* A bank-bound wrapper over Sound::Play, defined in
   src/_ZN5Sound9PlayBank3EjRK7Vector3.cpp. The position is a REFERENCE -- the
   mangled name says RK7Vector3 -- and a caller spelling it as a pointer is
   byte-identical, which is why the wrong spelling survived so long in the
   sources that call it. Declared here so callers stop hand-mangling the name. */
void PlayBank3(u32 id, const Vector3 &pos);

/* Sequence archive metadata returned by InfoSequenceEntry::GetWithID. The
   recovered consumers account for every byte through playerNumber: fileId is
   loaded as the sequence resource, bankId selects the instrument bank, volume
   and channelPriority are installed on the player, and the final pair is passed
   into the player-allocation path. */
struct InfoSequenceEntry {
    u32 fileId;
    u16 bankId;
    u8 volume;
    u8 channelPriority;
    u8 playerPriority;
    u8 playerNumber;

    static InfoSequenceEntry *GetWithID(u32 id);
};

/* Instrument-bank archive metadata. Every recovered consumer treats the first
   word as the bank file and the following four halfwords as optional wave
   archive IDs, with 0xffff marking an unused slot. */
struct InfoInstrumentBankEntry {
    u32 fileId;
    u16 waveArchiveIds[4];

    static InfoInstrumentBankEntry *GetWithID(u32 id);
};

struct Player {
    /* STATIC, and the bytes are what say so. A non-static member would take
     * `this` in r0; the ROM body multiplies r0 by 0x1c and uses it as a table
     * index, so r0 is the first int and there is no this. The mangling cannot
     * distinguish the two -- _ZN5Sound6Player19SetPlayableSeqCountEii is the
     * same for both -- so this is settled by the disassembly alone. */
    static void SetPlayableSeqCount(int index, int count);
};

/* The rest of the recovered Sound entry points, declared so callers stop
   hand-spelling the mangled name. Two different sources, kept straight:

     PARAMETERS come from the mangled symbol. Note what that is and is not: the
     image holds zero _Z... strings, so these names are this TREE's convention
     rather than ROM evidence (see the Fix12 note in types.h). What they are is
     the one spelling every existing caller already agrees on, which is what a
     shared declaration needs. Where the convention and the bytes disagree the
     bytes win -- so `5Fix12IiE` is declared Fix12i, the plain s32 the callers
     actually pass, not the template the name implies.

     Sound::PlaySub and Sound::ChangeMusicVolume are deliberately NOT here. Their
     definitions are mangled-name C symbols carrying `5Fix12IiE`, and no type this
     tree defines mangles to that (types.h refuses to make Fix12 a template), so a
     namespace-qualified call cannot reach them. Callers keep the extern spelling.

     RETURN TYPES come from each DEFINITION in src/, never from a caller. A
     return type is not part of an Itanium mangled name, so the symbol is
     silent on it and a caller that ignores the result evidences nothing.

   Parameter NAMES are mostly not recoverable -- the definitions carry
   placeholders -- so only the ones a definition actually names are named here.

   PlaySub and ChangeMusicVolume are defined as `extern "C"` functions literally
   named by the mangled symbol, so nothing type-checks across that seam and a
   wrong declaration here would link silently. Their call sites are byte-verified
   individually, which is what actually settles them. */


void Play(u32, u32, const Vector3 &v);
void Play2D(u32, u32);
void PlayBank0(u32 id, const Vector3 &pos);
u32  PlayBank2_2D(u32);
u32  PlayCharVoice(u32, u32, const Vector3 &v);
int  PlayLong(u32 handle, u32, u32, const Vector3 &pos, s16);
int  PlaySecretSound(dActor_c *actor, u16 *counter);

void PauseMusic();
void UnpauseMusic();
void LoadInitialGroup(int group);
void LoadAndSetMusic_Layer1(int);
void StopLoadedMusic_Layer1(u32);
void StopLoadedMusic_Layer2();

void UnsetPlayerVoiceGroup();

}

#endif /* __cplusplus */

#endif /* SOUND_H */
