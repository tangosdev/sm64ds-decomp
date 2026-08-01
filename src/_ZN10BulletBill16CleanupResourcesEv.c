extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov079_02128300[];
extern int data_ov079_021282f0[];
int _ZN10BulletBill16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov079_02128300);
    _ZN13SharedFilePtr7ReleaseEv(data_ov079_021282f0);
    return 1;
}
