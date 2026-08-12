extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov010_02112d50[];
int _ZN4Trap16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov010_02112d50);
    return 1;
}
