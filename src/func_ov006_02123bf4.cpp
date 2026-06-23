//cpp
struct Player { void St_Null_Init(); };
extern "C" {
extern Player *func_ov006_020cedf0(void *c);
extern int ApproachLinear(int &v, int a, int b);
extern int func_ov006_020ca7b8(void);
extern void func_ov004_020b0a54(void *c);
extern double data_ov006_0213fbe0;
}

extern "C" void func_ov006_02123bf4(char *c)
{
    func_ov006_020cedf0(c)->St_Null_Init();
    if (ApproachLinear(*(int *)(c + 0x7b84), 0, 1) == 0)
        return;
    if (func_ov006_020ca7b8() == 0)
        return;
    func_ov004_020b0a54((void *)0x12);
    *(unsigned char *)(c + 0xc3) = 0;
    *(int *)(c + 0x7b84) = 0xb4;
    *(double *)(c + 0x5004) = data_ov006_0213fbe0;
}
