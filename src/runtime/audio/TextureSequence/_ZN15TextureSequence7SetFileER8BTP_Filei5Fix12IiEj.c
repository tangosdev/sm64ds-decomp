#include "types.h"
/* TextureSequence::SetFile at 0x020159ac, size=0x54
 * Sets animation file; fast path (same file) just updates flags+speed.
 */
struct BTP_File {
    u16 numFrames;
};

struct TextureSequence {
    void* vtable;
    s32 numFramesAndFlags;
    s32 currFrame;
    s32 speed;
    struct BTP_File* file;
};

extern void _ZN9Animation8SetFlagsEi(struct TextureSequence* anim, s32 flags);
extern void _ZN9Animation12SetAnimationEti5Fix12IiEt(struct TextureSequence* anim, u16 frames, s32 flags, s32 speed, u16 startFrame);

void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(struct TextureSequence* thiz, struct BTP_File* file, s32 flags, s32 speed, u16 startFrame)
{
    if (file == thiz->file)
    {
        _ZN9Animation8SetFlagsEi(thiz, flags);
        thiz->speed = speed;
    }
    else
    {
        thiz->file = file;
        _ZN9Animation12SetAnimationEti5Fix12IiEt(thiz, file->numFrames, flags, speed, startFrame);
    }
}
