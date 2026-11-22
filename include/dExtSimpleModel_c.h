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
 * transformsBuf with operator new[] and the destructors operator delete[]
 * it. dExtSimpleModel_c::Render multiplies mat4x3 into a stack temp before
 * handing it to ModelComponents::Render.
 *
 * The static loaders (LoadFile, LoadTexAndPal, UpdateFileOffsets, the VRAM
 * routines) carry no this at all -- their mangled names take only the file
 * or size arguments -- which is why they are static members here.
 * LoadCompressedTextureToVram returns the pre-bump texel cursor, the same u32
 * its sibling LoadTextureToVram returns; that return is what the matched
 * definition needed, and the declaration here had it right all along.
 */

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
    virtual ~dExtSimpleModel_c(); /* slots 0 (D1), 1 (D0) */
    virtual int DoSetFile(char *file, int a, int b);  /* slot 2 */
    virtual void UpdateVerts();                       /* slot 3 */
    virtual void Virtual10(Matrix4x3 &mat);           /* slot 4 */
    virtual void Render(const Vector3 *scale);        /* slot 5 */

    /* --- non-virtual --- */
    void HideMaterial(int boneID, int listIdx);
    void ShowMaterial(int boneID, int listIdx);
    void SetPolygonID(int id);
    /* ANDs ~mask / ORs mask into the flags of every material. The parameters
       stay unnamed: a named parameter here renumbers every consumer's @NNN. */
    void ClearMaterialFlags(u32);
    void SetMaterialFlags(u32);
    void SetPolygonMode(int mode);
    void LoadAndSetFile(u16 ov0ID, int a, int b);
    void func_02016aac(int, int);
    void func_02016a14(int);
    void func_02016a04(int);
    void func_020169f4(int);
    void func_020169d8(int, unsigned int);

    /* --- static --- */
    static BMD_File *LoadFile(SharedFilePtr &ptr);
    static void *AddToCommonModelDataArr(BMD_File &file);
    static void LoadTexAndPal(BMD_File &file);
    static void UpdateFileOffsets(BMD_File &file);
    static u32 GetVramOffset(u32 size);
    static u32 LoadTextureToVram(char *texData, u32 size);
    static u32 LoadCompressedTextureToVram(char *src, u32 size, char *dst);
};

typedef char dExtSimpleModel_c_size_must_be_0x50[sizeof(dExtSimpleModel_c) == 0x50 ? 1 : -1];

#endif /* DEXTSIMPLEMODEL_C_H */
