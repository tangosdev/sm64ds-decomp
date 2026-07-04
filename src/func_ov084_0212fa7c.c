typedef struct BCA_File BCA_File;

extern int _ZN9Animation8FinishedEv(void *a);
extern int _ZNK9Animation12WillHitFrameEi(void *a, int f);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *m, BCA_File *f, int a, int fix, unsigned int j);
extern int func_ov084_0212f1d0(char *c);
extern void func_02012694(int a, void *v);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, int x, int y, int z);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, void *v, unsigned int d);
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int fix, int e);
extern void func_ov084_0212ef00(char *self);

extern int *data_ov084_02130df4;
extern int *data_ov084_02130e0c;

void func_ov084_0212fa7c(char *c) {
    *(unsigned char *)(c + 0x45c) = 1;
    if (_ZN9Animation8FinishedEv(c + 0x160) || _ZNK9Animation12WillHitFrameEi(c + 0x160, 0)) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, (BCA_File *)((int *)&data_ov084_02130df4)[1], 0, 0x1000, 0);
    }
    {
        int thr = (*(int *)(c + 0x46c) != 0) ? 0x12c000 : 0x190000;
        if (*(int *)(c + 0x464) < thr && func_ov084_0212f1d0(c)) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, (BCA_File *)((int *)&data_ov084_02130e0c)[1], 0x40000000, 0x1000, 0);
            *(int *)(c + 0x458) = 3;
            func_02012694(0x175, c + 0x74);
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xfe, *(int *)(c + 0x434), *(int *)(c + 0x438), *(int *)(c + 0x43c));
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xff, *(int *)(c + 0x434), *(int *)(c + 0x438), *(int *)(c + 0x43c));
            return;
        }
    }
    *(int *)(c + 0x478) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(int *)(c + 0x478), 3, 0x174, c + 0x74, 0);
    if (*(int *)(c + 0x464) < 0x352000) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x36, 0, 0x7f, 0x1451, 0);
        *(unsigned char *)(c + 0x45d) = 0;
    } else if (*(unsigned char *)(c + 0x45d) == 0) {
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x36, 0x7f, 0, 0x1451, 0))
            *(unsigned char *)(c + 0x45d) = 1;
    }
    func_ov084_0212ef00(c);
}
