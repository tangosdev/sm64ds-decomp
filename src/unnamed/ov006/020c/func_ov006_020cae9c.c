extern int func_ov006_020ca430(int p);
extern int data_ov006_021405bc;
extern int data_ov006_021405b8;
extern int data_ov006_02140554;
extern int data_ov006_0214097c[];
extern int data_ov006_02140968[];

int func_ov006_020cae9c(int base, int n)
{
    int i;
    int off;

    data_ov006_021405bc = n;
    data_ov006_021405b8 = n;
    data_ov006_02140554 = base;

    i = 0;
    if (i < n) {
        off = i;
        do {
            int p;
            if (func_ov006_020ca430(data_ov006_02140554 + off) == 0) return 0;
            p = data_ov006_02140554 + off;
            if (i < 5) data_ov006_0214097c[i] = p;
            if (i < 5) data_ov006_02140968[i] = p;
            off = off + 0xdc;
            n = data_ov006_021405bc;
            i++;
        } while (i < n);
    }
    return 1;
}
