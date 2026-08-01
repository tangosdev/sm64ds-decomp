extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov021_02114a50[];
int _ZN11RollingRock16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov021_02114a50);
    return 1;
}
