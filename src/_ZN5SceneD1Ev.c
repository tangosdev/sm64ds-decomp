extern void *_ZTV5Scene;
extern void *_ZTV12ActorDerived;
extern void *_ZN9ActorBaseD2Ev(void *self);

struct Scene {
    void *vtable;
};

void *_ZN5SceneD1Ev(struct Scene *self)
{
    self->vtable = &_ZTV5Scene;
    self->vtable = &_ZTV12ActorDerived;
    _ZN9ActorBaseD2Ev(self);
    return self;
}
