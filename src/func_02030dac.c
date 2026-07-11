extern int func_0203d890(void);

typedef unsigned char u8;
typedef signed char s8;
extern u8 data_0209fc5c[];
extern s8 data_0209fc64[];
extern u8 data_0209fc50;

void func_02030dac(void)
{
    int n = func_0203d890();
    int i;
    int j;
    for (i = 0; i < n; i++) {
        data_0209fc5c[i] = 1;
        data_0209fc64[i] = (s8)i;
    }
    for (j = n; j < 4; j++) {
        data_0209fc5c[j] = 0;
        data_0209fc64[j] = -1;
    }
    data_0209fc50 = (u8)n;
}
