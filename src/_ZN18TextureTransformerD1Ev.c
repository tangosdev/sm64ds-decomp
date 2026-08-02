// @symbol _ZN18TextureTransformerD1Ev
struct TextureTransformer { void **vtable; };
extern void *_ZTV18TextureTransformer[];
extern void _ZN9AnimationD2Ev(struct TextureTransformer *thiz);

struct TextureTransformer *_ZN18TextureTransformerD1Ev(struct TextureTransformer *thiz)
{
    thiz->vtable = (void *)_ZTV18TextureTransformer;
    _ZN9AnimationD2Ev(thiz);
    return thiz;
}
