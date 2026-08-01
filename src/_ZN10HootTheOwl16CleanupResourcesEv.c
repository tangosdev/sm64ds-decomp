extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov094_02136ae0[];
extern int data_ov094_02136af8[];
extern int data_ov094_02136ae8[];
extern int data_ov094_02136af0[];
int _ZN10HootTheOwl16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov094_02136ae0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov094_02136af8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov094_02136ae8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov094_02136af0);
    return 1;
}
