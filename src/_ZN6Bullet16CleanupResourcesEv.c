extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int G0[];
int _ZN6Bullet16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(G0);
    return 1;
}
