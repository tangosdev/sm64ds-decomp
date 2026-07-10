struct SharedFilePtr { unsigned int data[4]; };
extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *);
extern void _ZN5Sound22StopLoadedMusic_Layer2Ev(void);
extern struct SharedFilePtr data_ov062_0211e00c;
extern struct SharedFilePtr data_ov062_0211e014;
extern struct SharedFilePtr data_ov062_0211e024;
extern struct SharedFilePtr data_ov062_0211e01c;
extern struct SharedFilePtr data_ov062_0211e034;
extern struct SharedFilePtr data_ov062_0211e03c;
extern struct SharedFilePtr data_ov062_0211e02c;
extern struct SharedFilePtr data_ov062_0211e004;
int _ZN13KoopaTheQuick16CleanupResourcesEv(char *c) {
    _ZN13SharedFilePtr7ReleaseEv(&data_ov062_0211e00c);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov062_0211e014);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov062_0211e024);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov062_0211e01c);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov062_0211e034);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov062_0211e03c);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov062_0211e02c);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov062_0211e004);
    if (*(unsigned char*)(c + 0x3b5)) _ZN5Sound22StopLoadedMusic_Layer2Ev();
    return 1;
}
