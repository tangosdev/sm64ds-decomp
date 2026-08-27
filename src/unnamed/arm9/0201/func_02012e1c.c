extern signed char data_0208e420;
extern signed char data_0208e424;
extern signed char data_0208e42c;
extern signed char data_0209b470;

void func_02012e1c(void)
{
    signed char v = data_0208e420;
    if (v < 0) return;
    data_0208e42c = v;
    data_0209b470 = data_0208e424;
    data_0208e420 = -1;
}
