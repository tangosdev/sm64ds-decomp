#include "types.h"
/* _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj at 0x020158ac
 * Sets the BTA file for a TextureTransformer. If the file is the same, updates flags and speed.
 * If different, stores the file and calls SetAnimation with the file's frame count.
 */
struct BTA_File { u16 numFrames; };

struct Animation {
    void* vtable;
    s32 numFramesAndFlags;
    s32 currFrame;
    s32 speed;
};

struct TextureTransformer {
    struct Animation anim;
    struct BTA_File* file;
};

extern void _ZN9Animation8SetFlagsEi(struct TextureTransformer* self, s32 flags);
extern void _ZN9Animation12SetAnimationEti5Fix12IiEt(struct TextureTransformer* self, u16 frames, s32 flags, s32 speed, u16 startFrame);

void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(struct TextureTransformer* self, struct BTA_File* file, s32 flags, s32 speed, u16 startFrame)
{
    if (file == self->file) {
        _ZN9Animation8SetFlagsEi(self, flags);
        self->anim.speed = speed;
    } else {
        self->file = file;
        _ZN9Animation12SetAnimationEti5Fix12IiEt(self, file->numFrames, flags, speed, startFrame);
    }
}
