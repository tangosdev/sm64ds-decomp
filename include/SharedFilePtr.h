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
 * No fields, deliberately. The layout is NOT recovered: files that declare this struct
 * locally disagree about it -- {int a, file;}, {int data[4];}, {int f0; void *f4;} and
 * {int h;} all appear -- each sized to whatever pointer arithmetic that file needed.
 * Committing to one here would silently change the others' indexing, so those files
 * keep their local declaration and are left out of this change until the real layout
 * is recovered. Callers converted here only call methods through a pointer.
 */
struct SharedFilePtr
{
    void Construct(unsigned int fileID);
    void ReallocateModelFile();
    void Release();
    void LoadFile();
    void Load();
};

#endif
