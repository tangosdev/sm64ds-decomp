#include "types.h"
/* BlendModelAnim::UpdateVerts at 0x02016578, size=0x4c
 * Updates bones, then calls func_0204531c(data, unk64) (blend) or UpdateVertsUsingBones based on unk64.
 */
struct ModelComponents {
    void* modelFile;
    void* materials;
    void* bones;
    void* transforms;
    void* unk10;
};

struct BlendModelAnim {
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
    s32 unk64;
    s32 unk68;
    s32 unk6C;
};

extern void _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(struct ModelComponents* data, void* file, s32 frame);
extern void func_0204531c(struct ModelComponents* data, s32 unk64);
extern void _ZN15ModelComponents21UpdateVertsUsingBonesEv(struct ModelComponents* data);

void _ZN14BlendModelAnim11UpdateVertsEv(struct BlendModelAnim* thiz)
{
    s32 currFrame = thiz->currFrame;
    _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(&thiz->data, thiz->file, (u32)(currFrame << 4) >> 0x10);
    if (thiz->unk64 < 0x1000)
    {
        func_0204531c(&thiz->data, thiz->unk64);
    }
    else
    {
        _ZN15ModelComponents21UpdateVertsUsingBonesEv(&thiz->data);
    }
}
