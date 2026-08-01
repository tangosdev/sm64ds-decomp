extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov081_02128d90[];
int _ZN8Snowball16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov081_02128d90);
    return 1;
}
