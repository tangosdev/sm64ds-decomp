extern unsigned char data_0209f21c;
extern signed char data_0209f310;
extern signed char data_0209f311;

signed char NumVsStarsObtained(void)
{
    int i = 1;
    signed char sum = data_0209f310;
    signed char *p;
    if (i >= (int)data_0209f21c) return sum;
    p = &data_0209f311;
    do {
        sum = (signed char)(sum + *p);
        i++;
        p++;
    } while (i < (int)data_0209f21c);
    return sum;
}
