#ifndef DS_WIDTH_H
#define DS_WIDTH_H

/* Pointer width for a 64-bit host.
 *
 * The matching build is 32-bit ARM. A pointer and a u32 are the same size
 * there, so every macro below expands to nothing or to the cast the ROM
 * was built with. mwccarm never sees a type from this header.
 *
 * A 64-bit host has 8-byte pointers. Two layouts have to stay 4 bytes wide
 * anyway, because they are the bytes of a file or of a record the ARM7
 * reads: file-relative slots (BMD, BCA, BTP, KCL, level object tables) and
 * the DS bus addresses in FSOverlayInfo. Write `T * DS_P32 name` for those
 * slots. MSVC's __ptr32 keeps the slot 4 bytes and truncates on store.
 * DS bus addresses sit below 0x08000000, so the sign bit stays clear when
 * the slot is widened back to a native pointer.
 *
 * An in-place rebase stores the sum back into that 4-byte slot, and
 * SharedFilePtr::filePtr is the same kind of slot: the loaded buffer is
 * stored in 4 bytes. On a 64-bit host every address written through DS_P32
 * has to sit in the low 4GB (the game heap and the file image both). A
 * native pointer field is the other kind: list links, OSThread, a C++
 * object. Those widen. A virtual member is an 8-byte vptr the compiler
 * emits. Named members track that. A numeric offset from a byte pointer
 * does not.
 *
 * Do not put a struct, a class, or a typedef in this header. mwccarm numbers
 * file-local types from parse position, and a new type here would move
 * @class$ / Bundle$ in every includer.
 */

#if defined(SM64DS_PLATFORM_PC) && (defined(_WIN64) || defined(__LP64__) || defined(_LP64))
#define SM64DS_HOST_64 1
#endif

#if defined(SM64DS_HOST_64)
#include <stdint.h>
#endif

#if defined(SM64DS_HOST_64) && defined(_MSC_VER)
#define DS_P32 __ptr32
#elif defined(SM64DS_HOST_64)
/* The port compiler is MSVC. Another 64-bit compiler has no __ptr32, so the
   slot stays a native pointer and the file-image size locks fail closed. */
#define DS_P32
#else
#define DS_P32
#endif

/* Integer wide enough to hold an address. On the matching build these are
   the (int) and (u32) casts the call already wrote. The extra parentheses
   are part of the macro; a use has to be matched, not assumed identical. */
#if defined(SM64DS_HOST_64)
#define DS_IADDR(p) ((intptr_t)(p))
#define DS_UADDR(p) ((uintptr_t)(p))
#else
#define DS_IADDR(p) ((int)(p))
#define DS_UADDR(p) ((u32)(p))
#endif

#endif /* DS_WIDTH_H */
