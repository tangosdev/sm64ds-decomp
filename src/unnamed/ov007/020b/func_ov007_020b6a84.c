extern void *func_ov007_020c9388(void *self);
extern char *func_ov007_020c023c(void);
extern int func_ov007_020c241c(int a);
extern char *func_ov007_020c2068(void);
extern char *func_ov007_020c2304(void);
extern void *func_ov007_020c908c(void *r5);
extern int func_ov007_020c3df4(int a, void *b);
extern void func_02059eb0(unsigned char *dst);
extern char *data_ov007_0210342c;

void func_ov007_020b6a84(void)
{
    unsigned char buf[0x54];
    int v;

    *(void **)(data_ov007_0210342c + 4) = func_ov007_020c9388((void *)0);
    *(void **)(data_ov007_0210342c + 8) = func_ov007_020c9388((void *)-1);
    *(void **)(data_ov007_0210342c + 0xc) = func_ov007_020c9388((void *)1);
    *(char **)(data_ov007_0210342c + 0x30) = func_ov007_020c023c();
    *(char **)(data_ov007_0210342c + 0x34) = func_ov007_020c023c();
    *(int *)(data_ov007_0210342c + 0x38) = func_ov007_020c241c(0);
    *(int *)(data_ov007_0210342c + 0x3c) = func_ov007_020c241c(1);
    *(char **)(data_ov007_0210342c + 0x50) = func_ov007_020c2068();

    *(char **)(data_ov007_0210342c + 0x54) = func_ov007_020c2304();
    *(void **)(data_ov007_0210342c + 0x6c) = func_ov007_020c908c((void *)(data_ov007_0210342c + 0x64));
    *(void **)(data_ov007_0210342c + 0x70) = func_ov007_020c908c((void *)(data_ov007_0210342c + 0x68));

    *(int *)(data_ov007_0210342c + 0x28) = func_ov007_020c3df4(0, (void *)0x3c);

    func_02059eb0(buf);
    v = buf[0];
    switch (v) {
    case 0:
    case 1:
        *(unsigned char *)(*(int *)(data_ov007_0210342c + 0x28) + 0x3a) = 1;
        break;
    case 2:
        *(unsigned char *)(*(int *)(data_ov007_0210342c + 0x28) + 0x3a) = 2;
        break;
    case 3:
        *(unsigned char *)(*(int *)(data_ov007_0210342c + 0x28) + 0x3a) = 3;
        break;
    case 4:
        *(unsigned char *)(*(int *)(data_ov007_0210342c + 0x28) + 0x3a) = 5;
        break;
    case 5:
        *(unsigned char *)(*(int *)(data_ov007_0210342c + 0x28) + 0x3a) = 4;
        break;
    default:
        *(unsigned char *)(*(int *)(data_ov007_0210342c + 0x28) + 0x3a) = 0;
        break;
    }
    *(int *)(data_ov007_0210342c + 0xe8) = 0;
}
