/* Hand-edited, against evidence. This file used to carry the
 * "AUTO-GENERATED ... by tools/gen_header.py" banner, which stopped being true the
 * moment the __cplusplus block was corrected -- see
 * notes/runbook-type-reconstruction.md section 2.
 *
 * class Scene: 12 matched functions, 1 evidenced field.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen.
 *
 * THE __cplusplus BLOCK BELOW HAD NEVER BEEN COMPILED. It declared
 *     Bool BeforeInitResources();
 * and `Bool` is defined nowhere in the tree. The generator lifted the return type
 * verbatim out of src/_ZN5Scene19BeforeInitResourcesEv.c, which opens with its own
 * file-local `typedef int Bool;` -- a spelling that exists only inside that one file.
 * Scene.h is included by two .c files and no .cpp, so the block was always skipped and
 * the undefined type sat there unnoticed.
 *
 * Corrected to `int`, which is exactly what that typedef resolves to, so the meaning
 * is unchanged. NOT to `bool`: types.h defines `typedef int bool;` only `#ifndef
 * __cplusplus`, so `bool` is 4 bytes in C and the 1-byte keyword in C++, and using it
 * here would make the declaration mean two different things depending on the includer.
 *
 * Found by sweeping every header through the pinned compiler as C and as C++ (a header
 * that fails as C++ but passes as C has a broken __cplusplus block). This was the last
 * of three; the other two were ExpandingHeapAllocator.h's `forwards void* Allocate(...)`
 * (#1211) and SolidHeapAllocator.h's `call ResetEnd void Reset(...)`. All three are the
 * same root cause -- the generator copied a token out of a source file without checking
 * it names anything visible from the header -- in three different disguises: a spilled
 * parameter-name fragment, scraped comment text, and a file-local typedef.
 *
 * The generator also emitted `struct x;` -- a parameter name mistaken for a type name.
 * No source ever referenced it; dropped. */
#ifndef SCENE_H
#define SCENE_H
#include "types.h"

struct Scene {
    u8  pad_000[0x13];
    u8  unk_013;            /* 0x013 */
#ifdef __cplusplus
    /* methods */
    int BeforeInitResources();   /* 0 on failure, 1 on success */
    int BeforeRender();
#endif
};

#endif
