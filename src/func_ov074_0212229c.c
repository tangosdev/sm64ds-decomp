typedef int s32;
struct SharedFilePtr { int data[4]; };
extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *self);
extern struct SharedFilePtr data_ov084_02130cf8;
extern struct SharedFilePtr *data_ov074_0212292c[];

s32 func_ov074_0212229c(void) {
    _ZN13SharedFilePtr7ReleaseEv(&data_ov084_02130cf8);
    s32 r5 = 0;
    do {
        _ZN13SharedFilePtr7ReleaseEv(data_ov074_0212292c[r5]);
        r5++;
    } while (r5 < 7);
    return 1;
}
