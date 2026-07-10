struct SharedFilePtr { int data[4]; };
extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *self);
extern struct SharedFilePtr data_ov070_021235fc;
extern struct SharedFilePtr data_ov070_02123604;
extern struct SharedFilePtr *func_ov074_021222e0[2];
extern struct SharedFilePtr data_ov070_021235ec;

int _ZN3Amp16CleanupResourcesEv(void)
{
    int r5;
    _ZN13SharedFilePtr7ReleaseEv(&data_ov070_021235fc);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov070_02123604);
    r5 = 0;
    do {
        _ZN13SharedFilePtr7ReleaseEv(func_ov074_021222e0[r5]);
        r5++;
    } while (r5 < 2);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov070_021235ec);
    return 1;
}
