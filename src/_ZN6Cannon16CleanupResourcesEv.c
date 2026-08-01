extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov098_0213c8e8[];
extern int data_ov098_0213c91c[];
extern int data_ov002_0210da38[];
int _ZN6Cannon16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov098_0213c8e8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov098_0213c91c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da38);
    return 1;
}
