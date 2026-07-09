extern unsigned char data_0209f250;
extern void *data_0209f394[];
extern signed char data_0209f2f8;
extern int data_0208e434;
extern signed char data_02092120;
extern int data_0209b48c;
extern int data_0208e438;
extern int func_020124c4(int a, int b, int c, int d);

int func_02013078(void)
{
    char *obj = (char *)data_0209f394[data_0209f250];
    int state;
    int flag;
    int result;

    if (obj == 0)
        return -1;
    flag = (*(unsigned char *)(obj + 0x706) != 0);
    state = data_0209f2f8;
    result = -1;
    if (state == 8) {
        result = data_0208e434;
        if (result < 0) {
            if (flag)
                result = 0;
            else
                result = 1;
        }
    } else if (state == 9) {
        result = flag ? 1 : 2;
    } else if (state == 0x12) {
        if (data_02092120 == 0)
            result = 1;
        else
            result = flag ? 0 : 2;
    } else if (state == 0xc) {
        if (data_02092120 == 0)
            data_0209b48c = func_020124c4(data_0209b48c, 3, 0x18d, 0);
        result = (data_02092120 == 0) ? 4 : 3;
    } else if (state == 0xd) {
        result = (*(int *)(obj + 0x60) < (int)0xffe3e000) ? 5 : 6;
    } else if (state == 0x15) {
        result = (*(int *)(obj + 0x60) < 0) ? 5 : 6;
    }
    if (result >= 0 && result != data_0208e438) {
        data_0208e438 = result;
        return result;
    }
    return -1;
}
