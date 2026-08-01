extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210da58[];
int _ZN11WingFeather16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da58);
    return 1;
}
