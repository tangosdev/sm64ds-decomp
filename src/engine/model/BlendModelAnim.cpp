//cpp
/* BlendModelAnim -- ModelAnim child that cross-fades from the old pose into
   the new animation. Cartridge class: _ZTV14BlendModelAnim at 0x0208e94c
   plus its Animation-side thunk vtable at 0x0208e970; the cartridge RTTI
   names it dExtBlendAnmModel_c. TU claims 0x020163e0..0x02016644 -- the
   class's method run including func_020165c4 (0x020165c4..0x02016604),
   the helper DoSetFile tail-calls; DoSetFile is its only caller in the
   tree. The lifecycle pair (D0, the D1 shard, C1) and the two
   _ZThn80_ destructor thunks stay enrolled shards: defining the key
   function ~BlendModelAnim() would emit the whole MI vtable group into
   this object, and the secondary table's preamble lands inside the
   primary's compared extent, where RTTI-off zeros cannot license the
   discard. Written back-to-front: mwccarm emits .text in reverse source
   order under default deferred codegen. */
#include "BlendModelAnim.h"
#include "Memory.h"

extern "C" {
/* local extern: no header declares it; blends verts toward the old pose in
   Virtual10/UpdateVerts while blendWeight is still below 1.0 */
void func_0204531c(ModelComponents *data, s32 weight);
/* local extern: no header declares it; sizes the unk_6c blend buffer off the
   file. Spelled (int *) the way func_020462bc.c defines it */
u32 func_020462bc(int *file);
/* local extern: no header declares it; initializes ModelComponents' side of
   the blend buffer. Spelled (int *, int) the way func_020462b4.c defines it */
void func_020462b4(int *data, int buf);
/* defined below -- DoSetFile's tail; it sits between UpdateVerts and
   DoSetFile in the ROM and DoSetFile is its only caller here */
int func_020165c4(BlendModelAnim *self, char *file);
/* SetAnim below and its SetAnimation call keep their mangled spellings:
   wall 6az (notes/mwccarm-codegen.md) homes class-typed by-value
   parameters that a body reads, and the real signature carries
   Fix12<int> -- passing one to the member declaration homes it to the
   caller's stack. The declarations in BlendModelAnim.h and dExtFrameCtrl_c.h
   are the real ones. */
void _ZN15dExtFrameCtrl_c12SetAnimationEti5Fix12IiEt(dExtFrameCtrl_c *self, u16 numFrames, s32 flags, s32 speed, u16 startFrame);
}

namespace cstd { int fdiv(int a, int b); }

/* Only the header word SetAnim reads; dExtFrameCtrl_c::UpdateFileOffsets' shard
   spells the pointer fields instead. */
struct BCA_File {
    u16 unk_00;
    u16 numFrames;
};

// @symbol _ZN14BlendModelAnim9DoSetFileEPcii
int BlendModelAnim::DoSetFile(char *file, int a, int b)
{
    char *f = file;
    int r = Model::DoSetFile(file, a, b);
    if (r == 0)
        return 0;
    return func_020165c4(this, f);
}

/* DoSetFile's tail: allocates the unk_6c blend buffer sized off the file and
   hands it plus the ModelComponents member to func_020462b4. It sits between
   UpdateVerts and DoSetFile in the ROM. */
// @symbol func_020165c4
extern "C" int func_020165c4(BlendModelAnim *self, char *file)
{
    self->unk_6c = Memory::operator_new2(func_020462bc((int *)file));
    void *p = self->unk_6c;
    if (p == 0) return 0;
    func_020462b4((int *)&self->data, (int)p);
    return 1;
}

// @symbol _ZN14BlendModelAnim11UpdateVertsEv
void BlendModelAnim::UpdateVerts()
{
    s32 frame = currFrame;
    data.UpdateBones(file, (u32)(frame << 4) >> 0x10);
    if (blendWeight < 0x1000) {
        func_0204531c(&data, blendWeight);
    } else {
        data.UpdateVertsUsingBones();
    }
}

// @symbol _ZN14BlendModelAnim9Virtual10ER9Matrix4x3
void BlendModelAnim::Virtual10(Matrix4x3 &mat)
{
    s32 frame = currFrame;
    data.UpdateBones(file, (u32)(frame << 4) >> 0x10);
    if (blendWeight < 0x1000) {
        func_0204531c(&data, blendWeight);
    } else {
        Model::Virtual10(mat);
    }
}

// @symbol _ZN14BlendModelAnim6RenderEPK7Vector3
void BlendModelAnim::Render(const Vector3 *scale)
{
    UpdateVerts();
    Model::Render(scale);
}

// @symbol _ZN14BlendModelAnim9Virtual18EjPK7Vector3
void BlendModelAnim::Virtual18(u32 mat, const Vector3 *scale)
{
    Virtual10(*(Matrix4x3 *)mat);
    Model::Render(scale);
}

// @symbol _ZN14BlendModelAnim7AdvanceEv
void BlendModelAnim::Advance()
{
    dExtFrameCtrl_c::Advance();
    if (blendWeight < 0x1000) {
        /* launder: keep the RMW aliasing the member so the compiler
           re-reads it the way the ROM does */
        *(int *)(&blendWeight) += blendStep;
    }
}

// @symbol _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt
extern "C" void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
    BlendModelAnim *thiz, BCA_File &file, s32 numBlendFrames, s32 flags,
    s32 speed, u16 startFrame)
{
    if (&file == thiz->file)
    {
        thiz->SetFlags(flags);
        thiz->speed = speed;
    }
    else
    {
        thiz->file = &file;
        _ZN15dExtFrameCtrl_c12SetAnimationEti5Fix12IiEt(
            &static_cast<dExtFrameCtrl_c &>(*thiz), file.numFrames, flags, speed,
            startFrame);
        if (numBlendFrames <= 0)
        {
            thiz->blendWeight = 0x1000;
        }
        else
        {
            thiz->blendWeight = 0;
            thiz->blendStep = cstd::fdiv(0x1000, (numBlendFrames + 1) << 12);
        }
    }
}
