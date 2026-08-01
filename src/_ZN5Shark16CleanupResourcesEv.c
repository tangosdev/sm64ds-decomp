extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov090_021345a4[];
extern int data_ov090_021345ac[];
int _ZN5Shark16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov090_021345a4);
    _ZN13SharedFilePtr7ReleaseEv(data_ov090_021345ac);
    return 1;
}
