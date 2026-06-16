extern void _ZN13SharedFilePtr7ReleaseEv(void *self);
extern int data_ov002_0210da40[];
extern int data_ov002_0210d9a0[];
extern int data_ov002_0210d9c0[];
extern int data_ov085_02130480[];
extern int data_ov085_02130488[];
extern int data_ov085_02130490[];

int func_ov085_021297e8(void *c)
{
    if (*(unsigned char *)((char *)c + 0x20b) == 1) {
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da40);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9a0);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9c0);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_02130480);
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_02130488);
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_02130490);
    return 1;
}
