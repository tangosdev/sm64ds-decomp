// @symbol _ZN15TextureSequenceD1Ev
struct TextureSequence { void **vtable; };
extern void *_ZTV15TextureSequence[];
extern void _ZN9AnimationD2Ev(struct TextureSequence *thiz);

struct TextureSequence *_ZN15TextureSequenceD1Ev(struct TextureSequence *thiz)
{
    thiz->vtable = (void *)_ZTV15TextureSequence;
    _ZN9AnimationD2Ev(thiz);
    return thiz;
}
