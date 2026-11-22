#ifndef DEXTBLENDANMMODEL_C_H
#define DEXTBLENDANMMODEL_C_H

#include "types.h"
#include "dExtAnmModel_c.h"

/* dExtAnmModel_c child that cross-fades between the previous pose and the new
 * animation, vtable _ZTV19dExtBlendAnmModel_c at 0x0208e94c (same seven-slot
 * shape as dExtAnmModel_c, every slot overridden) plus its own secondary
 * dExtFrameCtrl_c table at _ZTV19dExtBlendAnmModel_c + 0x24
 * (VTable_dExtFrameCtrl_c_dExtBlendAnmModel_cThunk).
 *
 * THE DESTRUCTOR IS DECLARED FIRST -- see include/dExtAnmModel_c.h; the D1
 * additionally frees mBlendBuffer before chaining to dExtAnmModel_c::D2. Defined
 * out of line in src/engine/model/d_ext_model.cpp. That definition is
 * the key function, so the TU also emits the MI vtable group with its RTTI,
 * and the retail table stays externalized (see the TU manifest).
 *
 * LAYOUT: dExtAnmModel_c 0x00..0x64, then the blend state. blendWeight ramps
 * 0 -> 0x1000 (1.0) by blendStep per Advance; below 1.0 UpdateVerts and
 * Virtual10 route through the blending func_0204531c instead of
 * UpdateVertsUsingBones. mBlendBuffer is a buffer the destructor frees and
 * InitBlendBuffer (DoSetFile's tail) sets up.
 *
 * SetAnim is a real method taking the scalar fx32 speed; it reproduces
 * 0x020163e0/0x9c.
 */

struct dExtBlendAnmModel_c : dExtAnmModel_c {
    fx32 blendWeight;   /* 0x64 - 0x1000 is 1.0 */
    fx32 blendStep;     /* 0x68 - per-frame increment */
    void *mBlendBuffer; /* 0x6c - pose blend buffer; DoSetFile tail allocates it, D1 frees it */

    /* --- vtable order. Do not reorder. --- */
    virtual ~dExtBlendAnmModel_c(); /* slots 0 (D1), 1 (D0) */
    virtual int DoSetFile(char *file, int a, int b);      /* slot 2 */
    virtual void UpdateVerts();                           /* slot 3 */
    virtual void Virtual10(Matrix4x3 &mat);               /* slot 4 */
    virtual void Render(const Vector3 *scale);            /* slot 5 */
    virtual void Virtual18(u32 mat, const Vector3 *scale);/* slot 6 */

    /* DECLARED, defined out of line in src/engine/model/d_ext_model.cpp
     * as real C++ -- complete-object context for every ROM caller (it is a
     * member subobject in daKing_Donketu_c/daIwante_c/daJango_c/daMoray_c...), hence C1.
     * Body is empty; blendWeight and mBlendBuffer are initialised through the init
     * list so their stores land where the ROM has them. */

    /* --- non-virtual --- */
    dExtBlendAnmModel_c();
    void Advance();
    void SetAnim(BCA_File &animFile, int numBlendFrames, int flags, fx32 speed, u16 startFrame);
    int InitBlendBuffer(char *); /* DoSetFile's tail (0x020165c4); coined */
};

typedef char dExtBlendAnmModel_c_size_must_be_0x70[sizeof(dExtBlendAnmModel_c) == 0x70 ? 1 : -1];

#endif /* DEXTBLENDANMMODEL_C_H */
