//cpp
typedef int Fix12;
struct BCA_File;
struct ModelAnim { int pad; void SetAnim(BCA_File *f, int a, Fix12 b, unsigned int c); };
extern "C" {
extern int _ZN7Vector38IDENTITYE[];
extern int data_ov006_02140564[];
extern double data_ov006_0213b11c;
}

extern "C" void func_ov006_020ca3a8(char *c)
{
    *(short *)(c + 0x6e) = 0;
    *(int *)(c + 0x28) = 0;
    *(int *)(c + 0x24) = 0;
    *(int *)(c + 0x2c) = 0;
    *(int *)(c + 0x3c) = _ZN7Vector38IDENTITYE[0];
    *(int *)(c + 0x40) = _ZN7Vector38IDENTITYE[1];
    *(int *)(c + 0x44) = _ZN7Vector38IDENTITYE[2];
    *(int *)(c + 0x48) = 0;
    *(int *)(c + 0x64) = 0;
    ((ModelAnim *)(c + 0x78))->SetAnim((BCA_File *)data_ov006_02140564[0], 0, 0x800, 0);
    *(double *)(c + 0x70) = data_ov006_0213b11c;
}
