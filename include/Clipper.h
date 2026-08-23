/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Clipper: 7 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CLIPPER_H
#define CLIPPER_H
#include "types.h"

#ifdef __cplusplus
struct Matrix4x3;
#include "math/Fix12.h"
#endif

struct Clipper {
    u8  pad_000[0x4];
    /* The four clip-plane normals, written as a run by Func_0201559C: it builds
       the frustum's four corner vectors on the stack, cross-multiplies adjacent
       pairs into 0x004/0x010/0x01c/0x028 and normalises each in place. Four
       consecutive Vector3 is the only shape those eight calls can be. */
    Vector3 mPlaneNormals[4];   /* 0x004 */
    u8  pad_034[0x18];
    /* SIGNED. Func_0201559C sign-extends it into a 64-bit multiply, which a u32
       cannot do; the only other user just stores to it, so nothing disagrees. */
    s32 unk_04c;            /* 0x04c */
    s32 unk_050;            /* 0x050 */
    s32 unk_054;            /* 0x054 */
    u16 unk_058;            /* 0x058 */
#ifdef __cplusplus
    /* methods */
    void Func_0201559C();

    /* Methods whose mangled names carry a by-value class parameter (5Fix12IiE)
       are deliberately NOT definable as real methods -- see
       notes/mwccarm-codegen.md 6az. CW homes class-typed by-value parameters
       to the stack, costing +0x14, so these keep extern "C" definitions with
       scalar args. A true-signature declaration for callers is fine and is
       tracked separately. */
    int  Func_020150E8(Vector3 &v, Fix12<int> clip, u8 *hint);
    void Func_02015560(Matrix4x3 &mat, Vector3 &srcVec, Fix12<int> scale, Vector3 &dstVec);
#endif
};

#endif
