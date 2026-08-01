#include "types.h"
/* ModelAnim::UpdateVerts at 0x0201686c, size=0x30
 * Updates bone transforms then vertex positions from animation.
 */
struct ModelComponents {
    void* modelFile;
    void* materials;
    void* bones;
    void* transforms;
    void* unk10;
};

struct ModelAnim {
    void* vtable;
    void* unk04;
    struct ModelComponents data;
    char mat4x3[0x30];
    void* unkMatPtr;
    void* animVtable;
    s32 numFramesAndFlags;
    s32 currFrame;
    s32 speed;
    void* file;
};

extern void _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(struct ModelComponents* data, void* file, s32 frame);
extern void _ZN15ModelComponents21UpdateVertsUsingBonesEv(struct ModelComponents* data);

void _ZN9ModelAnim11UpdateVertsEv(struct ModelAnim* thiz)
{
    s32 currFrame = thiz->currFrame;
    _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(&thiz->data, thiz->file, (u32)(currFrame << 4) >> 0x10);
    _ZN15ModelComponents21UpdateVertsUsingBonesEv(&thiz->data);
}
