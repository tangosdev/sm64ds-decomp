extern unsigned char data_0209fc50;
extern char data_0209fc64[];
extern unsigned char data_0209fc5c[];

void func_020308d0(int n)
{
    int i;
    data_0209fc50 = (unsigned char)n;
    for (i = 0; i < n; i++) {
        data_0209fc64[i] = (char)i;
        data_0209fc5c[i] = 1;
    }
    for (i = n; i < 4; i++) {
        data_0209fc64[i] = -1;
        data_0209fc5c[i] = 0;
    }
}
