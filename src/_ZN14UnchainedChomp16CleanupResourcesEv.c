extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern void UnloadSilverStarAndNumber(void);
extern int data_ov002_0211092c[];
extern int data_ov100_021486bc[];
extern int data_ov100_021486a4[];
extern int data_ov100_021486ac[];
extern int data_ov100_021486b4[];
int _ZN14UnchainedChomp16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0211092c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov100_021486bc);
    _ZN13SharedFilePtr7ReleaseEv(data_ov100_021486a4);
    _ZN13SharedFilePtr7ReleaseEv(data_ov100_021486ac);
    _ZN13SharedFilePtr7ReleaseEv(data_ov100_021486b4);
    UnloadSilverStarAndNumber();
    return 1;
}
