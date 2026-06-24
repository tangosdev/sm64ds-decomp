//cpp
struct Heap;

extern "C" void _ZN12WithMeshClsnD1Ev(void *thiz);
extern "C" void func_0207328c(void *p, int a, int b, void *fn);
extern "C" void _ZN11ShadowModelD1Ev(void *thiz);
extern "C" void _ZN9ModelAnimD1Ev(void *thiz);
extern "C" void _ZN18MovingMeshColliderD1Ev(void *thiz);
extern "C" void _ZN5ModelD1Ev(void *thiz);
extern "C" void _ZN5ActorD1Ev(void *thiz);
extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *p, struct Heap *h);
extern "C" void _ZN25MovingCylinderClsnWithPosD1Ev(void *thiz);
extern void *data_ov063_0211ed34[];
extern void *data_ov002_0210ae38[];
extern struct Heap *data_020a0eac;

extern "C" void *func_ov063_0211d54c(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    *(void **)c = data_ov063_0211ed34;
    _ZN12WithMeshClsnD1Ev(c + 0x50c);
    func_0207328c(c + 0x48c, 2, 0x40, (void *)_ZN25MovingCylinderClsnWithPosD1Ev);
    _ZN11ShadowModelD1Ev(c + 0x3d4);
    _ZN11ShadowModelD1Ev(c + 0x3ac);
    _ZN11ShadowModelD1Ev(c + 0x384);
    _ZN9ModelAnimD1Ev(c + 0x320);
    *(void **)c = data_ov002_0210ae38;
    _ZN18MovingMeshColliderD1Ev(c + 0x124);
    _ZN5ModelD1Ev(c + 0xd4);
    _ZN5ActorD1Ev(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, data_020a0eac);
    return c;
}
