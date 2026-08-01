extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov072_02122b20[];
int _ZN11SnowmanBody16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov072_02122b20);
    return 1;
}
