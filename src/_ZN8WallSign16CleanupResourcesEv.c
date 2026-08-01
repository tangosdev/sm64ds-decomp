extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov085_02130858[];
int _ZN8WallSign16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_02130858);
    return 1;
}
