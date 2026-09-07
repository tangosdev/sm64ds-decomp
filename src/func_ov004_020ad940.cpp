//cpp
extern "C" {
int func_ov004_020adafc(void);
int func_ov004_020ad6fc(void* c, int v);
void func_ov004_020ada20(void);
void func_ov004_020ada40(int);
int func_ov004_020adc3c(void *c);
int func_02013580(int a, int b);
void func_02013568(int a, int b, int v);
int func_ov004_020adbc0(void);
extern unsigned char data_0209b300[];

void func_ov004_020ad940(char* c)
{
    int n = func_ov004_020ad6fc(c, func_ov004_020adafc());
    *(int*)(c+0x64) = n;
    if (*(int*)(c+0x64) < 0) {
        func_ov004_020ada20();
        *(int*)(c+0x4000+0x658) = func_ov004_020adafc();
        func_ov004_020ada40(0);
        return;
    }
    int i;
    for (i = 4; i > *(int*)(c+0x64); i--) {
        int a = func_ov004_020adc3c(c);
        int b = func_ov004_020adc3c(c);
        int v = func_02013580(b, i-1);
        func_02013568(a, i, v);
    }
    int a = func_ov004_020adc3c(c);
    int v = func_ov004_020adbc0();
    func_02013568(a, *(int*)(c+0x64), v);
    data_0209b300[0] = 1;
    func_ov004_020ada20();
    *(int*)(c+0x4000+0x658) = func_ov004_020adafc();
    func_ov004_020ada40(0);
}
}
