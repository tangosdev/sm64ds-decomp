struct Stage { void **vtable; };
struct Heap;
extern void *data_020921c0[];  /* the table this dtor actually installs; not _ZTV5Stage (0x02092680) */
extern void *_ZTV5Stage[];
extern void *data_0208e4b8[];
extern void _ZN12MeshColliderD1Ev(void *thiz);
extern void _ZN5ModelD1Ev(void *thiz);
extern void _ZN8Particle10SysTrackerD1Ev(void *thiz);
extern void _ZN9ActorBaseD2Ev(struct Stage *thiz);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *ptr, struct Heap *heap);
extern struct Heap *data_020a0eac;

struct Stage *_ZN5StageD0Ev(struct Stage *thiz)
{
    thiz->vtable = (void **)data_020921c0;
    _ZN12MeshColliderD1Ev((char *)thiz + 0x91c);
    _ZN5ModelD1Ev((char *)thiz + 0x86c);
    _ZN8Particle10SysTrackerD1Ev((char *)thiz + 0x50);
    thiz->vtable = (void **)_ZTV5Stage;
    thiz->vtable = (void **)data_0208e4b8;
    _ZN9ActorBaseD2Ev(thiz);
    _ZN6Memory10DeallocateEPvP4Heap(thiz, data_020a0eac);
    return thiz;
}
