extern unsigned char data_020a9d2c[];
extern void func_02068484(void);

void func_020684a8(void)
{
    *(int *)&data_020a9d2c[0] = 0;
    *(int *)&data_020a9d2c[4] = 0;
    data_020a9d2c[0xc] = 1;
    func_02068484();
}
