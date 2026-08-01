extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210d9d0[];
extern int data_ov002_0210d9b0[];
int _ZN9PushBlock16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9d0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9b0);
    return 1;
}
