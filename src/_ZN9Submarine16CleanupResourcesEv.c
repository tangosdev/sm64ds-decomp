extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov026_02113f0c[];
extern int data_ov026_02113f04[];
int _ZN9Submarine16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov026_02113f0c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov026_02113f04);
    return 1;
}
