//cpp
extern int ApproachLinear(int &, int, int);
extern "C" {
void func_0203d388(int *p, int angle);
void func_ov006_020eb9b0(char *p);
int func_02012468(int a, int b, int c, int d, int e, int f, int g, int h);
void func_ov006_020ebf20(char *p);
}

#define L(a) (*(int*)(((long long)(int)(a))))
#define M(a) (*(int*)(((long long)(unsigned)(a))))

extern "C" void func_ov006_020eb610(char *c)
{
    int v[2];
    int step;
    int v93;

    if (*(unsigned char *)(c + 0x92) != 0) {
        step = 0x1000;
        if (*(int *)(c + 0x40) < 0xa00) step = step >> 2;
        ApproachLinear(*(int *)(c + 0x40), *(int *)(c + 0x44), step);
        v[1] = -*(int *)(c + 0x40);
        v[0] = 0;
        func_0203d388(v, *(short *)(c + 0x7c));
        L(c + 0x20) += v[0];
        M(c + 0x24) += v[1];
        if (*(unsigned char *)(c + 0x93) != 0) {
            *(short *)(long long)(int)(c + 0x8e) = *(short *)(long long)(int)(c + 0x8e) - 1;
            if (*(short *)(c + 0x8e) == 0) {
                func_ov006_020eb9b0(c);
            }
        }
    }
    if (*(int *)(c + 0x6c) != 0 && *(int *)(c + 0x70) == 0) {
        v93 = 0;
        if (*(unsigned char *)(c + 0x93) != 0) v93 = 0x1f4;
        *(int *)(c + 0x68) = func_02012468(*(int *)(c + 0x68), 2,
            *(int *)(c + 0x6c), 2, 0, v93, 0, 0);
    } else {
        *(int *)(c + 0x68) = 0;
    }
    if (*(int *)c != 0) {
        int off = *(int *)(c + 4);
        char *base = c + (off >> 1);
        void (*fn)(char *);
        if (off & 1)
            fn = *(void (**)(char *))(*(char **)base + *(int *)c);
        else
            fn = *(void (**)(char *))c;
        fn(base);
    }
    if (*(unsigned char *)(c + 0x92) == 0) return;
    func_ov006_020ebf20(c);
}
