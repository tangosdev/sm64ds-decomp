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

namespace Sound {

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
