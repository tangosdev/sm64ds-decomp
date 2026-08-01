typedef unsigned int u32;
typedef int Fix12i;

struct ModelComponents;

struct Matrix4x3 {
    Fix12i r[12];
};

struct ModelBase {
    u32 *vtable;
    u32 unk04;
};

struct CommonModel {
    u32 *vtable;
    u32 unk04;
    struct ModelComponents *data;
    struct Matrix4x3 mat4x3;
};

extern u32 _ZTV11CommonModel[];
extern struct Matrix4x3 data_02082128;
extern void _ZN9ModelBaseC1Ev(struct ModelBase *thiz);

struct CommonModel *_ZN11CommonModelC1Ev(struct CommonModel *thiz)
{
    _ZN9ModelBaseC1Ev((struct ModelBase *)thiz);
    thiz->vtable = _ZTV11CommonModel;
    thiz->data = 0;
    thiz->mat4x3 = data_02082128;
    return thiz;
}
