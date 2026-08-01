typedef unsigned int u32;
typedef unsigned char u8;

struct ModelComponents;
struct Matrix4x3;
struct Vector3 { int x, y, z; };

struct ModelBase {
    u32 *vtable;
    u32 unk04;
};

struct ShadowModel {
    u32 *vtable;
    u32 unk04;
    struct ModelComponents *modelDataPtr;
    struct Matrix4x3 *matPtr;
    struct Vector3 scale;
    u8 opacity;
    u8 unk1d;
    u8 unk1e;
    u8 unk1f;
    struct ShadowModel *prev;
    struct ShadowModel *next;
};

extern u32 _ZTV11ShadowModel[];
extern void _ZN9ModelBaseC1Ev(struct ModelBase *thiz);

struct ShadowModel *_ZN11ShadowModelC1Ev(struct ShadowModel *thiz)
{
    _ZN9ModelBaseC1Ev((struct ModelBase *)thiz);
    thiz->vtable = _ZTV11ShadowModel;
    thiz->matPtr = 0;
    thiz->prev = 0;
    thiz->next = 0;
    return thiz;
}
