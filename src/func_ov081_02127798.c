extern void _ZN13SharedFilePtr7ReleaseEv(void *self);
extern void *data_ov081_02128ed4;
extern void *data_ov081_021280d8[];
int func_ov081_02127798(void) {
    _ZN13SharedFilePtr7ReleaseEv(&data_ov081_02128ed4);
    for (int i = 0; i < 4; i++) {
        _ZN13SharedFilePtr7ReleaseEv(data_ov081_021280d8[i]);
    }
    return 1;
}
