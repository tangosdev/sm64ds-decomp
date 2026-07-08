extern void func_0203d2fc(int *out, void *a, void *b);
extern int func_0203ce2c(int *v);
extern void func_02016748(void *self, int anim, int a, int speed, int b);
extern void func_020e6df0(int a, int b, int c);
extern int data_02140408[];
extern int data_ov006_0213b028[];

void func_ov006_020c7ba4(char *c)
{
    int v[3];
    func_0203d2fc(v, c + 0x14, c + 4);
    if (func_0203ce2c(v) != 0) {
        *(int *)(c + 0x20) = v[0];
        *(int *)(c + 0x24) = v[1];
        *(int *)(c + 0x28) = v[2];
    } else {
        *(int *)(c + 0x20) = -*(int *)(c + 0x20);
        *(int *)(c + 0x24) = -*(int *)(c + 0x24);
    }
    func_02016748(c + 0x4c, data_02140408[0], 0x40000000, 0x800, 0);
    *(int *)(c + 0xa4) = 0;
    func_020e6df0(0, 6, *(int *)(c + 0x14));
    *(short *)(c + 0x32) = 0x20;
    {
        int w0 = data_ov006_0213b028[0];
        int w1 = data_ov006_0213b028[1];
        w0 = w1 ? w0 : w0;
        *(int *)(c + 0x3c) = w0;
        *(int *)(c + 0x40) = w1;
    }
}
