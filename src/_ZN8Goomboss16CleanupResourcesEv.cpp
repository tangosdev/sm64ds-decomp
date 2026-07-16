//cpp
extern "C" {
int func_ov074_0212229c(int* c);
void _ZN13SharedFilePtr7ReleaseEv(void* thiz);
void UnloadKeyModels(int i);
extern char data_ov002_0210da30;
extern char data_ov084_02130cf8;
extern void* data_ov074_0212292c[];
extern char data_ov074_02123000;
extern void* data_ov074_02122948[];
extern char data_ov074_02123040;

int _ZN8Goomboss16CleanupResourcesEv(int* c)
{
    int i;
    int v = c[2];
    if (v == 0x1111) {
        return func_ov074_0212229c(c);
    }
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210da30);
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
