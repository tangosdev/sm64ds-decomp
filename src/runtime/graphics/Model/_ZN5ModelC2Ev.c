typedef unsigned int u32;
typedef int Fix12i;

struct BMD_File;

struct ModelComponents {
    struct BMD_File *modelFile;
    void *materials;
    void *bones;
    void *transforms;
    char *unk10;
};

struct Matrix4x3 {
    Fix12i r[12];
};

struct ModelBase {
    u32 *vtable;
    u32 unk04;
};

struct Model {
    u32 *vtable;
    u32 unk04;
    struct ModelComponents data;
    struct Matrix4x3 mat4x3;
    struct Matrix4x3 *unkMatPtr;
};

extern u32 _ZTV5Model[];
extern struct Matrix4x3 IDENTITY_MATRIX4X3;
extern void _ZN9ModelBaseC2Ev(struct ModelBase *thiz);

struct Model *_ZN5ModelC2Ev(struct Model *thiz)
{
    _ZN9ModelBaseC2Ev((struct ModelBase *)thiz);
    thiz->vtable = _ZTV5Model;
    thiz->unkMatPtr = 0;
    thiz->mat4x3 = IDENTITY_MATRIX4X3;
    return thiz;
}
