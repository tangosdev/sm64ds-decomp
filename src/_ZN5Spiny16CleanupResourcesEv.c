extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov077_02127b48[];
extern int data_ov077_02127b38[];
extern int data_ov077_02127c14[];
int _ZN5Spiny16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov077_02127b48);
    _ZN13SharedFilePtr7ReleaseEv(data_ov077_02127b38);
    _ZN13SharedFilePtr7ReleaseEv(data_ov077_02127c14);
    return 1;
}
