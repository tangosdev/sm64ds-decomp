extern int _ZN16MeshColliderBase9IsEnabledEv(void *);
extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
int _ZN6Thwomp16CleanupResourcesEv(char *t) {
    void **fp;
    if (_ZN16MeshColliderBase9IsEnabledEv(t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv(t + 0x124);
    }
    fp = *(void***)(t + 0x320);
    _ZN13SharedFilePtr7ReleaseEv(fp[0]);
    fp = *(void***)(t + 0x320);
    _ZN13SharedFilePtr7ReleaseEv(fp[1]);
    fp = *(void***)(t + 0x320);
    if (fp[3] != 0) {
        _ZN13SharedFilePtr7ReleaseEv(fp[3]);
    }
    return 1;
}
