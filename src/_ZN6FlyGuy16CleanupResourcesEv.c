extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov070_02123530[];
extern int data_ov070_02123520[];
extern int data_ov070_02123518[];
extern int data_ov070_02123510[];
extern int data_ov070_02123528[];
extern int data_ov070_02123508[];
extern int data_ov070_02123500[];
int _ZN6FlyGuy16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov070_02123530);
    _ZN13SharedFilePtr7ReleaseEv(data_ov070_02123520);
    _ZN13SharedFilePtr7ReleaseEv(data_ov070_02123518);
    _ZN13SharedFilePtr7ReleaseEv(data_ov070_02123510);
    _ZN13SharedFilePtr7ReleaseEv(data_ov070_02123528);
    _ZN13SharedFilePtr7ReleaseEv(data_ov070_02123508);
    _ZN13SharedFilePtr7ReleaseEv(data_ov070_02123500);
    return 1;
}
