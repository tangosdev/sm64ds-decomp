//cpp
/* The dExt model family: one translation unit, arm9 .text 0x0201577c..0x020171c8.

   Eleven classes, all under their cartridge RTTI names:
     dExtModel_c          the root (pure-virtual DoSetFile)
     dExtSimpleModel_c    dExtModel_c + an in-place ModelComponents
     dExtAnmModel_c       dExtSimpleModel_c + dExtFrameCtrl_c at +0x50
     dExtBlendAnmModel_c  dExtAnmModel_c that cross-fades between poses
     dExtAnm2Model_c      dExtAnmModel_c with a second frame controller
     dExtCommonModel_c    dExtModel_c over a pooled ModelComponents
     dExtShadowModel_c    dExtModel_c drawing a drop shadow
     dExtFrameCtrl_c      the frame cursor every animation drives
     dExtAnmTexPat_c      dExtFrameCtrl_c over a BTP file
     dExtAnmTexSRT_c      dExtFrameCtrl_c over a BTA file
     dExtAnmMaterial_c    dExtFrameCtrl_c over a BMA file

   One object, from two independent pieces of evidence. The .text run tiles
   from dClipper's C1 (ends 0x0201577c) to dFdDummy_c (0x020171c8) with no
   foreign class between. And the .data block 0x0208e738..0x0208e9e0, between
   dClipper's vtable and dFader_c's typeinfo, holds every one of these
   classes' _ZTI, _ZTS and _ZTV records interleaved with one another, led by
   the textures-loaded flag AddToCommonModelDataArr reads -- one compiler
   output, not eleven.

   mwccarm emits .text in reverse source order under default deferred
   codegen, so the classes and the members inside each run ROM-descending.
   The six _ZThn80_ destructor thunks are generated after every function, in
   the source order of the three multiply-inheriting destructors that need
   them (dExtAnmModel_c, dExtBlendAnmModel_c, dExtAnm2Model_c); that is the
   block that closes the run at 0x02017168..0x020171c8.

   RTTI is on, as the cartridge was compiled: the records above are the
   compiler's own, and every type name the TU emits has its cartridge home
   in that .data block. The vtables and records are compared against those
   homes and discarded; this TU claims .text only. */

/* common.h comes first so this TU sees the flat Matrix4x3 { s32 m[12]; }
   spelling: the constructors' mat4x3 = IDENTITY_MATRIX4X3 copies block-move
   twelve words under it, where the structured Matrix3x3 + Vector3 spelling
   scalarizes and lands +0x1c bytes long. */
#include "common.h"
#include "dExtSimpleModel_c.h"
#include "dExtAnmModel_c.h"
#include "dExtBlendAnmModel_c.h"
#include "dExtAnm2Model_c.h"
#include "dExtCommonModel_c.h"
#include "dExtShadowModel_c.h"
#include "dExtFrameCtrl_c.h"
#include "dExtAnmTexPat_c.h"
#include "dExtAnmTexSRT_c.h"
#include "dExtAnmMaterial_c.h"
#include "Memory.h"
#include "Heap.h"

extern Matrix4x3 IDENTITY_MATRIX4X3;

extern "C" {
extern Matrix4x3 data_0209b3ec; /* local extern: the camera-space matrix the Render paths use */
extern Matrix4x3 data_020a0e68; /* local extern: dExtShadowModel_c's shared render matrix scratch */
/* local extern: no header declares it */
void MulMat4x3Mat4x3(const Matrix4x3 *m1, const Matrix4x3 *m0, Matrix4x3 *mF);
/* local extern: no header declares it; data.transforms-bone walk in Virtual10 */
void func_02045074(ModelComponents *data, BMD_Bone *bones);
/* local extern: no header declares it; sizes the transforms buffer from the file */
u32 func_02046564(BMD_File *file);
/* local extern: no header declares it; initializes ModelComponents over the file */
void func_020462d0(ModelComponents *data, BMD_File *file, void *buffer);
/* defined below; DoSetFile calls it */
void func_02016b24(dExtSimpleModel_c *model, u32 mask);
/* defined below; dExtCommonModel_c and dExtShadowModel_c take their pooled
   components from it */
ModelComponents *func_02016e70(BMD_File *file);
/* local extern: no header declares it; delete veneer for the transforms and
   blend buffers */
void func_0203cbc0(void *ptr);
#ifdef _MSC_VER
/* local extern: host only. ::operator new is the CRT's on the PC port, but
   Memory::operator_delete2 hands this block back to the DS default heap, so
   the host allocates it from that heap directly (src/engine/heap/Heap.cpp
   defines this veneer for the host). */
void *func_0203cc0c(u32 size);
#endif
/* local extern: LoadFile.c defines int(int). This is the spelling
   LoadAndSetFile's shard matched (banked return and parameter rows). */
void *LoadFile(u16 ov0ID);
/* local extern: func_020469e0.c defines int(int *). The caller's shard
   matched this spelling (banked rows). */
u32 func_020469e0(void *ptr);
/* local extern: no header declares it; dExtModel_c's destructor frees the file.
   decl_common.h's copy is the same spelling; that catch-all does not count. */
void Deallocate(void *ptr);
/* local extern: game heap pointer func_02017060 reads */
extern void *data_020a0ea0;
/* decl_common.h's int[] spelling. The records are 0xc-byte CommonModelData. */
extern int data_0209cef8[];
extern int data_0209cefc[];
/* local extern: no header declares it; CleanCommonModelDataArr clears it.
   int, not void *: the other readers declare int, and the store of 0 is
   the same word. */
extern int data_0209cef0;
/* local extern: AddToCommonModelDataArr's "textures already loaded" flag.
   A scalar, matching the other reader; [0] on an int[] is the same word
   but flips the declaration plurality onto this shorter path. */
extern int data_0208e738;
/* local extern: polygon-mode / polygon-id / opacity. Model4.cpp defines the
   first and third as (void *, void *) and (void *, unsigned); these are the
   spellings the call shards matched (banked rows). */
void func_02046234(ModelComponents *data, int mode);
void func_02046008(ModelComponents *data, int id);
void func_020461b4(ModelComponents *data, u32 a);
/* local extern: the +8 adjustors the low veneers forward to. The one-param
   spellings are deliberate under-declarations -- the thunks forward r1/r2
   untouched, which C cannot express (banked arity rows). */
void func_02045ec4(void *self, unsigned int value, int index);
void func_02045e70(char *p);
void func_02045ef8(char *p);
void func_02045f80(char *p);
void func_0204605c(char *p);
/* defined below; LoadAndSetFile calls it */
int func_02016ff4(dExtModel_c *self, BMD_File *file, int c, int d);
u32 func_02017060(void *ptr);
/* local extern: defined in src/engine/model/Model4.cpp, which types the frame
   `short`; no header declares it. Stores (file, frame) into bone slot i of the
   components' animation array. The frame stays `int` here: the ROM passes
   func_020162a4's `lsr #16` result with no sign extension, which a `short`
   parameter would add (the banked param:#4 int disagreement). */
void func_02045e2c(int obj, int i, int a, int b);
/* local extern: no header declares it; blends verts toward the old pose in
   Virtual10/UpdateVerts while blendWeight is still below 1.0 */
void func_0204531c(ModelComponents *data, s32 weight);
/* local extern: no header declares it; sizes the mBlendBuffer blend buffer off the
   file. Spelled (int *) the way func_020462bc.c defines it */
u32 func_020462bc(int *file);
/* local extern: no header declares it; initializes ModelComponents' side of
   the blend buffer. Spelled (int *, int) the way func_020462b4.c defines it */
void func_020462b4(int *data, int buf);
/* defined below -- DoSetFile's tail; it sits between UpdateVerts and
   DoSetFile in the ROM and DoSetFile is its only caller here */
int func_020165c4(dExtBlendAnmModel_c *self, char *file);
/* local extern: dExtShadowModel_c's live list head and freeze flag */
extern dExtShadowModel_c *data_0209cef4;
extern u8 data_0209ceec;
/* The two shadow model files live in the shared overlay-0/1 region: arm9's own
 * symbols.txt rows for 0x020ad524/0x020ad560 sit past .bss and name nothing
 * the link defines, so the ov000 spelling is the resolvable one. */
extern BMD_File data_ov000_020ad524;
extern BMD_File data_ov000_020ad560;
/* local extern: no header declares these; the shadow scale matrix and the two
   opacity passes RenderAll draws with */
void func_0203c178(Matrix4x3 *m, Fix12i sx, Fix12i sy, Fix12i sz);
void func_02046120(ModelComponents *data, u32 opacity);
void func_02046088(ModelComponents *data, u32 opacity, int f);
/* local extern: the texture-animation helpers. Each Prepare's ROM body is a
   0xc long-call veneer (ldr ip, [pc]; bx ip); the registers pass through
   untouched, so the helper's own two-argument signature is the call surface. */
void func_02046d50(BMD_File *model, BTP_File *file);
void func_02046bbc(ModelComponents *model, BTP_File *file, int frame);
void func_02046b64(BMD_File *model, BTA_File *file);
void func_020469e8(ModelComponents *model, BTA_File *file, int frame);
void func_020470e8(BMD_File *model, BMA_File *file);
void func_02046e28(ModelComponents *model, BMA_File *file, int frame);
}

namespace cstd {
int fdiv(int a, int b);
}

/* Only the header word the SetAnim paths read; dExtFrameCtrl_c::UpdateFileOffsets'
   own TU spells the pointer fields instead. Nothing here reads the two bytes
   ahead of the frame count, so they stay unnamed. */
struct BCA_File {
    u8 pad_00[2];
    u16 numFrames; /* 0x02 */
};

/* 0xc-byte records in the common model data array at data_0209cefc, counted
   by data_0209cef8. func_02016e70 fills the two owned slots; the destructors
   and CleanCommonModelDataArr free them. */
struct CommonModelData {
    BMD_File *file;              /* 0x00 */
    ModelComponents *components; /* 0x04 */
    void *buffer;                /* 0x08 */
};

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dExtFrameCtrl_c() it
 * emits, so this arm spells out, in terms of the destructor defined
 * below, what the deleting destructor does: the D1 body, called
 * qualified so it is a direct call, then the class-specific operator
 * delete. It sits ahead of the class's definitions so no ROM symbol's
 * region claims it. Nothing here reaches mwccarm. */
extern "C" dExtFrameCtrl_c *_ZN15dExtFrameCtrl_cD0Ev(dExtFrameCtrl_c *thiz) {
    thiz->dExtFrameCtrl_c::~dExtFrameCtrl_c();
    dExtFrameCtrl_c::operator delete(thiz);
    return thiz;
}
#endif

/* ---------------------------------------------------------------------------
   dExtModel_c and dExtSimpleModel_c, 0x020169d8..0x02017168: the
   dExtSimpleModel_c run, the dExtModel_c edge interleaved with it, and the
   unnamed helpers between them.
   --------------------------------------------------------------------------- */

/* One definition emits C1 and C2. The cartridge homes only C2 (nothing
   constructs a complete dExtModel_c), so the manifest deadstrips C1. */
// @symbol _ZN11dExtModel_cC2Ev
dExtModel_c::dExtModel_c() : modelFile(0) {}

/* One definition emits D2, D0 and D1, in that order: the vptr store is the
   compiler's; the body releases modelFile. D0 follows with the dExtModel_c
   operator delete (Memory::operator_delete2). */
// @symbol _ZN11dExtModel_cD2Ev
// @symbol _ZN11dExtModel_cD0Ev
// @symbol _ZN11dExtModel_cD1Ev
dExtModel_c::~dExtModel_c() {
    if (modelFile != 0) {
        Deallocate(modelFile);
    }
}

// @symbol func_02017060
extern "C" u32 func_02017060(void *ptr) {
    if (!ptr)
        return 0;
    void *heap = data_020a0ea0;
    ((Heap *)heap)->_Sizeof(ptr);
    u32 newSize = func_020469e0(ptr);
    return ((Heap *)heap)->Reallocate(ptr, newSize);
}

/* Loads the file offsets, calls the object's DoSetFile through vtable slot 2,
   and on success reallocs the file down to its reported size. */
// @symbol func_02016ff4
extern "C" int func_02016ff4(dExtModel_c *self, BMD_File *file, int c, int d) {
    int ret;
    if (!file)
        return 0;
    dExtSimpleModel_c::UpdateFileOffsets(*file);
    ret = self->DoSetFile((char *)file, c, d);
    if (ret)
        func_02017060(file);
    return ret;
}

// @symbol _ZN11dExtModel_c7SetFileEP8BMD_Fileii
int dExtModel_c::SetFile(BMD_File *file, int a, int b) {
    return DoSetFile((char *)file, a, b);
}

// @symbol _ZN17dExtSimpleModel_c14LoadAndSetFileEtii
void dExtSimpleModel_c::LoadAndSetFile(u16 ov0ID, int a, int b) {
    BMD_File *file = (BMD_File *)::LoadFile(ov0ID);
    modelFile = file;
    func_02016ff4(this, file, a, b);
}

// @symbol _ZN17dExtSimpleModel_c23AddToCommonModelDataArrER8BMD_File
void *dExtSimpleModel_c::AddToCommonModelDataArr(BMD_File &file) {
    CommonModelData *p = (CommonModelData *)data_0209cefc;
    int i = 0;
    int n = data_0209cef8[0];
    while (i < n) {
        if (&file == p->file)
            return p;
        i++;
        p++;
    }
    p->file = &file;
    p->components = 0;
    p->buffer = 0;
    data_0209cef8[0] = data_0209cef8[0] + 1;
    if (data_0208e738 == 0)
        return p;
    LoadTexAndPal(file);
    return p;
}

/* Get-or-create the pooled ModelComponents entry for the pointer-holding
   siblings (dExtCommonModel_c/dExtShadowModel_c). */
// @symbol func_02016e70
extern "C" ModelComponents *func_02016e70(BMD_File *file) {
    CommonModelData *e;
    void *b;

    e = (CommonModelData *)dExtSimpleModel_c::AddToCommonModelDataArr(*file);
    if (e->components == 0) {
#ifdef _MSC_VER
        e->components = (ModelComponents *)func_0203cc0c(0x14);
#else
        e->components = (ModelComponents *)::operator new(0x14);
#endif
        if (e->components == 0)
            return 0;
        e->buffer = Memory::operator_new2(func_02046564(file));
        b = e->buffer;
        if (b == 0) {
            Memory::operator_delete2(e->components);
            e->components = 0;
            return 0;
        }
        func_020462d0(e->components, file, b);
    }
    return e->components;
}

// @symbol CleanCommonModelDataArr
extern "C" void CleanCommonModelDataArr(void) {
    CommonModelData *e = (CommonModelData *)data_0209cefc;
    int i = 0;

    if (data_0209cef8[0] > 0) {
        do {
            if (e->buffer)
                func_0203cbc0(e->buffer);
            if (e->components)
                Memory::operator_delete2(e->components);
            i++;
            e++;
        } while (i < data_0209cef8[0]);
    }

    data_0209cef8[0] = 0;
    data_0209cef0 = 0;
}

/* One definition emits C1 and C2: the base-chain call to dExtModel_c::C2, the
   _ZTV17dExtSimpleModel_c vptr store, the flat twelve-word identity copy, and the
   transformsBuf zero. The cartridge keeps both variants. */
// @symbol _ZN17dExtSimpleModel_cC2Ev
// @symbol _ZN17dExtSimpleModel_cC1Ev
dExtSimpleModel_c::dExtSimpleModel_c() : transformsBuf(0) {
    mat4x3 = IDENTITY_MATRIX4X3;
}

/* One definition emits D2, D0 and D1: the vptr store and the dExtModel_c
   destructor call are the compiler's; the body is the class's one written
   obligation -- handing transformsBuf back through the delete veneer. D0
   follows with the dExtModel_c operator delete (Memory::operator_delete2). */
// @symbol _ZN17dExtSimpleModel_cD1Ev
dExtSimpleModel_c::~dExtSimpleModel_c() {
    if (transformsBuf)
        func_0203cbc0(transformsBuf);
}

// @symbol _ZN17dExtSimpleModel_c11UpdateVertsEv
void dExtSimpleModel_c::UpdateVerts() {
    data.UpdateVertsUsingBones();
}

// @symbol _ZN17dExtSimpleModel_c9DoSetFileEPcii
int dExtSimpleModel_c::DoSetFile(char *file_, int a, int b) {
    BMD_File *file = (BMD_File *)file_;
    void *buffer;
    AddToCommonModelDataArr(*file);
    transformsBuf = Memory::operator_new2(func_02046564(file));
    buffer = transformsBuf;
    if (buffer == 0)
        return 0;
    func_020462d0(&data, file, buffer);
    data.UpdateVertsUsingBones();
    if (a != 0)
        func_02016b24(this, 0x8000);
    if (b < 0)
        return 1;
    SetPolygonID(b & 0xff);
    return 1;
}

// @symbol _ZN17dExtSimpleModel_c9Virtual10ER9Matrix4x3
void dExtSimpleModel_c::Virtual10(Matrix4x3 &mat) {
    /* The copy is one flat 12-word ldm/stm run in the ROM. CW copies the
       nested Matrix4x3 (Matrix3x3 + Vector3) memberwise, which is bigger,
       so it goes through a flat 12-word view instead. */
    struct Mtx12w {
        Fix12i m[12];
    };
    *(Mtx12w *)data.transforms = *(const Mtx12w *)&mat;
    func_02045074(&data, data.modelFile->bones);
}

// @symbol _ZN17dExtSimpleModel_c6RenderEPK7Vector3
void dExtSimpleModel_c::Render(const Vector3 *scale) {
    Matrix4x3 temp;
    MulMat4x3Mat4x3(&mat4x3, &data_0209b3ec, &temp);
    data.Render(&temp, (Vector3 *)scale);
}

// @symbol func_02016b24
extern "C" void func_02016b24(dExtSimpleModel_c *model, u32 mask) {
    u32 count = model->data.modelFile->numMaterials;
    BMD_Material *p = model->data.materials;
    u32 i;
    for (i = 0; i < count; i++) {
        p->flags |= mask;
        p++;
    }
}

// @symbol func_02016acc
extern "C" void func_02016acc(dExtSimpleModel_c *model, unsigned int mask) {
    BMD_File *file = model->data.modelFile;
    unsigned int n = file->numMaterials;
    BMD_Material *q = model->data.materials;

    for (unsigned int i = 0; i < n; i++) {
        unsigned int nmask = ~mask;
        q->flags &= nmask;
        q++;
    }
}

// @symbol _ZN17dExtSimpleModel_c12SetPolygonIDEi
void dExtSimpleModel_c::SetPolygonID(int id) {
    func_02046008(&data, id);
}

// @symbol func_02016aac
extern "C" void func_02016aac(char *self) {
    func_0204605c(self + 8);
}

// @symbol _ZN11dExtModel_c12ApplyOpacityEjj
void dExtModel_c::ApplyOpacity(u32 opacity, u32) {
    func_020461b4(&static_cast<dExtSimpleModel_c *>(this)->data, opacity);
}

// @symbol _ZN17dExtSimpleModel_c14SetPolygonModeEi
void dExtSimpleModel_c::SetPolygonMode(int mode) {
    func_02046234(&data, mode);
}

// @symbol _ZN17dExtSimpleModel_c12HideMaterialEii
void dExtSimpleModel_c::HideMaterial(int boneID, int listIdx) {
    BMD_File *file = data.modelFile;
    BMD_Material *mats = data.materials;
    u8 *ids = file->bones[boneID].materialIds;
    mats[ids[listIdx]].flags |= 0x80000000;
}

// @symbol _ZN17dExtSimpleModel_c12ShowMaterialEii
void dExtSimpleModel_c::ShowMaterial(int boneID, int listIdx) {
    BMD_File *file = data.modelFile;
    BMD_Material *mats = data.materials;
    u8 *ids = file->bones[boneID].materialIds;
    mats[ids[listIdx]].flags &= 0x7fffffff;
}

// @symbol func_02016a14
extern "C" void func_02016a14(char *self) {
    func_02045f80(self + 8);
}

// @symbol func_02016a04
extern "C" void func_02016a04(char *self) {
    func_02045ef8(self + 8);
}

// @symbol func_020169f4
extern "C" void func_020169f4(char *self) {
    func_02045e70(self + 8);
}

// @symbol func_020169d8
extern "C" void func_020169d8(char *self, int index, unsigned int value) {
    func_02045ec4(self + 8, value, index);
}

/* ---------------------------------------------------------------------------
   dExtAnmModel_c, 0x02016714..0x020169d8: the method run, then D2, D0, D1,
   C1 and C2.
   --------------------------------------------------------------------------- */

/* One definition emits C1 and C2: dExtSimpleModel_c::C2, dExtFrameCtrl_c::C2 at +0x50,
   both vptr stores, and file = 0. The cartridge keeps both variants. */
// @symbol _ZN14dExtAnmModel_cC1Ev
// @symbol _ZN14dExtAnmModel_cC2Ev
dExtAnmModel_c::dExtAnmModel_c() {
    file = 0;
}

/* One definition emits D2, D0 and D1, and queues its _ZThn80_ pair for the
   end of the object. The body is empty: both vptr restores and the base
   destructor calls are the compiler's, and D0 finishes with
   dExtAnmModel_c::operator delete. The cartridge keeps all three. */
// @symbol _ZN14dExtAnmModel_cD2Ev
// @symbol _ZN14dExtAnmModel_cD0Ev
// @symbol _ZN14dExtAnmModel_cD1Ev
dExtAnmModel_c::~dExtAnmModel_c() {}

// @symbol _ZN14dExtAnmModel_c11UpdateVertsEv
void dExtAnmModel_c::UpdateVerts() {
    s32 frame = currFrame;
    data.UpdateBones(file, (u32)(frame << 4) >> 0x10);
    data.UpdateVertsUsingBones();
}

// @symbol _ZN14dExtAnmModel_c9Virtual10ER9Matrix4x3
void dExtAnmModel_c::Virtual10(Matrix4x3 &mat) {
    s32 frame = currFrame;
    data.UpdateBones(file, (u32)(frame << 4) >> 0x10);
    dExtSimpleModel_c::Virtual10(mat);
}

// @symbol _ZN14dExtAnmModel_c6RenderEPK7Vector3
void dExtAnmModel_c::Render(const Vector3 *scale) {
    UpdateVerts();
    dExtSimpleModel_c::Render(scale);
}

// @symbol _ZN14dExtAnmModel_c9Virtual18EjPK7Vector3
void dExtAnmModel_c::Virtual18(u32 mat, const Vector3 *scale) {
    Virtual10(*(Matrix4x3 *)mat);
    dExtSimpleModel_c::Render(scale);
}

/* Refresh bones without touching verts -- UpdateVerts' first half kept
   file-local; daMoray_c calls it on a dExtBlendAnmModel_c*. */
// @symbol func_020167a4
extern "C" void func_020167a4(dExtAnmModel_c *self) {
    s32 frame = self->currFrame;
    self->data.UpdateBones(self->file, (u32)(frame << 4) >> 0x10);
}

// @symbol _ZN14dExtAnmModel_c7SetAnimEP8BCA_Fileiit
void dExtAnmModel_c::SetAnim(BCA_File *animFile, int flags, Fix12i speed, u16 startFrame) {
    if (animFile == file) {
        SetFlags(flags);
        this->speed = speed;
    } else {
        file = animFile;
        static_cast<dExtFrameCtrl_c &>(*this).SetAnimation(animFile->numFrames, flags, speed,
                                                           startFrame);
    }
}

/* Both this and src adjust by +0x50 for the base call: the first real
   multiple-inheritance argument conversion in the tree. */
// @symbol _ZN14dExtAnmModel_c4CopyERKS_Pc
void dExtAnmModel_c::Copy(const dExtAnmModel_c &src, char *newFile) {
    dExtFrameCtrl_c::Copy(src);
    if (newFile != 0)
        file = (BCA_File *)newFile;
    else
        file = src.file;
}

/* ---------------------------------------------------------------------------
   dExtBlendAnmModel_c, 0x020163e0..0x02016714: the method run, then D0, D1
   and C1.
   --------------------------------------------------------------------------- */

/* One definition emits C1 and C2. C2 has no cartridge home. blendStep stays
   uninitialised: Advance seeds it from SetAnim, and the ROM ctor never stores
   it. Empty body; the two stores are the init list. */
// @symbol _ZN19dExtBlendAnmModel_cC1Ev
dExtBlendAnmModel_c::dExtBlendAnmModel_c() : blendWeight(0x1000), mBlendBuffer(0) {}

/* One definition emits D2, D0 and D1, and queues its _ZThn80_ pair. D0 adds
   the class operator delete. The written obligation is the conditional free
   of mBlendBuffer; a bare delete would call operator delete with no test. */
// @symbol _ZN19dExtBlendAnmModel_cD1Ev
dExtBlendAnmModel_c::~dExtBlendAnmModel_c() {
    if (mBlendBuffer)
        func_0203cbc0(mBlendBuffer);
}

// @symbol _ZN19dExtBlendAnmModel_c9DoSetFileEPcii
int dExtBlendAnmModel_c::DoSetFile(char *file, int a, int b) {
    char *f = file;
    int r = dExtSimpleModel_c::DoSetFile(file, a, b);
    if (r == 0)
        return 0;
    return func_020165c4(this, f);
}

/* DoSetFile's tail: allocates the mBlendBuffer blend buffer sized off the file and
   hands it plus the ModelComponents member to func_020462b4. It sits between
   UpdateVerts and DoSetFile in the ROM. */
// @symbol func_020165c4
extern "C" int func_020165c4(dExtBlendAnmModel_c *self, char *file) {
    self->mBlendBuffer = Memory::operator_new2(func_020462bc((int *)file));
    void *p = self->mBlendBuffer;
    if (p == 0)
        return 0;
    func_020462b4((int *)&self->data, (int)p);
    return 1;
}

// @symbol _ZN19dExtBlendAnmModel_c11UpdateVertsEv
void dExtBlendAnmModel_c::UpdateVerts() {
    s32 frame = currFrame;
    data.UpdateBones(file, (u32)(frame << 4) >> 0x10);
    if (blendWeight < 0x1000) {
        func_0204531c(&data, blendWeight);
    } else {
        data.UpdateVertsUsingBones();
    }
}

// @symbol _ZN19dExtBlendAnmModel_c9Virtual10ER9Matrix4x3
void dExtBlendAnmModel_c::Virtual10(Matrix4x3 &mat) {
    s32 frame = currFrame;
    data.UpdateBones(file, (u32)(frame << 4) >> 0x10);
    if (blendWeight < 0x1000) {
        func_0204531c(&data, blendWeight);
    } else {
        dExtSimpleModel_c::Virtual10(mat);
    }
}

// @symbol _ZN19dExtBlendAnmModel_c6RenderEPK7Vector3
void dExtBlendAnmModel_c::Render(const Vector3 *scale) {
    UpdateVerts();
    dExtSimpleModel_c::Render(scale);
}

// @symbol _ZN19dExtBlendAnmModel_c9Virtual18EjPK7Vector3
void dExtBlendAnmModel_c::Virtual18(u32 mat, const Vector3 *scale) {
    Virtual10(*(Matrix4x3 *)mat);
    dExtSimpleModel_c::Render(scale);
}

// @symbol _ZN19dExtBlendAnmModel_c7AdvanceEv
void dExtBlendAnmModel_c::Advance() {
    dExtFrameCtrl_c::Advance();
    if (blendWeight < 0x1000) {
        /* launder: keep the RMW aliasing the member so the compiler
           re-reads it the way the ROM does */
        *(int *)(&blendWeight) += blendStep;
    }
}

// @symbol _ZN19dExtBlendAnmModel_c7SetAnimER8BCA_Fileiiit
void dExtBlendAnmModel_c::SetAnim(BCA_File &animFile, int numBlendFrames, int flags, Fix12i speed,
                                  u16 startFrame) {
    if (&animFile == file) {
        SetFlags(flags);
        this->speed = speed;
    } else {
        file = &animFile;
        static_cast<dExtFrameCtrl_c &>(*this).SetAnimation(animFile.numFrames, flags, speed,
                                                           startFrame);
        if (numBlendFrames <= 0) {
            blendWeight = 0x1000;
        } else {
            blendWeight = 0;
            blendStep = cstd::fdiv(0x1000, (numBlendFrames + 1) << 12);
        }
    }
}

/* ---------------------------------------------------------------------------
   dExtAnm2Model_c, 0x02016254..0x020163e0: the method run, then D0, D1 and
   C1. A dExtAnmModel_c that carries a second dExtFrameCtrl_c (otherAnim) and
   the BCA file it plays (otherFile).
   --------------------------------------------------------------------------- */

/* One definition emits C1 and C2. C2 has no cartridge home. otherFile is
   declared before otherAnim, so the init list stores it before the member
   constructor; a body store would land one call too late. */
// @symbol _ZN15dExtAnm2Model_cC1Ev
dExtAnm2Model_c::dExtAnm2Model_c() : otherFile(0) {}

/* One definition emits D2, D0 and D1, and queues the last _ZThn80_ pair. D0
   adds the class operator delete. The body is empty: the vptr stores and the
   base destructor follow from the class. */
// @symbol _ZN15dExtAnm2Model_cD1Ev
dExtAnm2Model_c::~dExtAnm2Model_c() {}

/* The second animation's SetAnim, fast path and all: the same body as
   dExtAnmModel_c::SetAnim, run on otherFile and otherAnim. */
// @symbol _ZN15dExtAnm2Model_c13Func_020162C4Ejiit
void dExtAnm2Model_c::Func_020162C4(u32 animFile, int flags, Fix12i speed, u16 startFrame) {
    if (animFile == otherFile) {
        otherAnim.SetFlags(flags);
        otherAnim.speed = speed;
    } else {
        otherFile = animFile;
        otherAnim.SetAnimation(((BCA_File *)animFile)->numFrames, flags, speed, startFrame);
    }
}

/* Pose bone `bone` from the second animation at its current frame. Player
   calls it on a body model with bone 8; the int `self` keeps Player's
   declaration and this definition in agreement. */
// @symbol func_020162a4
extern "C" void func_020162a4(int self, int bone) {
    dExtAnm2Model_c *model = (dExtAnm2Model_c *)self;
    s32 frame = model->otherAnim.currFrame;
    func_02045e2c((int)&model->data, bone, model->otherFile, (u32)(frame << 4) >> 16);
}

/* Release bone `bone` back to the primary animation (no file, frame 0). */
// @symbol func_0201628c
extern "C" void func_0201628c(int self, int bone) {
    dExtAnm2Model_c *model = (dExtAnm2Model_c *)self;
    func_02045e2c((int)&model->data, bone, 0, 0);
}

/* otherAnim.Copy(src) upcasts src to its dExtFrameCtrl_c BASE (+0x50): the
   second animation is seeded from the first one's state. */
// @symbol _ZN15dExtAnm2Model_c4CopyERKS_Pcj
void dExtAnm2Model_c::Copy(const dExtAnm2Model_c &src, char *newFile, u32 newOtherFile) {
    dExtAnmModel_c::Copy(src, newFile);
    otherAnim.Copy(src);
    if (newOtherFile)
        otherFile = newOtherFile;
    else
        otherFile = src.otherFile;
}

/* ---------------------------------------------------------------------------
   dExtCommonModel_c, 0x0201609c..0x02016254: the dExtModel_c sibling whose
   rendering state is a POINTER into a pooled ModelComponents block
   (func_02016e70) rather than an in-place struct.
   --------------------------------------------------------------------------- */

// @symbol _ZN17dExtCommonModel_cC1Ev
dExtCommonModel_c::dExtCommonModel_c() : data(0) {
    mat4x3 = IDENTITY_MATRIX4X3;
}

/* An empty body; the vtable store at the top and the dExtModel_c subobject
 * call at the bottom are what `struct dExtCommonModel_c : dExtModel_c` and
 * `virtual ~dExtCommonModel_c()` already mean, and the compiler emits them around
 * it. Neither member has a destructor, so the ROM's 0x24 bytes are exactly
 * that. D0 is generated from the same body plus the class operator delete. */
// @symbol _ZN17dExtCommonModel_cD1Ev
dExtCommonModel_c::~dExtCommonModel_c() {}

// @symbol _ZN17dExtCommonModel_c9DoSetFileEPcii
int dExtCommonModel_c::DoSetFile(char *file, int a, int b) {
    data = func_02016e70((BMD_File *)file);
    if (data == 0)
        return 0;
    data->UpdateVertsUsingBones();
    if (a != 0)
        Func_020160AC(0x8000);
    if (b < 0)
        return 1;
    SetPolygonID((u32)b & 0xff);
    return 1;
}

// @symbol _ZN17dExtCommonModel_c6RenderEPK7Vector3
void dExtCommonModel_c::Render(const Vector3 *scale) {
    Matrix4x3 temp;
    MulMat4x3Mat4x3(&mat4x3, &data_0209b3ec, &temp);
    data->Render(&temp, (Vector3 *)scale);
}

/* ORs flags into every material; the (u32 *) launder on &p->flags keeps the
   ROM's materialized address for the read-modify-write. */
// @symbol _ZN17dExtCommonModel_c13Func_020160ACEj
void dExtCommonModel_c::Func_020160AC(u32 flags) {
    BMD_File *file = data->modelFile;
    u32 n = file->numMaterials;
    BMD_Material *p = data->materials;
    u32 i;

    for (i = 0; i < n; i++) {
        u32 *f = (u32 *)(&p->flags);
        *f |= flags;
        p++;
    }
}

// @symbol _ZN17dExtCommonModel_c12SetPolygonIDEj
void dExtCommonModel_c::SetPolygonID(u32 id) {
    func_02046008(data, id);
}

/* ---------------------------------------------------------------------------
   dExtShadowModel_c, 0x02015d38..0x0201609c: the dExtModel_c sibling that
   draws an actor's ground shadow from a shared cylinder or cuboid BMD_File.
   Every live instance sits on a global intrusive doubly-linked list (head
   data_0209cef4, freeze flag data_0209ceec): InitModel links in, the
   destructor unlinks, RenderAll walks the list and CleanAll empties it.
   --------------------------------------------------------------------------- */

/* An empty body; the three null stores after the vptr are exactly the member
 * init list -- mat at +0xc and the live-list links prev/next at +0x20/+0x24,
 * the same links ~dExtShadowModel_c unlinks. */
// @symbol _ZN17dExtShadowModel_cC1Ev
dExtShadowModel_c::dExtShadowModel_c() : mat(0), prev(0), next(0) {}

/* Only the unlink is written by hand: the vtable store at the top and the
 * dExtModel_c subobject call at the bottom are the compiler's. The list is
 * singly-headed, so removing the first node is the case that needs the head
 * compared against `this`; every other node is reached through its
 * predecessor. D0 is generated from the same body plus the class
 * operator delete. */
// @symbol _ZN17dExtShadowModel_cD1Ev
dExtShadowModel_c::~dExtShadowModel_c() {
    if (prev)
        prev->next = next;
    else if (data_0209cef4 == this)
        data_0209cef4 = next;

    if (next)
        next->prev = prev;

    prev = 0;
    next = 0;
}

/* Overrides the base slot, so it takes (char *, int, int) like its
   siblings; the polygon-ID argument is simply never used -- a shadow has
   no polygon ID to set. */
// @symbol _ZN17dExtShadowModel_c9DoSetFileEPcii
int dExtShadowModel_c::DoSetFile(char *file, int a, int b) {
    data = func_02016e70((BMD_File *)file);
    if (data == 0)
        return 0;
    data->UpdateVertsUsingBones();
    if (a != 0) {
        BMD_File *mf = data->modelFile;
        u32 n = mf->numMaterials;
        BMD_Material *mat = data->materials;
        u32 i;
        for (i = 0; i < n; i++) {
            /* (u32 *) launder: keep the ROM's materialized
               address for the read-modify-write. */
            *(u32 *)(&mat->flags) |= 0x8000;
            mat++;
        }
    }
    return 1;
}

// @symbol _ZN17dExtShadowModel_c10InitCuboidEv
int dExtShadowModel_c::InitCuboid() {
    return SetFile(&data_ov000_020ad524, 1, -1);
}

// @symbol _ZN17dExtShadowModel_c12InitCylinderEv
int dExtShadowModel_c::InitCylinder() {
    return SetFile(&data_ov000_020ad560, 1, -1);
}

/* MEASURED 2026-10-26: a real method reproduces 0x02015e64/0x58 exactly. The
 * old note here said the definition had to stay in the C front end because
 * wall 6az homes class-typed by-value parameters -- but the by-value class was
 * never in the ROM. The stripped cartridge carries no _Z strings at all, so
 * the 5Fix12IiE in the old symbol was OUR spelling of a 20.12 scalar, and
 * nothing about the bytes asked for it. Scalars, a real method, same bytes.
 */
// @symbol _ZN17dExtShadowModel_c9InitModelEP9Matrix4x3iiih
void dExtShadowModel_c::InitModel(Matrix4x3 *m, Fix12i sx, Fix12i sy, Fix12i sz, u8 opacity) {
    mat = m;
    scale.x = sx;
    scale.y = sy;
    scale.z = sz;
    this->opacity = opacity;
    if (data_0209ceec)
        return;
    /* The ROM loads the list head TWICE -- once for the store into next, once
       for the back-link test -- instead of keeping it in a register, so the
       first read is volatile. */
    next = *(dExtShadowModel_c *volatile *)&data_0209cef4;
    {
        dExtShadowModel_c *head = data_0209cef4;
        if (head)
            head->prev = this;
    }
    data_0209cef4 = this;
}

// @symbol _ZN17dExtShadowModel_c8CleanAllEv
void dExtShadowModel_c::CleanAll() {
    if (data_0209cef4) {
        do {
            dExtShadowModel_c *nx = data_0209cef4->next;
            if (nx)
                nx->prev = 0;
            /* the ROM re-reads the head for this store instead of keeping
               it in a register */
            data_0209cef4->next = 0;
            data_0209cef4 = nx;
        } while (data_0209cef4);
    }
    data_0209ceec = 0;
}

/* The scratch matrix is flat in this TU (common.h): m[10] is the
   translation's y, lifted two units so the shadow sits above the ground. */
// @symbol _ZN17dExtShadowModel_c9RenderAllEv
void dExtShadowModel_c::RenderAll() {
    dExtShadowModel_c *node = data_0209cef4;
    if (node) {
        do {
            ModelComponents *model = node->data;
            func_0203c178(&data_020a0e68, node->scale.x, node->scale.y, node->scale.z);
            MulMat4x3Mat4x3(&data_020a0e68, node->mat, &data_020a0e68);
            data_020a0e68.m[10] += 0x2000;
            MulMat4x3Mat4x3(&data_020a0e68, &data_0209b3ec, &data_020a0e68);
            func_02046120(model, node->opacity);
            model->Render(&data_020a0e68, 0);
            func_02046088(model, node->opacity, 1);
            model->Render(&data_020a0e68, 0);
            node = node->next;
        } while (node);
    }
    data_0209ceec = 1;
}

/* ---------------------------------------------------------------------------
   dExtFrameCtrl_c, 0x02015a7c..0x02015d38: the animation-playback root, a
   frame cursor the model and animation classes drive. LoadFile (0x0201794c)
   and UpdateFileOffsets (0x020469ac) are statics of this class too, but they
   link in other translation units.
   --------------------------------------------------------------------------- */

/* The vptr store is synthesized, then the body zeroes currFrame (+0x8) and
 * sets speed (+0xc) to 0x1000 = 1.0, in that order. C1 and C2 are the same
 * code (root class, nothing to chain); every ROM caller of C2 is a derived
 * base step. */
// @symbol _ZN15dExtFrameCtrl_cC1Ev
// @symbol _ZN15dExtFrameCtrl_cC2Ev
dExtFrameCtrl_c::dExtFrameCtrl_c() {
    currFrame = 0;
    speed = 0x1000;
}

/* The destructor, first among the class's virtuals and so its key function.
 * The body is empty; D0 adds the call to the class operator delete the
 * header defines. dExtFrameCtrl_c has no virtual bases, so D1 and D2 are
 * the same code: a vtable slot reaches D1, a derived destructor's base step
 * reaches D2. */
// @symbol _ZN15dExtFrameCtrl_cD2Ev
// @symbol _ZN15dExtFrameCtrl_cD0Ev
// @symbol _ZN15dExtFrameCtrl_cD1Ev
dExtFrameCtrl_c::~dExtFrameCtrl_c() {}

// @symbol _ZN15dExtFrameCtrl_c7AdvanceEv
void dExtFrameCtrl_c::Advance() {
    u32 f = numFramesAndFlags;
    u32 len = f & ~0xc0000000;
    if ((f & 0xc0000000) == 0) {
        currFrame = (currFrame + speed + (int)len) % (int)len;
    } else {
        /* the ROM materializes the field address for this
           read-modify-write, and reloads currFrame through the member
           afterwards; the launder keeps this store aliasing the member so
           the compiler re-reads it the way the ROM does */
        int *pFrame = (int *)((long long)((int)this + 8));
        *pFrame = *pFrame + speed;
        if (currFrame < 0) {
            currFrame = 0;
        } else if (currFrame >= (int)len) {
            currFrame = (int)len - 1;
        }
    }
}

// @symbol _ZN15dExtFrameCtrl_c12SetAnimationEtiit
void dExtFrameCtrl_c::SetAnimation(u16 numFrames, int flags, Fix12i speed, u16 startFrame) {
    *(int *)((char *)&this->numFramesAndFlags) = flags | (numFrames << 12);
    *(int *)((char *)&this->currFrame) = startFrame << 12;
    *(int *)((char *)&this->speed) = speed;
}

// @symbol _ZNK15dExtFrameCtrl_c13GetFrameCountEv
u32 dExtFrameCtrl_c::GetFrameCount() const {
    u32 v = numFramesAndFlags;
    return ((v & 0x3fffffff) << 4) >> 16;
}

// @symbol _ZN15dExtFrameCtrl_c8SetFlagsEi
void dExtFrameCtrl_c::SetFlags(int flags) {
    numFramesAndFlags = (numFramesAndFlags & 0x3fffffff) | flags;
}

// @symbol _ZN15dExtFrameCtrl_c8GetFlagsEv
int dExtFrameCtrl_c::GetFlags() {
    return numFramesAndFlags & 0xC0000000;
}

// @symbol _ZN15dExtFrameCtrl_c8FinishedEv
int dExtFrameCtrl_c::Finished() {
    u32 f = numFramesAndFlags;
    int cur = currFrame;
    return cur >= (int)((f & 0x3fffffff) - 1);
}

// @symbol _ZNK15dExtFrameCtrl_c12WillHitFrameEi
bool dExtFrameCtrl_c::WillHitFrame(int frame) const {
    s32 f = frame << 12;
    s32 next = currFrame + speed;
    s32 num = numFramesAndFlags & ~0xc0000000;

    if ((numFramesAndFlags & 0xc0000000) == 0) {
        if (next < 0) {
            next = (next + num) % num;
            if ((f >= 0 && f < currFrame) || (next <= f && f < num))
                return true;
        } else if (next >= num) {
            next %= num;
            if ((currFrame <= f && f < num) || f < next)
                return true;
        } else if (currFrame <= next) {
            if (currFrame <= f && f < next)
                return true;
        } else {
            if (next <= f && f < currFrame)
                return true;
        }
    } else {
        if (next < 0)
            next = 0;
        if (next >= num)
            next = num - 1;

        if (currFrame <= next) {
            if (f >= currFrame && f < next)
                return true;
        } else if (f >= next && f < currFrame)
            return true;
    }

    return false;
}

/* Copies the three members and leaves the vptr alone. */
// @symbol _ZN15dExtFrameCtrl_c4CopyERKS_
void dExtFrameCtrl_c::Copy(const dExtFrameCtrl_c &anim) {
    numFramesAndFlags = anim.numFramesAndFlags;
    currFrame = anim.currFrame;
    speed = anim.speed;
}

/* ---------------------------------------------------------------------------
   dExtAnmTexPat_c, 0x0201597c..0x02015a7c: dExtFrameCtrl_c child driving
   BTP-file playback.
   --------------------------------------------------------------------------- */

// @symbol _ZN15dExtAnmTexPat_cC1Ev
dExtAnmTexPat_c::dExtAnmTexPat_c() {
    file = 0;
}

// @symbol _ZN15dExtAnmTexPat_cD1Ev
dExtAnmTexPat_c::~dExtAnmTexPat_c() {}

/* The header's first word is the frame count handed to SetAnimation, as in
   the sibling BTA. */
// @symbol _ZN15dExtAnmTexPat_c7SetFileER8BTP_Fileiit
void dExtAnmTexPat_c::SetFile(BTP_File &animFile, int flags, Fix12i speed, u16 startFrame) {
    if (&animFile == file) {
        SetFlags(flags);
        this->speed = speed;
    } else {
        file = &animFile;
        SetAnimation(animFile.numFrames, flags, speed, startFrame);
    }
}

// @symbol _ZN15dExtAnmTexPat_c6UpdateER15ModelComponents
void dExtAnmTexPat_c::Update(ModelComponents &model) {
    func_02046bbc(&model, file, (u16)((u32)currFrame >> 12));
}

/* Static: no this. */
// @symbol _ZN15dExtAnmTexPat_c7PrepareER8BMD_FileR8BTP_File
void dExtAnmTexPat_c::Prepare(BMD_File &model, BTP_File &animFile) {
    func_02046d50(&model, &animFile);
}

/* ---------------------------------------------------------------------------
   dExtAnmTexSRT_c, 0x0201587c..0x0201597c: dExtFrameCtrl_c child driving
   BTA-file playback.
   --------------------------------------------------------------------------- */

// @symbol _ZN15dExtAnmTexSRT_cC1Ev
dExtAnmTexSRT_c::dExtAnmTexSRT_c() {
    file = 0;
}

// @symbol _ZN15dExtAnmTexSRT_cD1Ev
dExtAnmTexSRT_c::~dExtAnmTexSRT_c() {}

// @symbol _ZN15dExtAnmTexSRT_c7SetFileER8BTA_Fileiit
void dExtAnmTexSRT_c::SetFile(BTA_File &animFile, int flags, Fix12i speed, u16 startFrame) {
    if (&animFile == file) {
        SetFlags(flags);
        this->speed = speed;
    } else {
        file = &animFile;
        SetAnimation(animFile.numFrames, flags, speed, startFrame);
    }
}

// @symbol _ZN15dExtAnmTexSRT_c6UpdateER15ModelComponents
void dExtAnmTexSRT_c::Update(ModelComponents &model) {
    func_020469e8(&model, file, (u16)((u32)currFrame >> 12));
}

/* Static: no this. */
// @symbol _ZN15dExtAnmTexSRT_c7PrepareER8BMD_FileR8BTA_File
void dExtAnmTexSRT_c::Prepare(BMD_File &model, BTA_File &animFile) {
    func_02046b64(&model, &animFile);
}

/* ---------------------------------------------------------------------------
   dExtAnmMaterial_c, 0x0201577c..0x0201587c: dExtFrameCtrl_c child driving
   BMA-file playback. The lowest class in the object, so the last in source.
   --------------------------------------------------------------------------- */

// @symbol _ZN17dExtAnmMaterial_cC1Ev
dExtAnmMaterial_c::dExtAnmMaterial_c() {
    file = 0;
}

// @symbol _ZN17dExtAnmMaterial_cD1Ev
dExtAnmMaterial_c::~dExtAnmMaterial_c() {}

// @symbol _ZN17dExtAnmMaterial_c7SetFileER8BMA_Fileiit
void dExtAnmMaterial_c::SetFile(BMA_File &animFile, int flags, Fix12i speed, u16 startFrame) {
    if (&animFile == file) {
        SetFlags(flags);
        this->speed = speed;
    } else {
        file = &animFile;
        SetAnimation(animFile.numFrames, flags, speed, startFrame);
    }
}

// @symbol _ZN17dExtAnmMaterial_c6UpdateER15ModelComponents
void dExtAnmMaterial_c::Update(ModelComponents &model) {
    func_02046e28(&model, file, (u16)((u32)currFrame >> 12));
}

/* Static: no this. */
// @symbol _ZN17dExtAnmMaterial_c7PrepareER8BMD_FileR8BMA_File
void dExtAnmMaterial_c::Prepare(BMD_File &model, BMA_File &animFile) {
    func_020470e8(&model, &animFile);
}
