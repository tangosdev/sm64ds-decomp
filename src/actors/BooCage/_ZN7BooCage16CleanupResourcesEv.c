extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov063_0211edec[];
int _ZN7BooCage16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov063_0211edec);
    return 1;
}
