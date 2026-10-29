#ifndef DEXTSIMPLEMODEL_C_H
#define DEXTSIMPLEMODEL_C_H

#include "types.h"
#include "BMD_File.h"
#include "dExtModel_c.h"
#include "math/Matrix.h"

/* The concrete model class, vtable _ZTV17dExtSimpleModel_c at 0x0208e90c.
 *
 * VTABLE is read out of the ROM:
 *
 *   slot 0  0x02016d20  ~dExtSimpleModel_c (D1)
 *   slot 1  0x02016ce0  ~dExtSimpleModel_c (D0)
 *   slot 2  0x02016bf8  DoSetFile(char *, int, int)
 *   slot 3  0x02016c98  UpdateVerts()
 *   slot 4  0x02016bb8  Virtual10(Matrix4x3 &)
 *   slot 5  0x02016b78  Render(Vector3 const *)
 *
 * THE DESTRUCTOR IS DECLARED FIRST. The promoted TU
 * (src/engine/model/d_ext_model.cpp) defines it once; mwcc emits D2/D0/D1 together
 * and all three stay enrolled. The key-function vtable emitted beside it is
 * stripped and rebound to the ROM's carved-out _ZTV17dExtSimpleModel_c, as described in
 * dExtModel_c.h.
 *
 * LAYOUT evidence: dExtSimpleModel_c::C2 calls dExtModel_c::C2, stores _ZTV17dExtSimpleModel_c, copies
 * mat4x3 from IDENTITY_MATRIX4X3 and zeroes +0x4c. dExtSimpleModel_c::DoSetFile allocates
 * transformsBuf with Memory::operator_new2 and the destructors operator
 * delete it. dExtSimpleModel_c::Render multiplies mat4x3 into a stack temp before
 * handing it to ModelComponents::Render.
 *
 * The static loaders (LoadFile, LoadTexAndPal, UpdateFileOffsets, the VRAM
 * routines) carry no this at all -- their mangled names take only the file
 * or size arguments -- which is why they are static members here.
 * LoadCompressedTextureToVram returns the pre-bump texel cursor, the same u32
 * its sibling LoadTextureToVram returns; that return is what the matched
 * definition needed, and the declaration here had it right all along.
 */

#ifdef __cplusplus

struct SharedFilePtr;

struct dExtSimpleModel_c : dExtModel_c {
    ModelComponents data;      /* 0x08 - embedded, unlike its pointer-holding siblings */
    Matrix4x3 mat4x3;          /* 0x1c */
    void *transformsBuf;       /* 0x4c - owned; sized by func_02046564(file) */

    /* DECLARED, NEVER DEFINED HERE, and that is the point. Left undeclared, the
       compiler synthesises a constructor and INLINES it into every class that
       holds a dExtSimpleModel_c, emitting the base-chain vtable stores straight into the
       holder. The ROM calls _ZN17dExtSimpleModel_cC1Ev (0x02016d58) out of line instead, so
       the declaration is what makes a holder's constructor reproduce. */
    dExtSimpleModel_c();

    /* --- vtable, in _ZTV17dExtSimpleModel_c order. Do not reorder. --- */
    /* The destructor pair spelled as two plain virtuals on the host, plus the
       non-virtual destructor declaration the src/ definitions need; the whole
       ruling, and the ROM-vs-MSVC layout measurement behind it, is in
       include/dExtModel_c.h. An override takes its base's slots, so these carry
       the SAME TWO NAMES the base declares -- a fresh name would append a slot
       instead of claiming one. */
#ifdef _MSC_VER
    virtual void Destructor1();                       /* slot 0 (D1) */
    virtual void Destructor0();                       /* slot 1 (D0) */
    ~dExtSimpleModel_c();                             /* no slot */
#else
    virtual ~dExtSimpleModel_c(); /* slots 0 (D1), 1 (D0) */
#endif
    virtual int DoSetFile(char *file, int a, int b);  /* slot 2 */
    virtual void UpdateVerts();                       /* slot 3 */
    virtual void Virtual10(Matrix4x3 &mat);           /* slot 4 */
    virtual void Render(const Vector3 *scale);        /* slot 5 */

    /* --- non-virtual --- */
    void HideMaterial(int boneID, int listIdx);
    void ShowMaterial(int boneID, int listIdx);
    void SetPolygonID(int id);
    void SetPolygonMode(int mode);
    void LoadAndSetFile(u16 ov0ID, int a, int b);

    /* --- static --- */
    static void *LoadFile(SharedFilePtr &ptr);
    static void *AddToCommonModelDataArr(BMD_File &file);
    static void LoadTexAndPal(BMD_File &file);
    static void UpdateFileOffsets(BMD_File &file);
    static u32 GetVramOffset(u32 size);
    static u32 LoadTextureToVram(char *texData, u32 size);
    static u32 LoadCompressedTextureToVram(char *src, u32 size, char *dst);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dExtSimpleModel_c_size_must_be_0x50[sizeof(dExtSimpleModel_c) == 0x50 ? 1 : -1];
#endif

#else

/* The same object for C translation units, vptr written out explicitly. */
struct dExtSimpleModel_c {
    void **vtable;                     /* 0x00 */
    struct BMD_File *modelFile;        /* 0x04 */
    struct ModelComponents data;       /* 0x08 */
    struct Matrix4x3 mat4x3;           /* 0x1c */
    void *transformsBuf;               /* 0x4c */
};

/* So an object header declaring a dExtSimpleModel_c member reads the same in both modes: C++
 * gets the class, C gets the flat stand-in above, and neither needs `struct`. */
typedef struct dExtSimpleModel_c dExtSimpleModel_c;

#endif /* __cplusplus */

#endif /* DEXTSIMPLEMODEL_C_H */
