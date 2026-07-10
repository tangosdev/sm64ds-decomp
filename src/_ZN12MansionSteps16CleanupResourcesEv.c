extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov063_0211e27c[];
extern int data_ov063_0211e28c[];
int _ZN12MansionSteps16CleanupResourcesEv(void *t)
{
    _ZN16MeshColliderBase7DisableEv((char *)t + 0x15c);
    int idx = *(int*)((char*)t + 0x140);
    _ZN13SharedFilePtr7ReleaseEv((void*)(data_ov063_0211e27c[idx]));
    idx = *(int*)((char*)t + 0x140);
    _ZN13SharedFilePtr7ReleaseEv((void*)(data_ov063_0211e28c[idx]));
    return 1;
}
