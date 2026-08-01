extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov036_02114070[];
int _ZN18RotatingPlatformRr16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov036_02114070);
    return 1;
}
