struct SharedFilePtr { unsigned int data[4]; };
extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *);
extern void _ZN16MeshColliderBase7DisableEv(void *);
extern struct SharedFilePtr data_ov002_0210d9c0;
extern struct SharedFilePtr *data_ov065_0211c08c[];
extern struct SharedFilePtr *data_ov065_0211c080[];
extern struct SharedFilePtr data_ov065_0211d720;
int _ZN6Dorrie16CleanupResourcesEv(char *c) {
    int i;
    char *p;
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9c0);
    p = c + 0x180;
    for (i = 0; i < 7; i++) {
        _ZN16MeshColliderBase7DisableEv(p);
        _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211c08c[i]);
        p += 0x200;
    }
    for (i = 0; i < 3; i++)
        _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211c080[i]);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov065_0211d720);
    return 1;
}
