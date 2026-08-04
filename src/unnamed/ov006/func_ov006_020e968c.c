extern void func_ov006_020e9374(void *c);
struct E { char pad[0x18]; };
void func_ov006_020e968c(char *c) {
    int i;
    struct E *arr;
    func_ov006_020e9374(c);
    if (*(unsigned char *)(c + 0x554f) == 0) return;
    *(int *)(c + 0x5540) = 3;
    arr = (struct E *)c;
    for (i = 0; i < 5; i++) {
        char *base = (char *)&arr[i];
        if (*(unsigned char *)(base + 0x5218) != 0) {
            if (*(unsigned char *)(base + 0x5219) != 2) {
                *(unsigned char *)(base + 0x5218) = 0;
                *(unsigned char *)(base + 0x521a) = 0;
            }
        }
    }
}
