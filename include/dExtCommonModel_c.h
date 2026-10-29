#ifndef DEXTCOMMONMODEL_C_H
#define DEXTCOMMONMODEL_C_H

#include "types.h"
#include "BMD_File.h"
#include "dExtModel_c.h"
#include "math/Matrix.h"

/* dExtModel_c sibling with a POINTER to pooled components, vtable
 * _ZTV17dExtCommonModel_c at 0x0208e8a4:
 *
 *   slot 0  0x020161e0  ~dExtCommonModel_c (D1)
 *   slot 1  0x020161b4  ~dExtCommonModel_c (D0)
 *   slot 2  0x02016144  DoSetFile(char *, int, int)
 *
 * Three slots, so unlike dExtSimpleModel_c its Render is NON-virtual. DoSetFile takes
 * its components from the shared pool via func_02016e70 instead of building
 * them in place, which is the whole difference from dExtSimpleModel_c.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND D1 IS A REAL METHOD -- the
 * key-function arrangement from include/dExtModel_c.h, and the objisolate
 * exemption to it recorded there. D0 and D2 stay C files.
 *
 * LAYOUT evidence: C1 calls dExtModel_c::C2, stores the vptr, zeroes the
 * data pointer at +0x8 and copies mat4x3 from IDENTITY_MATRIX4X3 to +0xc. The
 * mat4x3 landing at +0xc is what pins the components as a pointer here and
 * rules the embedded struct out of dExtModel_c.
 *
 * Func_020160AC keeps its placeholder name for now, but
 * its body is the material-flag OR pass, called from DoSetFile exactly
 * where dExtSimpleModel_c::DoSetFile calls its counterpart; it waits for a
 * human-approved name. SetPolygonID's rename had airtight evidence:
 * identical callee, identical call position.
 */

#ifdef __cplusplus

struct dExtCommonModel_c : dExtModel_c {
    ModelComponents *data;     /* 0x08 - pool entry from func_02016e70 */
    Matrix4x3 mat4x3;          /* 0x0c */

    /* DECLARED, NEVER DEFINED HERE -- same reasoning as dExtSimpleModel_c (include/dExtSimpleModel_c.h)
       and dExtModel_c: undeclared, the compiler synthesises this constructor and
       INLINES it into every holder; the ROM calls _ZN17dExtCommonModel_cC1Ev
       (0x02016204) out of line instead. */
    dExtCommonModel_c();

    /* --- vtable, in ROM order. Do not reorder. --- */
    /* The destructor pair spelled as two plain virtuals on the host, plus the
       non-virtual destructor declaration the src/ definitions need; the whole
       ruling is in include/dExtModel_c.h. Overrides take their base's slots, so
       these carry the SAME TWO NAMES dExtModel_c declares. */
#ifdef _MSC_VER
    virtual void Destructor1();                       /* slot 0 (D1) */
    virtual void Destructor0();                       /* slot 1 (D0) */
    ~dExtCommonModel_c();                                   /* no slot */
#else
    virtual ~dExtCommonModel_c();                           /* slots 0 (D1), 1 (D0) */
#endif
    virtual int DoSetFile(char *file, int a, int b);  /* slot 2 */

    /* --- non-virtual --- */
    void Render(const Vector3 *scale);
    void SetPolygonID(u32 id);
    void Func_020160AC(u32 flags);   /* ORs flags into every material */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dExtCommonModel_c_size_must_be_0x3c[sizeof(dExtCommonModel_c) == 0x3c ? 1 : -1];
#endif

#else

struct dExtCommonModel_c {
    void **vtable;                     /* 0x00 */
    struct BMD_File *modelFile;        /* 0x04 */
    struct ModelComponents *data;      /* 0x08 */
    struct Matrix4x3 mat4x3;           /* 0x0c */
};

#endif /* __cplusplus */

#endif /* DEXTCOMMONMODEL_C_H */
