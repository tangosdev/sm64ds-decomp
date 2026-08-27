extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern unsigned char data_ov006_0212e588[];
extern int volatile data_ov006_0213c818[];

void func_ov006_020e93e8(char *c)
{
    int a[5];
    int b[5];
    int n;
    int i;
    int k;
    int l;
    int m;
    int j;
    int v;
    int o;
    char *p;

    n = data_ov006_0212e588[*(int *)(c + 0x5544)];
    *(unsigned char *)(c + 0x5551) =
        (unsigned char)(((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff) * 5 >> 15);
    for (i = 0; i < 5; i++) {
        a[i] = 0;
        b[i] = 0;
    }
    for (i = 0; i < 5; i++) {
        do {
            k = (int)(((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff) * 5 >> 15);
        } while (b[k] != 0);
        b[k] = 1;
        a[i] = k & 0xff;
    }
    m = (int)((n * ((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff)) >> 15);
    v = *(unsigned char *)(c + 0x5551);
    for (j = 0; j < 5; j++) {
        if (v == a[j]) {
            a[j] = a[m];
            a[m] = v;
            break;
        }
    }
    l = 0;
    if (l < n) {
        o = 0;
        p = c;
        do {
            *(int *)(p + 0x5208) = ((unsigned short *)data_ov006_0213c818[n - 2])[o] << 12;
            *(int *)(p + 0x520c) = ((unsigned short *)data_ov006_0213c818[n - 2])[l * 2 + 1] << 12;
            *(unsigned char *)(p + 0x5218) = 1;
            *(unsigned char *)(p + 0x521a) = 1;
            *(unsigned char *)(p + 0x521c) = 1;
            *(unsigned char *)(p + 0x521b) = (unsigned char)a[l];
            *(unsigned char *)(p + 0x5219) = 0;
            o += 2;
            p += 0x18;
            l++;
        } while (l < n);
    }
    *(short *)(c + 0x51f0) = *(unsigned char *)(c + 0x5551);
}
