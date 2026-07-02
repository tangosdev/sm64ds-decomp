typedef unsigned short u16;
typedef unsigned int u32;

extern char data_020a1fc0[];
extern int data_020a0f38;
extern char data_020a2400[];

extern int func_02061c20(int *arg);
extern void func_02065a84(int x);
extern void func_020657d0(u16 id, void *obj);
extern void func_02041930(void);
extern void func_02040f30(int arg);

void func_02040bb0(void)
{
    int old;
    data_020a0f38 = 1;
    old = *(int *)data_020a1fc0;
    *(int *)(data_020a1fc0 + 4) = 1;
    if (old == 0) {
        u32 base = (u32)data_020a2400;
        func_02061c20((int *)(base + 0x340));
        func_02065a84(*(u16 *)(base + 0x4c4));
        func_020657d0(1, (void *)func_02040f30);
    } else {
        void (*fn)(int);
        *(int *)(data_020a1fc0 + 0xc) = 2;
        fn = *(void (**)(int))(data_020a1fc0 + 0x10);
        if (fn != 0) {
            fn(0);
        }
    }
    func_02041930();
}
