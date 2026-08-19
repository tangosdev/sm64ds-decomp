extern void *_ZTV16MeshColliderBase;
extern void _ZN6Memory16operator_delete2EPv(void *ptr);

struct MeshColliderBase {
    void *vtable;
};

void *_ZN16MeshColliderBaseD0Ev(struct MeshColliderBase *self)
{
    self->vtable = &_ZTV16MeshColliderBase;
    _ZN6Memory16operator_delete2EPv(self);
    return self;
}
