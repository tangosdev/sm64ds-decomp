extern int _ZN16MeshColliderBase9IsEnabledEv(void *);
extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern char data_ov015_02114534[];
extern char data_ov015_02114538[];
int _ZN14KnockDownPlank16CleanupResourcesEv(char *c)
{
    if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124))
        _ZN16MeshColliderBase7DisableEv(c + 0x124);
    _ZN13SharedFilePtr7ReleaseEv(*(void **)(data_ov015_02114534 + *(int *)(c + 0x32c) * 0xc));
    _ZN13SharedFilePtr7ReleaseEv(*(void **)(data_ov015_02114538 + *(int *)(c + 0x32c) * 0xc));
    return 1;
}
