// @symbol _ZN18TextureTransformerC1Ev
/* Stays a mangled free function: the destructor is the key function.
 * See include/ModelBase.h. */
struct TextureTransformer {
    unsigned int *vtable;      /* 0x00 */
    char pad[0xc];
    unsigned int file;         /* 0x10 */
};
extern void _ZN9AnimationC2Ev(void *self);
extern unsigned int _ZTV18TextureTransformer[];

struct TextureTransformer *_ZN18TextureTransformerC1Ev(struct TextureTransformer *self) {
    _ZN9AnimationC2Ev(self);
    self->vtable = _ZTV18TextureTransformer;
    self->file = 0;
    return self;
}
