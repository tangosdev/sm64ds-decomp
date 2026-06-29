typedef unsigned char u8;
extern void func_0203da2c(int v);
extern void func_0203da4c(void);
extern u8 data_0209fc5c[];
extern char data_0209fc64[];
extern u8 data_0209fc58[];
extern u8 data_0209fc50[];
extern int data_0209fc68;
extern int data_0209fc60;
extern u8 data_0209fc54;
extern u8 data_0209f1d8;

void func_02030aa4(int arg)
{
    int i;
    func_0203da2c(arg);
    func_0203da4c();
    for (i = 0; i < 4; i++) {
        data_0209fc5c[i] = 0;
        data_0209fc64[i] = -1;
    }
    data_0209fc50[0] = 0;
    data_0209fc58[0] = 0;
    if (arg == 3)
        data_0209fc68 = 1;
    else
        data_0209fc68 = 3;
    data_0209fc60 = 0;
    data_0209fc54 = 0;
    data_0209f1d8 = 1;
}
