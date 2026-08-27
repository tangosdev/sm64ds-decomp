//cpp
struct BCA_File;

extern "C" int _ZN9Animation8FinishedEv(void *thiz);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, struct BCA_File *f, int i, int t, unsigned int u);
extern "C" int func_ov084_0212f1d0(char *c);
extern void *data_ov084_02130e0c[];

extern "C" void func_ov084_0212f588(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    *(unsigned char *)(c + 0x45c) = 1;
    if (_ZN9Animation8FinishedEv(c + 0x160)) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, (struct BCA_File *)data_ov084_02130e0c[1], 0x40000000, 0x1000, 0);
        *(int *)(c + 0x458) = 1;
    } else {
        int thresh = (*(int *)(c + 0x46c) != 0) ? 0x12c000 : 0x190000;
        if (*(int *)(c + 0x464) < thresh) {
            if (func_ov084_0212f1d0((char *)c) != 0) {
                *(int *)(c + 0x168) = 0;
                *(int *)(c + 0x458) = 2;
            }
        }
    }
}
