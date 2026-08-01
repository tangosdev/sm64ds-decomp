extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov090_02134564[];
extern int data_ov090_0213455c[];
int _ZN10CheepCheep16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov090_02134564);
    _ZN13SharedFilePtr7ReleaseEv(data_ov090_0213455c);
    return 1;
}
