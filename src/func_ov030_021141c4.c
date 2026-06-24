typedef unsigned int u32;
struct SharedFilePtr { u32 data[4]; };

extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *self);

extern struct SharedFilePtr data_ov002_0210da40;
extern struct SharedFilePtr data_ov002_0210d9a0;
extern struct SharedFilePtr data_ov002_0210d9c0;
extern struct SharedFilePtr data_ov030_02115d00;
extern struct SharedFilePtr *data_ov030_02114824[10];

int func_ov030_021141c4(void)
{
    int i;
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210da40);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9a0);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9c0);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov030_02115d00);
    for (i = 0; i < 10; i++)
        _ZN13SharedFilePtr7ReleaseEv(data_ov030_02114824[i]);
    return 1;
}
