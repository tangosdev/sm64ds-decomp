extern void *_ZTV13ExpandingHeap;
extern void *_ZN4HeapD2Ev(void *self);

struct ExpandingHeap {
    void *vtable;
};

void *_ZN13ExpandingHeapD1Ev(struct ExpandingHeap *self)
{
    self->vtable = &_ZTV13ExpandingHeap;
    _ZN4HeapD2Ev(self);
    return self;
}
