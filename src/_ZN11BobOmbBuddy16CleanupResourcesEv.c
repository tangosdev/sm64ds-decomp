extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov084_02130da4[];
extern int data_ov084_02130d9c[];
int _ZN11BobOmbBuddy16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov084_02130da4);
    _ZN13SharedFilePtr7ReleaseEv(data_ov084_02130d9c);
    return 1;
}
