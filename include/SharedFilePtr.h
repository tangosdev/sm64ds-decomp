#ifndef SHAREDFILEPTR_H
#define SHAREDFILEPTR_H

#include "types.h"

/* A handle to a file shared between actors: callers hold one, ask it to load, and
 * release it when the actor is cleaned up.
 *
 * Declared as the real class rather than as `extern void _ZN13SharedFilePtr7ReleaseEv(
 * void *)`, which is how most call sites still reach these. That spelling only works by
 * accident: in a C translation unit the identifier is emitted verbatim, but a file
 * compiled as C++ mangles it a SECOND time and emits a reference to
 * _Z28_ZN13SharedFilePtr7ReleaseEvPv, which nothing defines -- so the file matches and
 * can never be enrolled into the ROM build. Declaring the member lets the compiler
 * produce _ZN13SharedFilePtr7ReleaseEv the ordinary way.
 *
 * The layout comes from the class's own translation unit, src/SharedFilePtr.cpp:
 * every member defined there reads it. func_02017e0c stores the FAT file ID with a
 * halfword store and clears the reference count with a byte store; Load caches the
 * buffer in the word at +4; LoadFile counts holders in the byte at +2; Release frees
 * the buffer when that count reaches zero. Eight bytes, word-aligned.
 */
struct SharedFilePtr {
    u16 fileID;    /* 0x00: FAT file ID, resolved from the overlay-0 ID */
    u8 numRefs;    /* 0x02: holders that have loaded the file */
    void *filePtr; /* 0x04: the loaded file, or null */

    /* RETURN TYPES ARE MEASURED, not chosen: both of these set r0 on the way out
       and the C forms that defined them returned a value, so declaring them void
       made the declaration and the definition disagree -- harmless while nothing
       was defined as a real method, and blocking as soon as one is. A return type
       is not part of an Itanium mangled name, so no symbol changes here. */
    SharedFilePtr &Construct(unsigned int fileID);
    void ReallocateModelFile();
    void Release();
    void *LoadFile();
    /* Load's return type comes from its DEFINITION, not from a caller:
       src/SharedFilePtr.cpp's Load returns the buffer it just stashed in
       filePtr. The host seam in port/hal/fs.cpp returns the same pointer. LoadFile only
       tests it (`if (!Load()) return 0;`), which is why a wrong `int` here
       went unnoticed. A return type is not part of an Itanium mangled name,
       so this changes no symbol. */
    void *Load();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char SharedFilePtr_size_must_be_0x8[sizeof(SharedFilePtr) == 0x8 ? 1 : -1];
#endif

#endif
