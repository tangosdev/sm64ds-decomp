extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov071_02122f80[];
extern int data_ov071_02122f88[];
int _ZN10Scuttlebug16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov071_02122f80);
    _ZN13SharedFilePtr7ReleaseEv(data_ov071_02122f88);
    return 1;
}
