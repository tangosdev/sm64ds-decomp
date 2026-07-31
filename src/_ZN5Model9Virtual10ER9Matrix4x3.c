#include "types.h"
struct Matrix4x3 {
    s32 m[12];
};

struct BMD_File {
    u32 scaleShift;
    u32 numBones;
    void* bones;
};

struct ModelComponents {
    struct BMD_File* modelFile;
    void* materials;
    void* bones;
    struct Matrix4x3* transforms;
    void* unk10;
};

struct Model {
    void* vtable;
    void* unk04;
    struct ModelComponents data;
};

extern void func_02045074(struct ModelComponents* data, void* bones);

void _ZN5Model9Virtual10ER9Matrix4x3(struct Model* self, struct Matrix4x3* arg0)
{
    struct Matrix4x3* dst = self->data.transforms;
    *dst = *arg0;
    func_02045074(&self->data, self->data.modelFile->bones);
}
