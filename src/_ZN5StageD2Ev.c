struct Stage { void **vtable; };
extern void *_ZTV5Stage[];  /* the table this dtor actually installs; not _ZTV5Scene (0x02092680) */
extern void *_ZTV5Scene[];
extern void *data_0208e4b8[];
extern void _ZN12MeshColliderD1Ev(void *thiz);
extern void _ZN5ModelD1Ev(void *thiz);
extern void _ZN8Particle10SysTrackerD1Ev(void *thiz);
extern void _ZN9ActorBaseD2Ev(struct Stage *thiz);

struct Stage *_ZN5StageD2Ev(struct Stage *thiz)
{
    thiz->vtable = (void **)_ZTV5Stage;
    _ZN12MeshColliderD1Ev((char *)thiz + 0x91c);
    _ZN5ModelD1Ev((char *)thiz + 0x86c);
    _ZN8Particle10SysTrackerD1Ev((char *)thiz + 0x50);
    thiz->vtable = (void **)_ZTV5Scene;
    thiz->vtable = (void **)data_0208e4b8;
    _ZN9ActorBaseD2Ev(thiz);
    return thiz;
}
