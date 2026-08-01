extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210ebb8[];
extern int data_ov002_0210eb20[];
extern int data_ov002_0210eaa0[];
int _ZN11MirrorLuigi16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210ebb8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210eb20);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210eaa0);
    return 1;
}
