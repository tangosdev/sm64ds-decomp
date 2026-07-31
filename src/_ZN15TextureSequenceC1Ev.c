#include "types.h"
// TextureSequence constructor - C++ style to get correct return value
struct Animation {
    void** vtable;
    Fix12i numFramesAndFlags;
    Fix12i currFrame;
    Fix12i speed;
};

struct TextureSequence {
    struct Animation base;
    void* file;
};

extern void _ZN9AnimationC2Ev(struct Animation* self);
extern void* _ZTV15TextureSequence[];

struct TextureSequence* _ZN15TextureSequenceC1Ev(struct TextureSequence* self) {
    _ZN9AnimationC2Ev(&self->base);
    self->base.vtable = (void*)_ZTV15TextureSequence;
    self->file = 0;
    return self;
}