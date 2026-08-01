extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov085_021305d8[];
extern int data_ov085_021305b8[];
extern int data_ov085_021305d0[];
extern int data_ov085_021305b0[];
extern int data_ov085_021305c8[];
extern int data_ov085_021305c0[];
extern int data_ov085_021305e0[];
int _ZN6Rabbit16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_021305d8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_021305b8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_021305d0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_021305b0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_021305c8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_021305c0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_021305e0);
    return 1;
}
