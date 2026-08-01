extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov016_02114d38[];
extern int data_ov016_02114d20[];
extern int data_ov016_02114d30[];
extern int data_ov016_02114d28[];
int _ZN5Unagi16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov016_02114d38);
    _ZN13SharedFilePtr7ReleaseEv(data_ov016_02114d20);
    _ZN13SharedFilePtr7ReleaseEv(data_ov016_02114d30);
    _ZN13SharedFilePtr7ReleaseEv(data_ov016_02114d28);
    return 1;
}
