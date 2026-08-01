extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov085_021305d8[];
int _ZN9RabbitKey16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_021305d8);
    return 1;
}
