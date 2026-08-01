extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov064_0211c9c4[];
extern int data_ov064_0211c9cc[];
extern int data_ov064_0211c9bc[];
int _ZN4Clam16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov064_0211c9c4);
    _ZN13SharedFilePtr7ReleaseEv(data_ov064_0211c9cc);
    _ZN13SharedFilePtr7ReleaseEv(data_ov064_0211c9bc);
    return 1;
}
