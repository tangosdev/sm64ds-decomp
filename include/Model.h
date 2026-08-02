#ifndef MODEL_H
#define MODEL_H

#include "types.h"
#include "BMD_File.h"
#include "ModelBase.h"
#include "math/Matrix.h"

/* The concrete model class, vtable _ZTV5Model at 0x0208e90c.
 *
 * VTABLE is read out of the ROM:
 *
 *   slot 0  0x02016d20  ~Model (D1)
 *   slot 1  0x02016ce0  ~Model (D0)
 *   slot 2  0x02016bf8  DoSetFile(char *, int, int)
 *   slot 3  0x02016c98  UpdateVerts()
 *   slot 4  0x02016bb8  Virtual10(Matrix4x3 &)
 *   slot 5  0x02016b78  Render(Vector3 const *)
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD, for the
 * same key-function reason as ModelBase (see include/ModelBase.h): the
 * D0/D1/D2 bodies stay C files so no TU ever emits _ZTV5Model, which the
 * module's gap object already supplies from ROM data.
 *
 * LAYOUT evidence: Model::C2 calls ModelBase::C1, stores _ZTV5Model, copies
 * mat4x3 from data_02082128 and zeroes +0x4c. Model::DoSetFile allocates
 * transformsBuf with Memory::operator_new2 and the destructors operator
 * delete it. Model::Render multiplies mat4x3 into a stack temp before
 * handing it to ModelComponents::Render.
 *
 * The static loaders (LoadFile, LoadTexAndPal, UpdateFileOffsets, the VRAM
 * routines) carry no this at all -- their mangled names take only the file
 * or size arguments -- which is why they are static members here.
 * LoadCompressedTextureToVram is declared but its definition stays at its
 * proven compiler floor (NONMATCHING terminal, see the file); a declaration
 * cannot change that file's codegen.
 */

#ifdef __cplusplus

struct SharedFilePtr;

struct Model : ModelBase {
    ModelComponents data;      /* 0x08 - embedded, unlike its pointer-holding siblings */
    Matrix4x3 mat4x3;          /* 0x1c */
    void *transformsBuf;       /* 0x4c - owned; sized by func_02046564(file) */

    /* --- vtable, in _ZTV5Model order. Do not reorder. --- */
    virtual ~Model();                                 /* slots 0 (D1), 1 (D0) */
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

typedef char Model_size_must_be_0x50[sizeof(Model) == 0x50 ? 1 : -1];

#else

/* The same object for C translation units, vptr written out explicitly. */
struct Model {
    void **vtable;                     /* 0x00 */
    struct BMD_File *modelFile;        /* 0x04 */
    struct ModelComponents data;       /* 0x08 */
    struct Matrix4x3 mat4x3;           /* 0x1c */
    void *transformsBuf;               /* 0x4c */
};

#endif /* __cplusplus */

#endif /* MODEL_H */
