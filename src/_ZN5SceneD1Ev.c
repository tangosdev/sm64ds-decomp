extern void *_ZTV5Stage;
extern void *data_0208e4b8;
extern void *_ZN9ActorBaseD2Ev(void *self);

struct Scene {
    void *vtable;
};

void *_ZN5SceneD1Ev(struct Scene *self)
{
    self->vtable = &_ZTV5Stage;
    self->vtable = &data_0208e4b8;
    _ZN9ActorBaseD2Ev(self);
    return self;
}
