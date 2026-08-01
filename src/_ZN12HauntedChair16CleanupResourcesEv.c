extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov020_02114af0[];
int _ZN12HauntedChair16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov020_02114af0);
    return 1;
}
