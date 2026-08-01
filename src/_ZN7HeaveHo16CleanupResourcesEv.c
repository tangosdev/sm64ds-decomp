extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov077_02127c88[];
extern int data_ov077_02127ca0[];
extern int data_ov077_02127c90[];
extern int data_ov077_02127c98[];
int _ZN7HeaveHo16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov077_02127c88);
    _ZN13SharedFilePtr7ReleaseEv(data_ov077_02127ca0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov077_02127c90);
    _ZN13SharedFilePtr7ReleaseEv(data_ov077_02127c98);
    return 1;
}
