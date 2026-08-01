extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov060_0211b208[];
extern int data_ov060_0211b1f8[];
extern int data_ov060_0211b200[];
int _ZN16BowserShockwaves16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov060_0211b208);
    _ZN13SharedFilePtr7ReleaseEv(data_ov060_0211b1f8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov060_0211b200);
    return 1;
}
