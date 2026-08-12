#ifndef SHADOWMODEL_H
#define SHADOWMODEL_H

#include "types.h"
#include "BMD_File.h"
#include "ModelBase.h"
#include "math/Matrix.h"
#include "math/Fix12.h"

/* ModelBase sibling for drop shadows, vtable _ZTV11ShadowModel at
 * 0x0208e868:
 *
 *   slot 0  0x02015ff8  ~ShadowModel (D1)
 *   slot 1  0x02015f80  ~ShadowModel (D0)
 *   slot 2  0x02015ef4  DoSetFile(char *, int, int)
 *
 * Every live ShadowModel sits on a global intrusive doubly-linked list
 * (head data_0209cef4, freeze flag data_0209ceec): InitModel links in,
 * the destructor unlinks, RenderAll walks it and CleanAll empties it.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND D1 IS A REAL METHOD -- the
 * key-function arrangement from include/ModelBase.h, and the objisolate
 * exemption to it recorded there. That matters more than usual here, because
 * D1 carries the unlink logic and is now the only place it is written.
 * D0 and D2 stay C files.
 *
 * LAYOUT evidence: C1 calls ModelBase::C1, stores the vptr, zeroes mat,
 * prev and next; InitModel fills mat/scale/opacity and links; the D1
 * unlink pins prev at 0x20 and next at 0x24.
 *
 * InitModel's mangled name carries 5Fix12IiES3_S3_ -- the real Fix12<int>
 * template with Itanium substitutions for the repeats. A plain int can
 * never reproduce that name, which is what include/math/Fix12.h exists
 * for. The DECLARATION below is therefore the true signature, but the
 * DEFINITION stays a mangled free function (its .c file): CW homes every
 * class-typed by-value parameter to the stack (push {r0-r3} plus reloads,
 * +0x14 bytes) under 1.2/sp2p3 and 2004/b56 alike, while the ROM reads the
 * registers directly. Until that homing lever is found, a real method body
 * cannot reproduce the bytes; callers may still use this declaration, since
 * a call emits the same S3_ symbol either way. See notes/mwccarm-codegen.md.
 */

#ifdef __cplusplus

struct ShadowModel : ModelBase {
    ModelComponents *data;     /* 0x08 - pool entry from func_02016e70 */
    Matrix4x3 *mat;            /* 0x0c */
    Vector3 scale;             /* 0x10 */
    u8 opacity;                /* 0x1c */
    u8 unk_1d;
    u8 unk_1e;
    u8 unk_1f;
    ShadowModel *prev;         /* 0x20 */
    ShadowModel *next;         /* 0x24 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~ShadowModel();                           /* slots 0 (D1), 1 (D0) */
    virtual int DoSetFile(char *file, int a, int b);  /* slot 2 */

    /* --- non-virtual --- */
    void InitModel(Matrix4x3 *m, Fix12<int> sx, Fix12<int> sy, Fix12<int> sz,
                   u32 opacity);   /* defined as a free function, see above */
    /* RETURNS int, not void: 0x02015ebc ends in `bx ip`, a tail call, so the
       callee's r0 is this function's. Same evidence as ModelBase::SetFile. */
    int InitCylinder();
    /* int for the same reason, and by the same evidence: 0x02015ed8 also ends in
       `bx ip`, so SetFile's r0 flows straight out. YoshiEgg::InitResources and
       func_ov091_02133254 both test the result and bail on 0. */
    int InitCuboid();

    /* --- static --- */
    static void RenderAll();
    static void CleanAll();
};

typedef char ShadowModel_size_must_be_0x28[sizeof(ShadowModel) == 0x28 ? 1 : -1];

#else

struct ShadowModel {
    void **vtable;                     /* 0x00 */
    struct BMD_File *modelFile;        /* 0x04 */
    struct ModelComponents *data;      /* 0x08 */
    struct Matrix4x3 *mat;             /* 0x0c */
    struct Vector3 scale;              /* 0x10 */
    u8 opacity;                        /* 0x1c */
    u8 unk_1d;
    u8 unk_1e;
    u8 unk_1f;
    struct ShadowModel *prev;          /* 0x20 */
    struct ShadowModel *next;          /* 0x24 */
};

#endif /* __cplusplus */

#endif /* SHADOWMODEL_H */
