void func_ov004_020af948(void *a, int b, int c, int d);
extern void *data_ov006_0213abc8[];
void func_ov006_020f12c8(char *c) {
    if (*(unsigned short*)(c + 0x5164) == 0) return;
    func_ov004_020af948(data_ov006_0213abc8[*(unsigned char*)(c + 0x545a)],
                        *(unsigned short*)(c + 0x5166),
                        *(unsigned short*)(c + 0x5168), 0);
}
