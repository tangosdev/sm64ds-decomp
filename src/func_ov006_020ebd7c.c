extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern int data_ov006_0213c958;
extern short data_ov006_02141fd0;
extern short data_ov006_02141fd4;
extern unsigned short data_ov006_02141fdc[];

void func_ov006_020ebd7c(int count)
{
    int i, j;
    data_ov006_02141fd4 = (short)(RandomIntInternal(&data_0209e650) & 0xf);
    data_ov006_02141fd0 = (short)count;
    for (i = 0; i < count; i++) {
        int r = RandomIntInternal(&data_0209e650);
        data_ov006_02141fdc[i] = (unsigned short)((int)((int)((unsigned int)(r & 0x7fffffff) >> 0x13) * data_ov006_0213c958) >> 0xc);
        j = 0;
        while (j < i) {
            if (data_ov006_02141fdc[j] == data_ov006_02141fdc[i]) {
                r = RandomIntInternal(&data_0209e650);
                data_ov006_02141fdc[i] = (unsigned short)((int)((int)((unsigned int)(r & 0x7fffffff) >> 0x13) * data_ov006_0213c958) >> 0xc);
                j = 0;
            } else {
                j++;
            }
        }
    }
}
