extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int G0[];
extern int G1[];
int _ZN10BulletBill16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(G0);
    _ZN13SharedFilePtr7ReleaseEv(G1);
    return 1;
}
