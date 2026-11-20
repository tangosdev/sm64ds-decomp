extern volatile int dat_27D7A84[];
extern unsigned char dat_27D8098[];

int fn_0238E158(unsigned char a1) {
    int result;
    dat_27D8098[dat_27D7A84[393]] = a1;
    result = (dat_27D7A84[393] + 1) & 15;
    dat_27D7A84[393] = result;
    return result;
}
