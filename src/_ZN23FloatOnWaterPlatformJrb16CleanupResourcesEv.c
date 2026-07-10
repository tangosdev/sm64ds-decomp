extern int _ZN16MeshColliderBase9IsEnabledEv(void *);
extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int G0[];
extern int G1[];
int _ZN23FloatOnWaterPlatformJrb16CleanupResourcesEv(void *t)
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(G0);
    _ZN13SharedFilePtr7ReleaseEv(G1);
    return 1;
}
