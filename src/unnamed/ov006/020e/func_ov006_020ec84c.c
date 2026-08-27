extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern int func_ov006_020ebeb4(char *c);
extern void func_ov006_020ec2bc(char *c);

struct CopySrc { int a, b; };
extern struct CopySrc data_ov006_0213ca14;

struct E { int a, b; };

void func_ov006_020ec84c(char *c)
{
    int i;
    *(int *)(c + 0x40) = 0xa00;
    *(int *)(c + 0x44) = 0xa00;
    for (i = 0; i < 5; i++) {
        ((struct E *)(c + 0x18))[i].a = (int)(((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13) << 8);
        ((struct E *)(c + 0x18))[i].b = (int)(((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13) * 0xc0);
        ((short *)(c + 0x7a))[i] = (short)((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10);
    }
    *(short *)(c + 0x76) = (short)((*(short *)(c + 0x7a) + *(short *)(c + 0x7c)) >> 1);
    func_ov006_020ebeb4(c);
    func_ov006_020ec2bc(c);
    *(short *)(c + 0x78) = 0x1e;
    *(int *)(c + 0x68) = 0;
    *(int *)(c + 0x6c) = 0x1ec;
    *(struct CopySrc *)(c + 0) = data_ov006_0213ca14;
    *(struct CopySrc *)(c + 8) = *(struct CopySrc *)(c + 0);
}
