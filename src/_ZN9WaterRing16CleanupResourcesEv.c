extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210da10[];
int _ZN9WaterRing16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da10);
    return 1;
}
