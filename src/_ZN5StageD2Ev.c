struct Stage { void **vtable; };
extern void *data_020921c0[];  /* the table this dtor actually installs; not _ZTV5Stage (0x02092680) */
extern void *_ZTV5Scene[];
extern void *_ZTV12ActorDerived[];
extern void _ZN12MeshColliderD1Ev(void *thiz);
extern void _ZN5ModelD1Ev(void *thiz);
extern void _ZN8Particle10SysTrackerD1Ev(void *thiz);
extern void _ZN9ActorBaseD2Ev(struct Stage *thiz);

struct Stage *_ZN5StageD2Ev(struct Stage *thiz)
{
    thiz->vtable = (void **)data_020921c0;
    _ZN12MeshColliderD1Ev((char *)thiz + 0x91c);
    _ZN5ModelD1Ev((char *)thiz + 0x86c);
    _ZN8Particle10SysTrackerD1Ev((char *)thiz + 0x50);
    thiz->vtable = (void **)_ZTV5Scene;
    thiz->vtable = (void **)_ZTV12ActorDerived;
    _ZN9ActorBaseD2Ev(thiz);
    return thiz;
}
