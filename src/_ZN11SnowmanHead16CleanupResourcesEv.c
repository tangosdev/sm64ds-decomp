typedef int s32;
struct SharedFilePtr { int data[4]; };
extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *self);
extern struct SharedFilePtr data_ov072_02122bc4;
extern struct SharedFilePtr *data_ov072_02121ffc[];

s32 _ZN11SnowmanHead16CleanupResourcesEv(void) {
    _ZN13SharedFilePtr7ReleaseEv(&data_ov072_02122bc4);
    s32 r5 = 0;
    do {
        _ZN13SharedFilePtr7ReleaseEv(data_ov072_02121ffc[r5]);
        r5++;
    } while (r5 < 2);
    return 1;
}
