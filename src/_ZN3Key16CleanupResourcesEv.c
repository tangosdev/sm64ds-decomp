extern void UnloadKeyModels(int i);
extern void _ZN13SharedFilePtr7ReleaseEv(void *self);
extern void _ZN5Event8ClearBitEj(unsigned int b);
extern int data_ov002_02110964[];
extern int data_ov089_02132c60[];
extern int data_ov089_02132c40[];
extern int data_ov089_02132c70[];
extern int data_ov089_02132c48[];

int _ZN3Key16CleanupResourcesEv(int *t)
{
    UnloadKeyModels(*(int *)((char *)t + 0x444));
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_02110964);
    if (*(int *)((char *)t + 0x444) != 7) {
        _ZN13SharedFilePtr7ReleaseEv(data_ov089_02132c60);
        _ZN13SharedFilePtr7ReleaseEv(data_ov089_02132c40);
        _ZN13SharedFilePtr7ReleaseEv(data_ov089_02132c70);
        _ZN13SharedFilePtr7ReleaseEv(data_ov089_02132c48);
    }
    _ZN5Event8ClearBitEj(0x1d);
    return 1;
}
