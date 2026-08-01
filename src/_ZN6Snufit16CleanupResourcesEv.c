extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov065_0211d610[];
extern int data_ov065_0211d618[];
extern int data_ov065_0211d600[];
extern int data_ov065_0211d608[];
int _ZN6Snufit16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211d610);
    _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211d618);
    _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211d600);
    _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211d608);
    return 1;
}
