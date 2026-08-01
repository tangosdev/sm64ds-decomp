extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_02110aa4[];
extern int data_ov002_02110a9c[];
int _ZN9OneUpLogo16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_02110aa4);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_02110a9c);
    return 1;
}
