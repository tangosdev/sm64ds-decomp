extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int G0[];
extern int G1[];
extern int G2[];
int _ZN6Cannon16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(G0);
    _ZN13SharedFilePtr7ReleaseEv(G1);
    _ZN13SharedFilePtr7ReleaseEv(G2);
    return 1;
}
