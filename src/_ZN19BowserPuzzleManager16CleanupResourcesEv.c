extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern void *data_ov064_0211adc8[];
extern void *data_ov075_0211c800;
int _ZN19BowserPuzzleManager16CleanupResourcesEv(void *t)
{
    unsigned char idx;
    _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    idx = *(unsigned char *)((char *)t + 0x337);
    _ZN13SharedFilePtr7ReleaseEv(data_ov064_0211adc8[idx]);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov075_0211c800);
    return 1;
}
