/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class CutsceneObject: 6 matched functions, 5 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CUTSCENEOBJECT_H
#define CUTSCENEOBJECT_H
#include "types.h"

struct CutsceneObject {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u8  pad_00c[0x74];
    u8  unk_080;            /* 0x080 */
    u8  pad_081[0x5b];
    s32 mModel;            /* 0x0dc */
    u8  unk_0e0;            /* 0x0e0 */
    u8  pad_0e1[0x21];
    u8  unk_102;            /* 0x102 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
