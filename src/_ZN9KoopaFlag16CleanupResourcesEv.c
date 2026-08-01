extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov062_0211e0d4[];
extern int data_ov062_0211e0dc[];
int _ZN9KoopaFlag16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211e0d4);
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211e0dc);
    return 1;
}
