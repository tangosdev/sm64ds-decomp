extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov100_02148608[];
extern int data_ov100_02148600[];
extern int data_ov002_0210d9d8[];
extern int data_ov100_02148668[];
int _ZN9Butterfly16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov100_02148608);
    _ZN13SharedFilePtr7ReleaseEv(data_ov100_02148600);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9d8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov100_02148668);
    return 1;
}
