extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210da40[];
extern int data_ov002_0210d9a0[];
extern int data_ov002_0210d9c0[];
extern int data_ov062_0211e0fc[];
extern int data_ov062_0211e114[];
extern int data_ov062_0211e10c[];
extern int data_ov062_0211e104[];
int _ZN6Klepto16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da40);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9a0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9c0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211e0fc);
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211e114);
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211e10c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211e104);
    return 1;
}
