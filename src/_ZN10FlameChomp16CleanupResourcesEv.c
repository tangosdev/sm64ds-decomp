extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov070_02123698[];
int _ZN10FlameChomp16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov070_02123698);
    return 1;
}
