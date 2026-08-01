#include "types.h"
/* ModelAnim::Virtual10 at 0x0201682c, size=0x40
 * Updates bones from animation then calls Model::Virtual10 with a transform.
 */
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
extern void _ZN5Model9Virtual10ER9Matrix4x3(struct ModelAnim* thiz, struct Matrix4x3* mat);

void _ZN9ModelAnim9Virtual10ER9Matrix4x3(struct ModelAnim* thiz, struct Matrix4x3* mat)
{
    s32 currFrame = thiz->currFrame;
    _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(&thiz->data, thiz->file, (u32)(currFrame << 4) >> 0x10);
    _ZN5Model9Virtual10ER9Matrix4x3(thiz, mat);
}
