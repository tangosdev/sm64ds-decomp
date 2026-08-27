extern int RandomIntInternal(int *seed);
extern int data_ov006_0212e900[];
extern int data_ov006_0212e90c[];
extern int data_0209d4b8;

void func_ov006_020f6f88(char *obj)
{
    int idx = *(unsigned char *)(obj + 0x5000 + 0x40a);
    int n = data_ov006_0212e900[idx];
    int mul = data_ov006_0212e90c[idx];
    int i;
    int zero = 0;
    for (i = 0; i < n; i++) {
        int v;
        v = (mul * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 15;
        if (i != 0) {
            int found;
            int j;
            do {
                found = zero;
                for (j = zero; j < i; j++) {
                    if (v == *(unsigned char *)(obj + j + 0x5000 + 0x3fd)) {
                        found++;
                        break;
                    }
                }
                if (found == 0) break;
                v = (mul * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 15;
            } while (1);
        }
        *(unsigned char *)(obj + i + 0x5000 + 0x3fd) = (unsigned char)v;
    }
}
