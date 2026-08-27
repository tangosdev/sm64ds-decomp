extern void func_ov004_020b0380(void *fn, int a, int b, int d);
extern void *data_ov006_02136e24[];
struct E { char pad[0x18]; };
void func_ov006_020dc99c(char *c) {
    int i;
    struct E *arr = (struct E *)c;
    for (i = 0; i < 0x20; i++) {
        char *base = (char *)&arr[i];
        if (*(unsigned char *)(base + 0x4d29) != 0) {
            int k = *(unsigned char *)(base + 0x4d2a);
            int a = *(int *)(base + 0x4d14);
            int b = *(int *)(base + 0x4d18);
            func_ov004_020b0380(data_ov006_02136e24[k], a >> 0xc, b >> 0xc, 0);
        }
    }
}
