// @symbol _ZN15MaterialChangerD1Ev
struct MaterialChanger { void **vtable; };
extern void *_ZTV15MaterialChanger[];
extern void _ZN9AnimationD2Ev(struct MaterialChanger *thiz);

struct MaterialChanger *_ZN15MaterialChangerD1Ev(struct MaterialChanger *thiz)
{
    thiz->vtable = (void *)_ZTV15MaterialChanger;
    _ZN9AnimationD2Ev(thiz);
    return thiz;
}
