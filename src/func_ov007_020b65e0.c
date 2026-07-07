extern void func_ov007_020c937c(int a);
extern void func_ov007_020c020c(void *a);
extern void func_ov007_020c2410(int a);
extern void func_ov007_020c2018(int a);
extern void func_ov007_020c22f8(int a);
extern void func_ov007_020c9080(int a);
extern void _ZN6Player17St_EndingFly_MainEv(void *p);
extern void func_ov007_020c1620(int a);
extern void func_ov007_020b3edc(int a);
extern void func_ov007_020c9460(int a);
extern int func_ov007_020b4bd4(int *a);
extern void func_ov007_020b6cd0(void);
extern void func_ov007_020b283c(void);
extern void func_ov007_020b9770(void);
extern void func_ov007_020bf304(void);
extern char *data_ov007_0210342c;

#pragma opt_strength_reduction off
#pragma opt_common_subs off
void func_ov007_020b65e0(void)
{
    int i;

    if (*(int *)(data_ov007_0210342c + 4) != 0) {
        func_ov007_020c937c(*(int *)(data_ov007_0210342c + 4));
        *(int *)(data_ov007_0210342c + 4) = 0;
    }
    if (*(int *)(data_ov007_0210342c + 8) != 0) {
        func_ov007_020c937c(*(int *)(data_ov007_0210342c + 8));
        *(int *)(data_ov007_0210342c + 8) = 0;
    }
    if (*(int *)(data_ov007_0210342c + 0xc) != 0) {
        func_ov007_020c937c(*(int *)(data_ov007_0210342c + 0xc));
        *(int *)(data_ov007_0210342c + 0xc) = 0;
    }
    if (*(int *)(data_ov007_0210342c + 0x30) != 0) {
        func_ov007_020c020c((void *)*(int *)(data_ov007_0210342c + 0x30));
        *(int *)(data_ov007_0210342c + 0x30) = 0;
    }
    if (*(int *)(data_ov007_0210342c + 0x34) != 0) {
        func_ov007_020c020c((void *)*(int *)(data_ov007_0210342c + 0x34));
        *(int *)(data_ov007_0210342c + 0x34) = 0;
    }
    if (*(int *)(data_ov007_0210342c + 0x38) != 0) {
        func_ov007_020c2410(*(int *)(data_ov007_0210342c + 0x38));
        *(int *)(data_ov007_0210342c + 0x38) = 0;
    }
    if (*(int *)(data_ov007_0210342c + 0x3c) != 0) {
        func_ov007_020c2410(*(int *)(data_ov007_0210342c + 0x3c));
        *(int *)(data_ov007_0210342c + 0x3c) = 0;
    }
    if (*(int *)(data_ov007_0210342c + 0x50) != 0) {
        func_ov007_020c2018(*(int *)(data_ov007_0210342c + 0x50));
        *(int *)(data_ov007_0210342c + 0x50) = 0;
    }
    if (*(int *)(data_ov007_0210342c + 0x54) != 0) {
        func_ov007_020c22f8(*(int *)(data_ov007_0210342c + 0x54));
        *(int *)(data_ov007_0210342c + 0x54) = 0;
    }
    if (*(int *)(data_ov007_0210342c + 0x6c) != 0) {
        func_ov007_020c9080(*(int *)(data_ov007_0210342c + 0x6c));
        *(int *)(data_ov007_0210342c + 0x6c) = 0;
    }
    if (*(int *)(data_ov007_0210342c + 0x70) != 0) {
        func_ov007_020c9080(*(int *)(data_ov007_0210342c + 0x70));
        *(int *)(data_ov007_0210342c + 0x70) = 0;
    }
    if (*(int *)(data_ov007_0210342c + 0x28) != 0) {
        _ZN6Player17St_EndingFly_MainEv((void *)*(int *)(data_ov007_0210342c + 0x28));
        *(int *)(data_ov007_0210342c + 0x28) = 0;
    }

    for (i = 0; i < 0xb; i++) {
        if (*(int *)(data_ov007_0210342c + i * 4 + 0x78) != 0) {
            func_ov007_020c1620(*(int *)(data_ov007_0210342c + i * 4 + 0x78));
            *(int *)(data_ov007_0210342c + i * 4 + 0x78) = 0;
        }
    }

    func_ov007_020b3edc(0x16);

    if (*(int *)(data_ov007_0210342c + 0x40) != 0) {
        func_ov007_020c937c(*(int *)(data_ov007_0210342c + 0x40));
        *(int *)(data_ov007_0210342c + 0x40) = 0;
    }

    for (i = 0; i < 3; i++) {
        if (*(int *)(data_ov007_0210342c + i * 4 + 0x44) != 0) {
            func_ov007_020c9460(*(int *)(data_ov007_0210342c + i * 4 + 0x44));
            *(int *)(data_ov007_0210342c + i * 4 + 0x44) = 0;
        }
    }

    for (i = 0; i < 0x18; i++) {
        if (*(int *)(data_ov007_0210342c + i * 4 + 0x114) != 0) {
            func_ov007_020b4bd4((int *)*(int *)(data_ov007_0210342c + i * 4 + 0x114));
            *(int *)(data_ov007_0210342c + i * 4 + 0x114) = 0;
        }
    }

    func_ov007_020b6cd0();
    func_ov007_020b283c();
    func_ov007_020b9770();
    func_ov007_020bf304();

    if (data_ov007_0210342c == 0) {
        return;
    }

    _ZN6Player17St_EndingFly_MainEv((void *)data_ov007_0210342c);
    data_ov007_0210342c = 0;
}
