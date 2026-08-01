extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov014_02114968[];
extern int data_ov014_02114978[];
extern int data_ov014_02114980[];
extern int data_ov014_02114970[];
int _ZN10ChainChomp16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov014_02114968);
    _ZN13SharedFilePtr7ReleaseEv(data_ov014_02114978);
    _ZN13SharedFilePtr7ReleaseEv(data_ov014_02114980);
    _ZN13SharedFilePtr7ReleaseEv(data_ov014_02114970);
    return 1;
}
