extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210d9f0[];
extern int data_ov025_02113ae0[];
extern int data_ov025_02113ad8[];
int _ZN11PyramidLift16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9f0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov025_02113ae0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov025_02113ad8);
    return 1;
}
