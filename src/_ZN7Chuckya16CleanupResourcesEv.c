extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov062_0211ddf0[];
extern int data_ov062_0211dde8[];
extern int data_ov062_0211dde0[];
extern int data_ov062_0211de00[];
extern int data_ov062_0211de08[];
extern int data_ov062_0211ddf8[];
int _ZN7Chuckya16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211ddf0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211dde8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211dde0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211de00);
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211de08);
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211ddf8);
    return 1;
}
