//cpp
/* ModelAnim2 -- a ModelAnim that carries a second dExtFrameCtrl_c (otherAnim)
   and the BCA file it plays (otherFile). The cartridge RTTI names the class
   dExtAnm2Model_c (_ZTI15dExtAnm2Model_c at 0x0208e788, single inheritance
   from dExtAnmModel_c, the tree's ModelAnim); its vtable _ZTV10ModelAnim2 at
   0x0208e9b4 overrides only the destructor pair.
   TU claims 0x02016254..0x02016320 -- the class's method run: Copy, the two
   bone-animation helpers Player calls on its body models, and Func_020162C4.
   The lifecycle row (D0, D1, C1 at 0x02016320..0x020163e0) stays enrolled
   shards, the split ModelAnim and BlendModelAnim recorded: the destructor is
   the key function, and defining it here would emit the vtable group of a
   class whose base is multiply inherited.
   Scaffolded by tools/class_scaffold.py, then filled. Written back-to-front:
   mwccarm emits .text in reverse source order under default deferred codegen. */
#include "ModelAnim2.h"

extern "C" {
/* local extern: Fix12 by-value speed (wall 6az, notes/mwccarm-codegen.md);
   the declaration in dExtFrameCtrl_c.h is the real one. */
void _ZN15dExtFrameCtrl_c12SetAnimationEti5Fix12IiEt(dExtFrameCtrl_c *self, u16 numFrames,
                                                     s32 flags, s32 speed, u16 startFrame);
/* local extern: defined in src/engine/model/Model4.cpp, which types the frame
   `short`; no header declares it. Stores (file, frame) into bone slot i of the
   components' animation array. The frame stays `int` here: the ROM passes
   func_020162a4's `lsr #16` result with no sign extension, which a `short`
   parameter would add (the one-function source this TU absorbed carried
   the same banked param:#4 int disagreement). */
void func_02045e2c(int obj, int i, int a, int b);
}

/* Only the header word Func_020162C4 reads; the first halfword is not
   read anywhere in this TU. */
struct BCA_File {
    u8 pad_00[0x2];
    u16 numFrames; /* 0x02 */
};

/* The second animation's SetAnim, fast path and all; keeps the mangled free
   spelling for the same Fix12<int> reason as ModelAnim::SetAnim. */
// @symbol _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt
extern "C" void _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt(ModelAnim2 *thiz, u32 animFile,
                                                            s32 flags, s32 speed, u16 startFrame) {
    if (animFile == thiz->otherFile) {
        thiz->otherAnim.SetFlags(flags);
        thiz->otherAnim.speed = speed;
    } else {
        thiz->otherFile = animFile;
        _ZN15dExtFrameCtrl_c12SetAnimationEti5Fix12IiEt(
            &thiz->otherAnim, ((BCA_File *)animFile)->numFrames, flags, speed, startFrame);
    }
}

/* Pose bone `bone` from the second animation at its current frame. Player
   calls it on a body model with bone 8; the int `self` keeps Player's
   declaration and this definition in agreement. */
// @symbol func_020162a4
extern "C" void func_020162a4(int self, int bone) {
    ModelAnim2 *model = (ModelAnim2 *)self;
    s32 frame = model->otherAnim.currFrame;
    func_02045e2c((int)&model->data, bone, model->otherFile, (u32)(frame << 4) >> 16);
}

/* Release bone `bone` back to the primary animation (no file, frame 0). */
// @symbol func_0201628c
extern "C" void func_0201628c(int self, int bone) {
    ModelAnim2 *model = (ModelAnim2 *)self;
    func_02045e2c((int)&model->data, bone, 0, 0);
}

/* otherAnim.Copy(src) upcasts src to its dExtFrameCtrl_c BASE (+0x50): the
   second animation is seeded from the first one's state. */
// @symbol _ZN10ModelAnim24CopyERKS_Pcj
void ModelAnim2::Copy(const ModelAnim2 &src, char *newFile, u32 newOtherFile) {
    ModelAnim::Copy(src, newFile);
    otherAnim.Copy(src);
    if (newOtherFile)
        otherFile = newOtherFile;
    else
        otherFile = src.otherFile;
}
