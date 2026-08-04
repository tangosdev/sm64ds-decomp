void func_ov004_020af948(void *a, int b, int c, int d);
extern void *data_ov006_02136e2c[];
void func_ov006_02104b5c(char *c) {
    if (*(unsigned char*)(c + 0x4685) == 0) return;
    func_ov004_020af948(data_ov006_02136e2c[0],
                        *(int*)(c + 0x4678) >> 12,
                        *(int*)(c + 0x467c) >> 12, 0);
}
