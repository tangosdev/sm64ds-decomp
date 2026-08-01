extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov065_0211d698[];
extern int data_ov065_0211d6a8[];
extern int data_ov065_0211d690[];
extern int data_ov065_0211d6a0[];
int _ZN5Swoop16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211d698);
    _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211d6a8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211d690);
    _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211d6a0);
    return 1;
}
