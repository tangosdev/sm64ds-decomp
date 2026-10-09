#include "types.h"
/* ModelAnim2::Func_020162C4 at 0x020162c4, size=0x5c
 * Updates otherAnim: fast path (same file) just updates flags+speed;
 * slow path stores new file, loads numFrames, calls SetAnimation.
 */
struct BCA_File {
    u16 unk00;
    u16 numFrames;
};

struct dExtFrameCtrl_c {
    void* vtable;
    s32 numFramesAndFlags;
    s32 currFrame;
    s32 speed;
};

struct ModelAnim2 {
    char pad[0x64];
    void* unk64;
    struct dExtFrameCtrl_c otherAnim;
};

extern void _ZN15dExtFrameCtrl_c8SetFlagsEi(struct dExtFrameCtrl_c* anim, s32 flags);
extern void _ZN15dExtFrameCtrl_c12SetAnimationEti5Fix12IiEt(struct dExtFrameCtrl_c* anim, u16 frames, s32 flags, s32 speed, u16 startFrame);

void _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt(struct ModelAnim2* thiz, void* newUnk64, s32 flags, s32 speed, u16 startFrame)
{
    if (newUnk64 == thiz->unk64)
    {
        _ZN15dExtFrameCtrl_c8SetFlagsEi(&thiz->otherAnim, flags);
        thiz->otherAnim.speed = speed;
    }
    else
    {
        thiz->unk64 = newUnk64;
        _ZN15dExtFrameCtrl_c12SetAnimationEti5Fix12IiEt(&thiz->otherAnim, ((struct BCA_File*)newUnk64)->numFrames, flags, speed, startFrame);
    }
}
