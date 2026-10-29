#ifndef DEXTSHADOWMODEL_C_H
#define DEXTSHADOWMODEL_C_H

#include "types.h"
#include "BMD_File.h"
#include "dExtModel_c.h"
#include "math/Matrix.h"
#include "math/Fix12.h"

/* dExtModel_c sibling for drop shadows, vtable _ZTV17dExtShadowModel_c at
 * 0x0208e868:
 *
 *   slot 0  0x02015ff8  ~dExtShadowModel_c (D1)
 *   slot 1  0x02015f80  ~dExtShadowModel_c (D0)
 *   slot 2  0x02015ef4  DoSetFile(char *, int, int)
 *
 * Every live dExtShadowModel_c sits on a global intrusive doubly-linked list
 * (head data_0209cef4, freeze flag data_0209ceec): InitModel links in,
 * the destructor unlinks, RenderAll walks it and CleanAll empties it.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND D1 IS A REAL METHOD -- the
 * key-function arrangement from include/dExtModel_c.h, and the objisolate
 * exemption to it recorded there. That matters more than usual here, because
 * D1 carries the unlink logic and is now the only place it is written.
 * D0 and D1 are both retained from that real destructor definition, so the
 * compiler owns their vptr reset, dExtModel_c teardown and deleting-wrapper call.
 * The ROM has no separately enrolled D2 symbol for this class.
 *
 * LAYOUT evidence: C1 calls dExtModel_c::C2, stores the vptr, zeroes mat,
 * prev and next; InitModel fills mat/scale/opacity and links; the D1
 * unlink pins prev at 0x20 and next at 0x24.
 *
 * CORRECTED 2026-10-26. This comment used to argue that InitModel's mangled
 * name carries 5Fix12IiES3_S3_, so a plain int "can never reproduce that
 * name", so the definition had to stay a mangled free function to dodge wall
 * 6az's by-value homing. That reasoning was circular: the name was ours. The
 * stripped cartridge contains no _Z strings and no 5Fix12IiE anywhere (arm9
 * plus all 105 overlays), so there is no ROM evidence that this parameter was
 * ever a class type -- only that it is a 20.12 scalar, which Fix12i says. With
 * Fix12i the DEFINITION is a real method and reproduces 0x02015e64/0x58
 * exactly. See notes/mwccarm-codegen.md 6az.
 */

#ifdef __cplusplus

struct dExtShadowModel_c : dExtModel_c {
    ModelComponents *data;     /* 0x08 - pool entry from func_02016e70 */
    Matrix4x3 *mat;            /* 0x0c */
    Vector3 scale;             /* 0x10 */
    u8 opacity;                /* 0x1c */
    u8 unk_1d;
    u8 unk_1e;
    u8 unk_1f;
    dExtShadowModel_c *prev;         /* 0x20 */
    dExtShadowModel_c *next;         /* 0x24 */

    /* DECLARED, NEVER DEFINED HERE -- same reasoning as dExtSimpleModel_c (include/dExtSimpleModel_c.h)
       and dExtModel_c: undeclared, the compiler synthesises this constructor and
       INLINES it into every holder; the ROM calls _ZN17dExtShadowModel_cC1Ev
       (0x02016068) out of line instead. */
    dExtShadowModel_c();

    /* --- vtable, in ROM order. Do not reorder. --- */
    /* The destructor pair spelled as two plain virtuals on the host, plus the
       non-virtual destructor declaration the src/ definition needs; the whole
       ruling is in include/dExtModel_c.h. Overrides take their base's slots, so
       these carry the SAME TWO NAMES dExtModel_c declares. */
#ifdef _MSC_VER
    virtual void Destructor1();                       /* slot 0 (D1) */
    virtual void Destructor0();                       /* slot 1 (D0) */
    ~dExtShadowModel_c();                                   /* no slot */
#else
    virtual ~dExtShadowModel_c();                           /* slots 0 (D1), 1 (D0) */
#endif
    virtual int DoSetFile(char *file, int a, int b);  /* slot 2 */

    /* --- non-virtual --- */
    /* opacity is u8, not the u32 the old name claimed: 0x02015e78 reads the
       fifth argument with `ldrb r2, [sp, #4]`. */
    void InitModel(Matrix4x3 *m, Fix12i sx, Fix12i sy, Fix12i sz, u8 opacity);
    /* RETURNS int, not void: 0x02015ebc ends in `bx ip`, a tail call, so the
       callee's r0 is this function's. Same evidence as dExtModel_c::SetFile. */
    int InitCylinder();
    /* int for the same reason, and by the same evidence: 0x02015ed8 also ends in
       `bx ip`, so SetFile's r0 flows straight out. daYegg_c::InitResources and
       daDsnBase_c::Init both test the result and bail on 0. */
    int InitCuboid();

    /* --- static --- */
    static void RenderAll();
    static void CleanAll();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dExtShadowModel_c_size_must_be_0x28[sizeof(dExtShadowModel_c) == 0x28 ? 1 : -1];
#endif

#else

struct dExtShadowModel_c {
    void **vtable;                     /* 0x00 */
    struct BMD_File *modelFile;        /* 0x04 */
    struct ModelComponents *data;      /* 0x08 */
    struct Matrix4x3 *mat;             /* 0x0c */
    struct Vector3 scale;              /* 0x10 */
    u8 opacity;                        /* 0x1c */
    u8 unk_1d;
    u8 unk_1e;
    u8 unk_1f;
    struct dExtShadowModel_c *prev;          /* 0x20 */
    struct dExtShadowModel_c *next;          /* 0x24 */
};

/* In C the tag alone is not a type name, so an owner header that embeds a
   dExtShadowModel_c BY VALUE -- which several of the cartridge's own destructors prove
   it does, see tools/dtor_members.py -- cannot spell the member without this.
   The definition and the typedef have to travel together: with the definition
   and no typedef the embed gets `undefined identifier', and then the owner's
   size assert gets `illegal constant expression' on top of it. */
typedef struct dExtShadowModel_c dExtShadowModel_c;

/* The C view substitutes for the C++ class only while it is the SAME SIZE. Once
   an owner embeds one by value the two branches lay that owner out differently if
   they ever disagree, and nothing else in the build compares them. */
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dExtShadowModel_c_size_must_be_0x28[sizeof(struct dExtShadowModel_c) == 0x28 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DEXTSHADOWMODEL_C_H */
