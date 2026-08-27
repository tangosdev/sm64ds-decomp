extern int func_02055490(int *out);
extern int func_02055464(int *out);

void func_02055624(void)
{
    int a;
    int b;
    *(volatile unsigned int *)0x4000600 |= 0x8000;
    while (func_02055490(&a));
    while (func_02055464(&b));
    *(volatile unsigned int *)0x4000440 = 3;
    *(volatile unsigned int *)0x4000454 = 0;
    *(volatile unsigned int *)0x4000440 = 0;
    if (b != 0) *(volatile unsigned int *)0x4000448 = b;
    *(volatile unsigned int *)0x4000440 = 2;
    *(volatile unsigned int *)0x4000448 = a;
    *(volatile unsigned int *)0x4000454 = 0;
}
