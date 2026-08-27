extern unsigned char data_0209f21c;
extern signed char data_0209f30c;
extern signed char data_0209f30d;

signed char NumRedCoins(void)
{
    int i = 1;
    signed char sum = data_0209f30c;
    signed char *p;
    if (i >= (int)data_0209f21c) return sum;
    p = &data_0209f30d;
    do {
        sum = (signed char)(sum + *p);
        i++;
        p++;
    } while (i < (int)data_0209f21c);
    return sum;
}
