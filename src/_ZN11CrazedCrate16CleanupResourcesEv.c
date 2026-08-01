extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov080_02128468[];
int _ZN11CrazedCrate16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov080_02128468);
    return 1;
}
