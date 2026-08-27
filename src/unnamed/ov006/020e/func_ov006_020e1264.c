#pragma opt_propagation off
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8[];
extern int data_ov006_0212e460[];
void func_ov006_020e1264(char *c, int idx) {
    unsigned r = (unsigned)RandomIntInternal(data_0209d4b8);
    int k = 0;
    unsigned m = ((r >> 16) & 0x7fff) << 3 >> 0xf;
    if (m == 5) k = 1;
    char *p = c + idx * 0x24;
    *(unsigned char *)(p + 0x47a9) = data_ov006_0212e460[k];
    *(unsigned char *)(p + 0x47aa) = 0;
}
