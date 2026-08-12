extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210e104[];
extern int data_ov002_0210e0fc[];
int _ZN7Seaweed16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210e104);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210e0fc);
    return 1;
}
