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
#ifdef __cplusplus
    /* 0x00 is the vptr, placed implicitly by the first virtual declaration.
       Vtable _ZTV7Clipper lives at 0x0208e730 (named in symbols.txt). The lone
       instance, data_0209f43c in bss, is constructed explicitly by
       __sinit_02074e84 -- which carries its own extern declaration of the
       constructor and never includes this header, so promoting the class does
       not touch that translation unit. */
#else
    u8  pad_000[0x4];       /* 0x00 - the vtable, written out for C */
#endif
    /* The four clip-plane normals, written as a run by Func_0201559C: it builds
       the frustum's four corner vectors on the stack, cross-multiplies adjacent
       pairs into 0x004/0x010/0x01c/0x028 and normalises each in place. Four
       consecutive Vector3 is the only shape those eight calls can be. */
    Vector3 mPlaneNormals[4];   /* 0x004 */
    u8  pad_034[0x18];
    /* SIGNED. Func_0201559C sign-extends it into a 64-bit multiply, which a u32
       cannot do; the only other user just stores to it, so nothing disagrees. */
    s32 mAspectRatio;            /* 0x04c */
    s32 mNearZ;            /* 0x050 */
    s32 mFarZ;            /* 0x054 */
    u16 mFovAngle;            /* 0x058 */
#ifdef __cplusplus
    /* methods */

    /* THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD -- the
       key-function arrangement from include/ModelBase.h: no TU emits the
       vtable or the D2 variant, and D0/D1 stay self-contained C files. */
    virtual ~Clipper();     /* slots 0 (D1), 1 (D0) */

    /* DECLARED, never defined as a method here -- src/_ZN7ClipperC1Ev.cpp owns
       C1 (notes/ctor-migration.md section 2). Its body calls the shared init
       helper, whose definition keeps its extern "C" spelling: the ROM symbol
       ends in Ev because it is enrolled under that literal name, not because
       the source had zero parameters, so a real member declaration would
       mangle differently and cannot be used. */
    Clipper();

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
