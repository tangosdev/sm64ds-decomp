extern int RandomIntInternal(int *seed);
extern int func_ov006_020ebc7c(int a, int b);
extern void func_ov006_020ecba4(void *self, int arg);
extern void func_ov006_020ec9c0(void *self, int arg);
extern void func_ov006_020ec84c(void *self, int arg);
extern void func_ov006_020ec4dc(void *self, int arg);

extern int data_0209e650;
extern short data_ov006_02141fd8;

struct Obj {
    char pad[0x4c];
    int arr[5];
};

void func_ov006_020ecdb8(char *self, int arg1, int arg2)
{
    int i;
    struct Obj *o = (struct Obj *)self;

    *(int *)(self + 0x68) = 0;
    *(int *)(self + 0x6c) = 0;
    *(int *)(self + 0x70) = 1;
    *(int *)(self + 0x64) = arg1;
    *(short *)(self + 0x8c) = 0;
    *(short *)(self + 0x84) = (short)(RandomIntInternal(&data_0209e650) & 0xfff0);
    *(short *)(self + 0x86) = 0x600;
    *(unsigned char *)(self + 0x93) = 0;
    *(unsigned char *)(self + 0x94) = 0;
    *(unsigned char *)(self + 0x95) = 2;
    *(int *)(self + 0x88) = arg2;
    *(unsigned char *)(self + 0x92) = 1;

    for (i = 0; i < 5; i++)
        o->arr[i] = 0x10000;

    *(short *)(self + 0x74) = (short)func_ov006_020ebc7c(arg1, 0xff);

    switch (data_ov006_02141fd8) {
    case 0:
        func_ov006_020ecba4(self, arg1);
        return;
    case 1:
        func_ov006_020ec9c0(self, arg1);
        return;
    case 2:
        func_ov006_020ec84c(self, arg1);
        return;
    case 3:
    default:
        func_ov006_020ec4dc(self, arg1);
        return;
    }
}
