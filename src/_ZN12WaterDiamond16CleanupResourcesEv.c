extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov029_02114270[];
int _ZN12WaterDiamond16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov029_02114270);
    return 1;
}
