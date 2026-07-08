//cpp
extern "C" {
extern int data_ov006_021405c8;
struct V3 { int a, b, c; };
extern struct V3 _ZN7Vector38IDENTITYE;
extern void *data_ov006_02140564;
struct G2 { int w[2]; };
extern struct G2 data_ov006_0213b174;
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *bca, int frame, int speed, unsigned int flags);
void func_ov006_020ccd78(char *c)
{
    *(int *)(c + 0x20) = data_ov006_021405c8;
    *(int *)(c + 0x1c) = -0x90000;
    *(int *)(c + 0x24) = 0;
    *(int *)(c + 0x34) = _ZN7Vector38IDENTITYE.a;
    *(int *)(c + 0x38) = _ZN7Vector38IDENTITYE.b;
    *(int *)(c + 0x3c) = _ZN7Vector38IDENTITYE.c;
    *(int *)(c + 0x40) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x6c, data_ov006_02140564, 0, 0x800, 0);
    *(struct G2 *)(c + 0x64) = data_ov006_0213b174;
}
}
