typedef int s32;
struct SharedFilePtr { int data[4]; };
extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *self);
extern struct SharedFilePtr data_ov072_02122cb4;
extern struct SharedFilePtr *data_ov072_02122004[];

s32 _ZN11BabyPenguin16CleanupResourcesEv(void) {
    _ZN13SharedFilePtr7ReleaseEv(&data_ov072_02122cb4);
    s32 r5 = 0;
    do {
        _ZN13SharedFilePtr7ReleaseEv(data_ov072_02122004[r5]);
        r5++;
    } while (r5 < 5);
    return 1;
}
