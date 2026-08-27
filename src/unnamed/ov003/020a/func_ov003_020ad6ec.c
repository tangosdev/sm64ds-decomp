extern int data_0209b2f4[];
extern int data_0209b2f8[];
extern int data_020a0d90[];
extern void func_ov003_020adcbc(int a, int b, int c);
extern void func_ov003_020adc74(void* p, int b, int c);

void func_ov003_020ad6ec(void)
{
    int v = data_0209b2f4[0];
    int nv = v - 0xa;
    data_0209b2f8[0] = nv;
    unsigned int t = (unsigned int)(v + 0xb);
    if (t > 0x36) {
        data_0209b2f8[0] = nv - (t - 0x36);
    }
    if (data_0209b2f8[0] < 0) data_0209b2f8[0] = 0;
    int s = data_0209b2f8[0];
    int cnt = 0x36 - s;
    int end = cnt + 2;
    if (end > 0x17) end = 0x17;
    int i;
    int j = s;
    for (i = 2; i < end; i++) {
        func_ov003_020adcbc(j, i, 3);
        j++;
    }
    func_ov003_020adc74(data_020a0d90, 0, 0);
}
