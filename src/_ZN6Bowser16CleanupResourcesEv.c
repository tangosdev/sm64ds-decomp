struct SharedFilePtr { unsigned int data[4]; };
extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *);
extern void func_02011cfc(void);
extern struct SharedFilePtr data_ov060_0211ac78;
extern struct SharedFilePtr *data_ov060_021192dc[];
extern struct SharedFilePtr *data_ov060_0211927c[];
extern struct SharedFilePtr data_ov060_0211b208;
extern struct SharedFilePtr data_ov089_02132c50;
int _ZN6Bowser16CleanupResourcesEv(void) {
    int i;
    _ZN13SharedFilePtr7ReleaseEv(&data_ov060_0211ac78);
    for (i = 0; i < 0x1c; i++)
        _ZN13SharedFilePtr7ReleaseEv(data_ov060_021192dc[i]);
    for (i = 0; i < 6; i++)
        _ZN13SharedFilePtr7ReleaseEv(data_ov060_0211927c[i]);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov060_0211b208);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov089_02132c50);
    func_02011cfc();
    return 1;
}
