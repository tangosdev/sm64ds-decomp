extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210da30[];
int _ZN21MegaMushroomCreateTag16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da30);
    return 1;
}
