/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class LightBeam: 5 matched functions, 3 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef LIGHTBEAM_H
#define LIGHTBEAM_H
#include "types.h"
#include "Model.h"
#include "dCcAcPos_c.h"

struct LightBeam {
    u8  pad_000[0xd4];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    dCcAcPos_c mdCcAcPos_c;   /* 0x124 */
    u8  pad_164[0x4];
    u16 mSoundTimer;              /* 0x168 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();                  /* slot  3 */
    int InitResources();
    int Render();
#endif
};

typedef char LightBeam_size_must_be_0x16c[sizeof(struct LightBeam) == 0x16c ? 1 : -1];

#endif
