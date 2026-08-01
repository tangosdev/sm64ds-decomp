extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov081_02128d60[];
extern int data_ov081_02128d68[];
int _ZN9Spindrift16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov081_02128d60);
    _ZN13SharedFilePtr7ReleaseEv(data_ov081_02128d68);
    return 1;
}
