extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern char data_ov090_02134524[];
extern char data_ov002_0210da10[];
extern char data_ov002_0210d9a8[];
extern char data_ov090_0213452c[];
int _ZN8MantaRay16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov090_02134524);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da10);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9a8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov090_0213452c);
    return 1;
}
