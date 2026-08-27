#pragma opt_common_subs off
extern void func_ov006_020c862c(void *c, int v);
extern void func_ov006_020c76e0(char *c);
extern int data_ov006_02140418[];
extern char data_ov006_0212dddc[];
extern int data_ov006_02140414[];
extern char *data_ov006_02140420[];
extern int data_ov006_0213b014[];

void func_ov006_020c7490(void)
{
    int i;
    int eoff;
    int ooff;

    i = 0;
    if (data_ov006_02140418[0] <= 0)
        return;
    eoff = 0;
    ooff = 0;
    do {
        char *e = data_ov006_0212dddc + data_ov006_02140414[0] * 0x24 + eoff;
        int word = *(int *)e;
        func_ov006_020c862c(data_ov006_02140420[0] + ooff, (short)word);
        *(int *)(data_ov006_02140420[0] + ooff + 0x14) = *(int *)(e + 4);
        {
            int s = data_ov006_0213b014[0];
            int c = *(int *)(e + 8);
            *(int *)(data_ov006_02140420[0] + ooff + 0x20) =
                (int)(((long long)s * c + 0x800) >> 12);
        }
        func_ov006_020c76e0(data_ov006_02140420[0] + ooff);
        i++;
        eoff += 0xc;
        ooff += 0xb8;
    } while (i < data_ov006_02140418[0]);
}
