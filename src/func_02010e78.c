extern int POS_AT_SPAWN;
extern int ROT_AT_SPAWN;
extern unsigned char data_0209b44c;
extern short data_0208e378;
extern int data_0209b458;

void func_02010e78(int a, int b, unsigned char c, short d)
{
    POS_AT_SPAWN = a;
    ROT_AT_SPAWN = b;
    data_0209b44c = c;
    data_0208e378 = d;
    data_0209b458 = 0;
}
