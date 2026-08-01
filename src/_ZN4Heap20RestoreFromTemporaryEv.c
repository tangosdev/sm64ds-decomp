// Heap::RestoreFromTemporary: sets default heap to tmpHeapPtr and resets tmpHeapPtr to null

typedef void Heap;

extern Heap* _ZN6Memory14defaultHeapPtrE;
extern Heap* data_020a0ea8;

extern Heap* _ZN4Heap10SetDefaultEv(Heap* self);

void _ZN4Heap20RestoreFromTemporaryEv(void) {
    _ZN4Heap10SetDefaultEv(data_020a0ea8);
    data_020a0ea8 = (Heap*)0;
}
