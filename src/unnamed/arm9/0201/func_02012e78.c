extern signed char data_0208e420;
extern signed char data_0208e42c;
extern signed char data_0209b470;
extern signed char data_0208e424;
extern int data_0209b494;

void func_02012e78(void)
{
    if (data_0208e420 >= 0) return;
    data_0208e420 = data_0208e42c;
    data_0208e424 = data_0209b470;
    data_0208e42c = 0x40;
    data_0209b470 = 0x40;
    data_0209b494 = 0xc999;
}
