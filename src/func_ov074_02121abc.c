//cpp
// NONMATCHING: register allocation (div=3). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
int func_ov074_0212229c(void);
void _ZN13SharedFilePtr7ReleaseEv(void* thiz);
void UnloadKeyModels(int i);
extern char SUPER_MUSHROOM_MODEL_PTR;
extern char data_ov084_02130cf8;
extern void* data_ov074_0212292c[];
extern char data_ov074_02123000;
extern void* data_ov074_02122948[];
extern char data_ov074_02123040;

int func_ov074_02121abc(int* c)
{
    int i;
    int v = c[2];
    if (v == 0x1111) {
        return func_ov074_0212229c();
    }
    _ZN13SharedFilePtr7ReleaseEv(&SUPER_MUSHROOM_MODEL_PTR);
    UnloadKeyModels(2);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov084_02130cf8);
    for (i = 0; i < 7; i++)
        _ZN13SharedFilePtr7ReleaseEv(data_ov074_0212292c[i]);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov074_02123000);
    for (i = 0; i < 0xc; i++)
        _ZN13SharedFilePtr7ReleaseEv(data_ov074_02122948[i]);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov074_02123040);
    return 1;
}
}
