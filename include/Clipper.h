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
extern "C" void _ZN6Memory16operator_delete2EPv(void *);
#endif

/* Same 0xc coordinate layout as Vector3, but deliberately POD. A Vector3[4]
 * member makes mwccarm emit __destroy_arr in Clipper's D1/D0; the cartridge's
 * trivial destructors prove these plane records have no element destructor. */
struct ClipperPlane {
    Fix12i x, y, z;
};

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
       consecutive 0xc coordinate records are the only shape those calls can
       be; the lifecycle evidence above decides their POD spelling. */
    ClipperPlane mPlaneNormals[4];   /* 0x004 */
    u8  pad_034[0x18];
    /* SIGNED. Func_0201559C sign-extends it into a 64-bit multiply, which a u32
       cannot do; the only other user just stores to it, so nothing disagrees. */
    s32 mAspectRatio;            /* 0x04c */
    s32 mNearZ;            /* 0x050 */
    s32 mFarZ;            /* 0x054 */
    u16 mFovAngle;            /* 0x058 */
#ifdef __cplusplus
    /* methods */

    /* Declared first so the compiler emits the two ROM-retained lifecycle
       variants from the empty out-of-line body: D1 restores this vptr, while
       D0 also takes the class-specific delete path below. There is no D2 in
       the image because nothing derives from Clipper. */
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

    /* D0 calls 0x0203cbcc, Memory::operator_delete2, rather than the global
       delete at 0x0203cbf0. Inline class lookup makes that choice exactly. */
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }
#endif
};

#endif
