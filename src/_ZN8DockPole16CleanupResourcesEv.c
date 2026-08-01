extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov009_02113eb8[];
extern int data_ov009_02113eb0[];
int _ZN8DockPole16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov009_02113eb8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov009_02113eb0);
    return 1;
}
