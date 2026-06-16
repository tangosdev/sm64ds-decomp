typedef int s32;
struct SharedFilePtr { int data[4]; };
extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *self);
extern struct SharedFilePtr data_ov085_021304f4;
extern struct SharedFilePtr *data_ov085_0212f280[];

s32 func_ov074_0212229c(void) {
    _ZN13SharedFilePtr7ReleaseEv(&data_ov085_021304f4);
    s32 r5 = 0;
    do {
        _ZN13SharedFilePtr7ReleaseEv(data_ov085_0212f280[r5]);
        r5++;
    } while (r5 < 7);
    return 1;
}
