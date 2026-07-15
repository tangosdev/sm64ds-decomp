//cpp
typedef int Fix12;
struct BCA_File;
struct ModelAnim {
    void SetAnim(BCA_File *f, int a, Fix12 b, unsigned int c);
};

extern "C" {
extern int data_ov006_0213b214[2];
extern int data_ov006_0213b0fc[2];
extern int data_ov006_02140540[];
extern int data_ov006_0213b114[2];
extern void func_ov006_020ca3a8(char *c);
extern void func_ov006_020e6e3c(int a, int b);
}

extern "C" void func_ov006_020c9098(char *c)
{
    int *p;
    int *d;

    p = (int *)(((long long)(int)(c + 0x70)) & 0xFFFFFFFFFFFFFFFFLL);
    d = data_ov006_0213b214;
    if (p[0] == d[0]) {
        if (p[1] != d[1]) {
            if (*(int *)(c + 0x70) != 0)
                goto check2;
        }
        return;
    }
check2:
    p = (int *)(((long long)(unsigned)(c + 0x70)) & 0xFFFFFFFFFFFFFFFFLL);
    d = data_ov006_0213b0fc;
    if (p[0] == d[0]) {
        if (p[1] != d[1]) {
            if (*(int *)(c + 0x70) != 0)
                goto body;
        }
        func_ov006_020ca3a8(c);
        return;
    }
body:
    *(int *)(c + 0x48) = 0x100;
    *(int *)(c + 0x3c) = 0;
    *(int *)(c + 0x40) = 0x2000;
    func_ov006_020e6e3c(0x1c9, *(int *)(c + 0x24));
    ((ModelAnim *)(c + 0x78))->SetAnim((BCA_File *)data_ov006_02140540[0], 0, 0x800, 0);
    *(int *)(c + 0xd0) = 0;
    {
        int a = data_ov006_0213b114[0];
        int b = data_ov006_0213b114[1];
        *(int *)(c + 0x70) = b ? a : a;
        *(int *)(c + 0x74) = b;
    }
}
