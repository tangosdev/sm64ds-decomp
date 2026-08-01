extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov096_02137ba8[];
extern int data_ov096_02137bb0[];
int _ZN7Tornado16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov096_02137ba8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov096_02137bb0);
    return 1;
}
