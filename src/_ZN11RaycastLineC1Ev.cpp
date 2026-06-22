//cpp
extern "C" void func_02035514(void *c);
extern "C" int *func_0203819c(void *t);
extern "C" void func_0203ac60(void *p);
extern void *data_020992a4;
extern void *data_020992b4;

extern "C" void *_ZN11RaycastLineC1Ev(char *thiz);

void *_ZN11RaycastLineC1Ev(char *thiz)
{
    func_02035514(thiz);
    func_0203819c(thiz + 0x10);
    *(void **)thiz = &data_020992a4;
    *(void **)(thiz + 0x10) = &data_020992b4;
    func_0203ac60(thiz + 0x64);
    *(int *)(thiz + 0x5c) = 0;
    *(int *)(thiz + 0x58) = *(int *)(thiz + 0x5c);
    *(int *)(thiz + 0x54) = *(int *)(thiz + 0x58);
    *(int *)(thiz + 0x60) = 0;
    return thiz;
}
