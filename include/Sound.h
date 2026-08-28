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

struct Player {
    /* STATIC, and the bytes are what say so. A non-static member would take
     * `this` in r0; the ROM body multiplies r0 by 0x1c and uses it as a table
     * index, so r0 is the first int and there is no this. The mangling cannot
     * distinguish the two -- _ZN5Sound6Player19SetPlayableSeqCountEii is the
     * same for both -- so this is settled by the disassembly alone. */
    static void SetPlayableSeqCount(int index, int count);
};

}

#endif /* __cplusplus */

#endif /* SOUND_H */
