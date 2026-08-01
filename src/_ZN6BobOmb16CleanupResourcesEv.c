extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210d9e0[];
extern int data_ov102_0214e9c0[];
extern int data_ov102_0214e9c8[];
int _ZN6BobOmb16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9e0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e9c0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e9c8);
    return 1;
}
