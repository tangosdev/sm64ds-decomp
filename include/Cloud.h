/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Cloud: 5 matched functions, 1 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CLOUD_H
#define CLOUD_H
#include "types.h"
#include "Model.h"

struct Cloud {
    u8  pad_000[0xd4];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. It is the last thing the
       header declares, so the marker had no pad and the struct simply ends 0x50 further
       on; nothing beyond it is claimed. */
    Model mModel;            /* 0x0d4 */
#ifdef __cplusplus
    /* methods */
    int InitResources();
    int Render();
#endif
};

#endif
