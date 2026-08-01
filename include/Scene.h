/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Scene: 12 matched functions, 1 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SCENE_H
#define SCENE_H
#include "types.h"

/* fwd */
struct x;
struct Scene {
    u8  pad_000[0x13];
    u8  unk_013;            /* 0x013 */
#ifdef __cplusplus
    /* methods */
    Bool BeforeInitResources();
    int BeforeRender();
#endif
};

#endif
