extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov009_02113c28[];
extern int data_ov009_02113c20[];
int _ZN4Bird16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov009_02113c28);
    _ZN13SharedFilePtr7ReleaseEv(data_ov009_02113c20);
    return 1;
}
