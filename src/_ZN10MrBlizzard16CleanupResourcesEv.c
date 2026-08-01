extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov081_02128d90[];
extern int data_ov081_02128db0[];
extern int data_ov081_02128d98[];
extern int data_ov081_02128db8[];
extern int data_ov081_02128da8[];
extern int data_ov081_02128d88[];
extern int data_ov081_02128da0[];
int _ZN10MrBlizzard16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov081_02128d90);
    _ZN13SharedFilePtr7ReleaseEv(data_ov081_02128db0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov081_02128d98);
    _ZN13SharedFilePtr7ReleaseEv(data_ov081_02128db8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov081_02128da8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov081_02128d88);
    _ZN13SharedFilePtr7ReleaseEv(data_ov081_02128da0);
    return 1;
}
