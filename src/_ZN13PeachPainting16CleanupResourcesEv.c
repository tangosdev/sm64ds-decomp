extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov010_02112d64[];
int _ZN13PeachPainting16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov010_02112d64);
    return 1;
}
