extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov080_021283c8[];
int _ZN13MontyMoleRock16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov080_021283c8);
    return 1;
}
