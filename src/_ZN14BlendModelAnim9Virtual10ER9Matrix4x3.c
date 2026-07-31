#include "types.h"
/* BlendModelAnim::Virtual10 at 0x02016518, size=0x60 */
struct ModelComponents {
    void* modelFile;
    void* materials;
    void* bones;
    void* transforms;
    void* unk10;
};

struct Matrix4x3 {
    s32 m[12];
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
extern void Func_0204531c(struct ModelComponents* data, s32 unk64);
extern void _ZN5Model9Virtual10ER9Matrix4x3(struct BlendModelAnim* thiz, struct Matrix4x3* mat);

void _ZN14BlendModelAnim9Virtual10ER9Matrix4x3(struct BlendModelAnim* thiz, struct Matrix4x3* mat)
{
    s32 currFrame = thiz->currFrame;
    _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(&thiz->data, thiz->file, (u32)(currFrame << 4) >> 0x10);
    if (thiz->unk64 < 0x1000)
    {
        Func_0204531c(&thiz->data, thiz->unk64);
    }
    else
    {
        _ZN5Model9Virtual10ER9Matrix4x3(thiz, mat);
    }
}
