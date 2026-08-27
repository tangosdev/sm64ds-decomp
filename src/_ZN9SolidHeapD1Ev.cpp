extern void *_ZTV9SolidHeap;
extern void *_ZN4HeapD2Ev(void *self);

struct SolidHeap {
    void *vtable;
};

void *_ZN9SolidHeapD1Ev(struct SolidHeap *self)
{
    self->vtable = &_ZTV9SolidHeap;
    _ZN4HeapD2Ev(self);
    return self;
}
