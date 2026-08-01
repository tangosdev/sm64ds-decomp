extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov085_0213074c[];
extern int data_ov085_02130744[];
extern int data_ov085_0213073c[];
int _ZN9LakituBro16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_0213074c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_02130744);
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_0213073c);
    return 1;
}
