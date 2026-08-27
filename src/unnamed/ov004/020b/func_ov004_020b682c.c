extern int func_ov004_020b04c0(void);
extern int data_ov004_020bc88c;
extern int data_ov004_020bc860;
extern int data_ov004_020bc878;
extern int data_ov004_020bc890;
extern int data_ov004_020bc864;
extern int data_ov004_020bc858;
extern int data_ov004_020bc868;
extern int data_ov004_020bc888;
extern int data_ov004_020bc8b8;
extern int data_ov004_020bc8b4;

void func_ov004_020b682c(void)
{
    int v1 = -0x1c - func_ov004_020b04c0();
    data_ov004_020bc88c = 0x80;
    data_ov004_020bc860 = v1;
    {
        int v2 = -0x1c - func_ov004_020b04c0();
        data_ov004_020bc878 = 0x80;
        data_ov004_020bc890 = v2;
    }
    {
        int v3 = -0x1c - func_ov004_020b04c0();
        data_ov004_020bc868 = 0x80;
        data_ov004_020bc858 = v3;
        data_ov004_020bc888 = 0x80;
        data_ov004_020bc864 = -0x1c;
        data_ov004_020bc8b8 = 0x80;
        data_ov004_020bc8b4 = 0x60;
    }
}
