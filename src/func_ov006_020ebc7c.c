extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern short data_ov006_02141fd0;
extern unsigned short data_ov006_02141fd4;
extern unsigned short data_ov006_02141fdc[];
extern unsigned short data_ov006_02141ff8[];

int func_ov006_020ebc7c(int n, int mask)
{
    int v;
    int i;
    int found = 0;

    for (i = 0; i < data_ov006_02141fd0; i++) {
        if (n == data_ov006_02141fdc[i]) {
            found = 1;
            break;
        }
    }

    v = (unsigned short)(mask & RandomIntInternal(&data_0209e650));
    if (found) v = data_ov006_02141fd4 | (unsigned short)(v & 0xf0);

    for (i = 0; i < n; i++) {
        if (v == data_ov006_02141ff8[i]) {
            v = (unsigned short)(mask & RandomIntInternal(&data_0209e650));
            if (found) v = data_ov006_02141fd4 | (unsigned short)(v & 0xf0);
            i = -1;
        }
    }

    data_ov006_02141ff8[n] = v;
    return v;
}
