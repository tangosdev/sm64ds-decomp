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
 * NO FIELDS, deliberately. Sound::Player's only recovered method is static
 * and reaches a file-scope table, so nothing here evidences an instance
 * layout -- and inventing one would silently retype whatever else turns out
 * to live at those addresses.
 */

#ifdef __cplusplus

#include "types.h"

/* Global scope on purpose: inside `namespace Sound` this would declare
   Sound::dActor_c, which is not the type any caller is holding. */
struct dActor_c;

/* Two of the arm9 bank-bound wrappers defined in src/engine/sound/Sound.cpp,
   with their C names: func_0201267c is Sound::Play(3, id, *pos), and
   func_02012790 is Sound::Play2D(2, id). */
extern "C" {
void func_0201267c(unsigned int, const Vector3 *);
unsigned int func_02012790(unsigned int);
}

namespace Sound {

/* A bank-bound wrapper over Sound::Play, defined in
   src/engine/sound/Sound.cpp. The position is a REFERENCE -- the
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

     RETURN TYPES come from each DEFINITION in src/, never from a caller. A
     return type is not part of an Itanium mangled name, so the symbol is
     silent on it and a caller that ignores the result evidences nothing.

   Parameter NAMES are mostly not recoverable -- the definitions carry
   placeholders -- so only the ones a definition actually names are named here.
   PlaySub's parameters stay unnamed: a name here takes an @NNN in every includer. */

void Play(u32, u32, const Vector3 &v);
u32 Play2D(u32, u32);
void PlayBank0(u32 id, const Vector3 &pos);
u32  PlayBank2_2D(u32);
u32 PlayBank3_2D(u32);
u32  PlayCharVoice(u32, u32, const Vector3 &v);
int  PlayLong(u32 handle, u32, u32, const Vector3 &pos, s16);
bool PlaySub(u32, u32, u32, Fix12i, bool);
/* A named parameter here numbers @NNN in every includer. */
int ChangeMusicVolume(u32, Fix12i);
int  PlaySecretSound(dActor_c *actor, u16 *counter);

void PauseMusic();
void UnpauseMusic();
void LoadInitialGroup(int group);
void LoadGroupAndSetBank(int, int);
void LoadAndSetMusic_Layer1(int);
void StopLoadedMusic_Layer1(u32);
void LoadAndSetMusic_Layer2(u32);
void StopLoadedMusic_Layer2();
void LoadAndSetMusic_Layer3(u32);
void StopLoadedMusic_Layer3();

/* func_02048fd4 wrappers, defined in src/engine/sound/Sound2.cpp. */
void Func_02048eb4();
void Func_02048ec4();
void Func_02048ee4();

/* The override layer Player drives: SetMusic(playerNo, music) installs a track
   (Sound.cpp ignores any player but the local one), and Sound::EndMusic takes
   it back off again. Player keeps two of these (mStateMusic and mPowerupMusic)
   and lets whichever survives restart when the other ends.

   The parameters are unnamed on purpose: a named parameter in this declaration
   takes one of the compiler's @NNN numbers in every file that includes
   Sound.h, which shifts the tu_manifest defined_symbol_renames keyed to them.

   EndMusic took a second parameter it never read, which left its symbol spelled
   _ZN5Sound8EndMusicEjj -- two `j`s for a function every ROM call site passes a
   single argument to. A truthful declaration mangled to ...Ej and matched
   nothing, so callers hand-spelled the two-parameter name and passed one
   argument. The symbol is now ...Ej and both are declared here. */
void SetMusic(u32, u32);
void EndMusic(u32);

void UnsetPlayerVoiceGroup();

}

#endif /* __cplusplus */

#endif /* SOUND_H */
