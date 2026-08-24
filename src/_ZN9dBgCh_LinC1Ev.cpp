//cpp
extern "C" void _ZN5dBgChC2Ev(void *c);
extern "C" int *_ZN5dBgPiC2Ev(void *t);
extern "C" void func_0203ac60(void *p);
extern void *data_020992a4;
extern void *data_020992b4;

extern "C" void *_ZN9dBgCh_LinC1Ev(char *thiz);

void *_ZN9dBgCh_LinC1Ev(char *thiz)
{
    _ZN5dBgChC2Ev(thiz);
    _ZN5dBgPiC2Ev(thiz + 0x10);
    *(void **)thiz = &data_020992a4;
    *(void **)(thiz + 0x10) = &data_020992b4;
    func_0203ac60(thiz + 0x64);
    *(int *)(thiz + 0x5c) = 0;
    *(int *)(thiz + 0x58) = *(int *)(thiz + 0x5c);
    *(int *)(thiz + 0x54) = *(int *)(thiz + 0x58);
    *(int *)(thiz + 0x60) = 0;
    return thiz;
}
