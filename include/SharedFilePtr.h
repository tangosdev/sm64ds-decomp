#ifndef SHAREDFILEPTR_H
#define SHAREDFILEPTR_H

#include "types.h"

struct dExtModel_c;

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
 *
 * A handle is constructed with the overlay-0 file ID and destroyed by publishing
 * its file ID, and in the ROM every handle is one of the derived kinds below.
 * The base constructor and destructor are only ever entered from a derived one: the ROM
 * keeps their base-object variants (C2 0x02017e48, D2 0x02017e34) and no caller
 * outside this unit reaches them.
 */
struct SharedFilePtr {
    u16 fileID;    /* 0x00: FAT file ID, resolved from the overlay-0 ID */
    u8 numRefs;    /* 0x02: holders that have loaded the file */
    void *filePtr; /* 0x04: the loaded file, or null */

    /* Each named prototype parameter advances mwcc's @NNN counter in every
       translation unit that includes this header, so this header names exactly
       one: the constructor's file ID. Name another, or drop this one, and the
       pinned @NNN names of every includer renumber. */
    SharedFilePtr(unsigned int fileID);
    ~SharedFilePtr();

    void ReallocateModelFile();
    void Release();
    void *LoadFile();
    /* Load's return type comes from its DEFINITION, not from a caller:
       src/SharedFilePtr.cpp's Load returns the buffer it just stashed in
       filePtr. LoadFile only
       tests it (`if (!Load()) return 0;`), which is why a wrong `int` here
       went unnoticed. A return type is not part of an Itanium mangled name,
       so this changes no symbol. */
    void *Load();
    /* Address-named. */
    void func_02017e0c(unsigned int);
    void func_02017c24();
    int func_020179b4(dExtModel_c *, int);
    void *func_0201787c();
};

typedef char SharedFilePtr_size_must_be_0x8[sizeof(SharedFilePtr) == 0x8 ? 1 : -1];

/* The handle kinds, one per file format. THE CLASS NAMES ARE COINED: none of
 * these classes is polymorphic, so the cartridge carries no RTTI for any of them
 * and no original name survives. What is measured is the hierarchy, from which
 * variants the retail link kept and who calls them:
 *
 *   - each kind's complete-object constructor (C1) and destructor (D1) is called
 *     by the static initializers and cleanup of the files that hold one, and
 *     each enters SharedFilePtr's C2 / D2;
 *   - ModelFile alone also keeps its base-object variants (C2 0x02017ae4,
 *     D2 0x02017a9c), and the only callers of those are ModelFileA's and
 *     ModelFileB's C1 / D1: both derive from ModelFile.
 *
 * mwcc emits all four variants of every constructor and destructor defined in
 * src/SharedFilePtr.cpp; the variants the cartridge does not carry are recorded
 * as deadstripped compiler output in its manifest. */

/* A KCL collision file: C1 0x02017b4c, D1 0x02017b34. */
struct ClsnFile : SharedFilePtr {
    ClsnFile(unsigned int);
    ~ClsnFile();
};

/* A BMD model file: C1 0x02017acc, C2 0x02017ae4, D1 0x02017ab4, D2 0x02017a9c. */
struct ModelFile : SharedFilePtr {
    ModelFile(unsigned int);
    ~ModelFile();
};

/* A BMD model file the minigames hand to a dExtModel_c through
   SharedFilePtr::func_020179b4: C1 0x02017a24, D1 0x02017a0c. */
struct ModelFileA : ModelFile {
    ModelFileA(unsigned int);
    ~ModelFileA();
};

/* A BCA animation file: C1 0x0201799c, D1 0x02017984. */
struct AnimFile : SharedFilePtr {
    AnimFile(unsigned int);
    ~AnimFile();
};

/* A BTP texture-sequence file: C1 0x02017934, D1 0x0201791c. */
struct TexSeqFile : SharedFilePtr {
    TexSeqFile(unsigned int);
    ~TexSeqFile();
};

/* A BMD file read for its textures and rebased through
   SharedFilePtr::func_0201787c: C1 0x020178cc, D1 0x020178b4. */
struct ModelFileB : ModelFile {
    ModelFileB(unsigned int);
    ~ModelFileB();
};

typedef char ClsnFile_size_must_be_0x8[sizeof(ClsnFile) == 0x8 ? 1 : -1];
typedef char ModelFile_size_must_be_0x8[sizeof(ModelFile) == 0x8 ? 1 : -1];
typedef char ModelFileA_size_must_be_0x8[sizeof(ModelFileA) == 0x8 ? 1 : -1];
typedef char AnimFile_size_must_be_0x8[sizeof(AnimFile) == 0x8 ? 1 : -1];
typedef char TexSeqFile_size_must_be_0x8[sizeof(TexSeqFile) == 0x8 ? 1 : -1];
typedef char ModelFileB_size_must_be_0x8[sizeof(ModelFileB) == 0x8 ? 1 : -1];

#endif
