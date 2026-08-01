extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov090_021344a0[];
extern int data_ov090_02134488[];
extern int data_ov090_02134480[];
extern int data_ov090_02134490[];
extern int data_ov090_02134498[];
int _ZN7Skeeter16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov090_021344a0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov090_02134488);
    _ZN13SharedFilePtr7ReleaseEv(data_ov090_02134480);
    _ZN13SharedFilePtr7ReleaseEv(data_ov090_02134490);
    _ZN13SharedFilePtr7ReleaseEv(data_ov090_02134498);
    return 1;
}
