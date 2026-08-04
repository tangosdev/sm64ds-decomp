extern int data_ov006_0213eb40;
extern int data_ov006_0213e5d4;
extern int func_020ad494;
extern void func_ov004_020b29c0(void *c);
void *func_ov006_0210a8c0(char *c) {
    *(int*)c = (int)&data_ov006_0213eb40;
    *(int*)(c + 0x4660) = (int)&data_ov006_0213e5d4;
    *(int*)(c + 0x4660) = (int)&func_020ad494;
    func_ov004_020b29c0(c);
    return c;
}
