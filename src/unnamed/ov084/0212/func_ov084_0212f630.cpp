//cpp
struct BCA_File;

extern "C" void _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int t, char flag);
extern "C" int func_ov084_0212ef00(void *thiz);
extern "C" int _ZN9Animation8FinishedEv(void *thiz);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, struct BCA_File *f, int i, int t, unsigned int u);
extern void *data_ov084_02130e14[];

extern "C" void func_ov084_0212f630(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x36, 0x7f, 0, 0xcb33, 0);
    if (func_ov084_0212ef00(c) != 0) return;
    if (*(unsigned short *)(c + 0x100) <= 0xb) return;
    if (!_ZN9Animation8FinishedEv(c + 0x160)) return;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, (struct BCA_File *)data_ov084_02130e14[1], 0x40000000, 0x1000, 0);
    *(int *)(c + 0x458) = 2;
}
