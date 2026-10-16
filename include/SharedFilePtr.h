#ifndef SHAREDFILEPTR_H
#define SHAREDFILEPTR_H

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
 * Layout is the one the constructor writes (func_02017e0c, also recorded in
 * include/daPiano_c_resources.h). Eight bytes, no C++ destructor: a global is
 * the zeroed ROM handle, and the startup file still fills it. A destructor
 * here would register a second cleanup the ROM does not have.
 */
struct SharedFilePtr
{
    unsigned short fileID;   /* 0x00 */
    unsigned char refCount;  /* 0x02 */
    unsigned char pad;       /* 0x03, never written */
    void *filePtr;           /* 0x04 */

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
       src/SharedFilePtr.cpp declares `void *Load();` on its own local
       struct and returns the buffer it just stashed in filePtr. The
       host seam in port/hal/fs.cpp returns the same pointer. LoadFile only
       tests it (`if (!Load()) return 0;`), which is why a wrong `int` here
       went unnoticed. A return type is not part of an Itanium mangled name,
       so this changes no symbol. */
    void *Load();
};

typedef char SharedFilePtr_size_must_be_8[sizeof(SharedFilePtr) == 8 ? 1 : -1];

#endif
