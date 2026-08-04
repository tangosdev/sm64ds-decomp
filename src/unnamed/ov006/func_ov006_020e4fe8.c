extern void func_ov004_020af948(int a, int b, int c, int d);
extern int GetGameLanguage(void);
extern void func_ov004_020b2444(int a, int b, int c, int d, int e, int f, int g);
extern int data_ov006_0213c3fc;
extern int *data_ov006_0213c4dc[];
void func_ov006_020e4fe8(char *c)
{
    int v;
    if (*(unsigned char *)(c + 0x5000 + 0x5c3) == 0)
        return;
    v = 5 - *(unsigned char *)(c + 0x5000 + 0x5ba);
    if (v < 0)
        v = 0;
    func_ov004_020af948((int)&data_ov006_0213c3fc, 0xd0, 0xb4, 0);
    func_ov004_020af948(data_ov006_0213c4dc[GetGameLanguage()][1], 0xe8, 0xb4, 0);
    func_ov004_020b2444(0xf8, 0xb4, v, 0, 0, 0, 0);
}
