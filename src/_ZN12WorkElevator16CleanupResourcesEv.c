extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov021_021149b0[];
extern int data_ov021_021149b8[];
extern int data_ov021_021149a0[];
extern int data_ov021_021149a8[];
int _ZN12WorkElevator16CleanupResourcesEv(char *t)
{
    int i;
    char *p = t;
    _ZN16MeshColliderBase7DisableEv(p + 0x124);
    p += 0x520;
    for (i = 0; i < 4; i++) {
        _ZN16MeshColliderBase7DisableEv(p);
        p += 0x1c8;
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov021_021149b0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov021_021149b8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov021_021149a0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov021_021149a8);
    return 1;
}
