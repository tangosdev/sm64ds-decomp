void func_0204fa2c(void* p, int b);

extern int data_0208e430;
extern int data_0209b490;
extern unsigned char data_0208e42c;
extern int data_0209b494;
extern char data_0209b4b0[];
extern int data_0208e444;
extern int data_0208e440;
extern int data_0209b49c;
extern unsigned char data_0209b470;
extern signed char data_0208e420;

void func_02011b7c(void)
{
    int v = data_0208e430;
    data_0209b490 = 0x7f000;
    data_0208e42c = 0x7f;
    data_0209b494 = 0x7f000;
    if (v >= 0) {
        data_0208e430 = -1;
        func_0204fa2c(data_0209b4b0, 0);
    }
    data_0208e444 = -1;
    data_0208e440 = -1;
    data_0209b49c = 0;
    data_0209b470 = 0;
    data_0208e420 = -1;
}
