#include "types.h"
/* _ZN9ModelAnim4CopyERKS_Pc at 0x02016714 (52 bytes)
 * ModelAnim::Copy -- copies animation data from another ModelAnim into this one.
 * If newFile != NULL, stores newFile at this->file (0x60); otherwise copies src->file.
 * Also calls Animation::Copy (base class copy) on the sub-object at +0x50.
 */
struct Animation {
    u32 vtable;
    Fix12i numFramesAndFlags;
    Fix12i currFrame;
    Fix12i speed;
};

/* ModelAnim = Model(0x50 bytes) + Animation(0x10 bytes) + file ptr(4) */
struct ModelAnim {
    char padModel[0x50];
    struct Animation anim; /* 0x50 */
    void *file;            /* 0x60 */
};

extern void _ZN9Animation4CopyERKS_(struct Animation *thiz, const struct Animation *src); /* 0x02015a7c */

void _ZN9ModelAnim4CopyERKS_Pc(struct ModelAnim *thiz, const struct ModelAnim *src, char *newFile)
{
    _ZN9Animation4CopyERKS_(&thiz->anim, &src->anim);
    if (newFile != 0)
        thiz->file = newFile;
    else
        thiz->file = src->file;
}
