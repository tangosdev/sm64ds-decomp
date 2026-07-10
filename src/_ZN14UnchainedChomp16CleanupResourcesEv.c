extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern void UnloadSilverStarAndNumber(void);
extern int G0[];
extern int G1[];
extern int G2[];
extern int G3[];
extern int G4[];
int _ZN14UnchainedChomp16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(G0);
    _ZN13SharedFilePtr7ReleaseEv(G1);
    _ZN13SharedFilePtr7ReleaseEv(G2);
    _ZN13SharedFilePtr7ReleaseEv(G3);
    _ZN13SharedFilePtr7ReleaseEv(G4);
    UnloadSilverStarAndNumber();
    return 1;
}
