//cpp
/* ModelAnim -- Model + Animation multiple inheritance: the animated-model
   base the scene objects embed. Cartridge class: _ZTV9ModelAnim at
   0x0208e980 plus its Animation-side thunk vtable; the cartridge RTTI
   names it dExtAnmModel_c (_ZTI14dExtAnmModel_c at 0x0208e924).
   TU claims 0x02016714..0x0201689c -- the class's method run: Copy,
   SetAnim, the file-local func_020167a4 helper, Virtual18, Render,
   Virtual10, UpdateVerts. The lifecycle row (D2, D0, D1, C1, C2 at
   0x0201689c..0x020169d8) and the two _ZThn80_ destructor thunks stay
   enrolled shards: defining the key function ~ModelAnim() would emit the
   whole MI vtable group into this object, and the secondary table's
   preamble lands inside the primary's compared extent, where RTTI-off
   zeros cannot license the discard -- the same wall BlendModelAnim
   recorded. Written back-to-front: mwccarm emits .text in reverse source
   order under default deferred codegen. */
#include "ModelAnim.h"

extern "C" {
/* SetAnim keeps its mangled spelling: wall 6az (notes/mwccarm-codegen.md)
   homes class-typed by-value parameters that a body reads, and the real
   signature carries Fix12<int> -- the same wall its BlendModelAnim
   sibling records. The declaration in ModelAnim.h is the real one.
   local extern: Fix12 by-value speed. */
void _ZN15dExtFrameCtrl_c12SetAnimationEti5Fix12IiEt(dExtFrameCtrl_c *self, u16 numFrames,
                                                     s32 flags, s32 speed, u16 startFrame);
}

/* Only the header word SetAnim reads; Animation::UpdateFileOffsets' shard
   spells the pointer fields instead. */
struct BCA_File {
    u16 unk_00;
    u16 numFrames;
};

// @symbol _ZN9ModelAnim11UpdateVertsEv
void ModelAnim::UpdateVerts() {
    s32 frame = currFrame;
    data.UpdateBones(file, (u32)(frame << 4) >> 0x10);
    data.UpdateVertsUsingBones();
}

// @symbol _ZN9ModelAnim9Virtual10ER9Matrix4x3
void ModelAnim::Virtual10(Matrix4x3 &mat) {
    s32 frame = currFrame;
    data.UpdateBones(file, (u32)(frame << 4) >> 0x10);
    Model::Virtual10(mat);
}

// @symbol _ZN9ModelAnim6RenderEPK7Vector3
void ModelAnim::Render(const Vector3 *scale) {
    UpdateVerts();
    Model::Render(scale);
}

// @symbol _ZN9ModelAnim9Virtual18EjPK7Vector3
void ModelAnim::Virtual18(u32 mat, const Vector3 *scale) {
    Virtual10(*(Matrix4x3 *)mat);
    Model::Render(scale);
}

/* Refresh bones without touching verts -- UpdateVerts' first half kept
   file-local; daMoray_c calls it on a BlendModelAnim*. */
// @symbol func_020167a4
extern "C" void func_020167a4(ModelAnim *self) {
    s32 frame = self->currFrame;
    self->data.UpdateBones(self->file, (u32)(frame << 4) >> 0x10);
}

// @symbol _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *thiz, BCA_File *animFile,
                                                            s32 flags, s32 speed, u16 startFrame) {
    if (animFile == thiz->file) {
        thiz->SetFlags(flags);
        thiz->speed = speed;
    } else {
        thiz->file = animFile;
        _ZN15dExtFrameCtrl_c12SetAnimationEti5Fix12IiEt(
            &static_cast<dExtFrameCtrl_c &>(*thiz), animFile->numFrames, flags, speed, startFrame);
    }
}

/* Both this and src adjust by +0x50 for the base call: the first real
   multiple-inheritance argument conversion in the tree. */
// @symbol _ZN9ModelAnim4CopyERKS_Pc
void ModelAnim::Copy(const ModelAnim &src, char *newFile) {
    dExtFrameCtrl_c::Copy(src);
    if (newFile != 0)
        file = (BCA_File *)newFile;
    else
        file = src.file;
}
