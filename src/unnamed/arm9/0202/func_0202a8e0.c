extern int data_0209f3e8[];
extern unsigned char data_0209f370[];

int func_0202a8e0(int a, unsigned char b)
{
    int i;
    for (i = 0; i < 9; i++) {
        if (data_0209f3e8[i] == 0) {
            data_0209f3e8[i] = a;
            data_0209f370[i] = b;
            return i;
        }
    }
    return -1;
}
