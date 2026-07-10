extern int _ZN16MeshColliderBase9IsEnabledEv(void *);
extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210dd60[];
extern int data_ov002_0210dd68[];
extern int data_ov002_0210dd58[];
extern int data_ov002_0210dd50[];
int _ZN14QuestionSwitch16CleanupResourcesEv(char *t)
{
    if (_ZN16MeshColliderBase9IsEnabledEv(t + 0x324)) _ZN16MeshColliderBase7DisableEv(t + 0x324);
    if (_ZN16MeshColliderBase9IsEnabledEv(t + 0x4ec)) _ZN16MeshColliderBase7DisableEv(t + 0x4ec);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210dd60);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210dd68);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210dd58);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210dd50);
    return 1;
}
