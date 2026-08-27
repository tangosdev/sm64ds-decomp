extern short ReadUnalignedShort(unsigned char *p);
extern void func_0200d8ac(char *c, int r1, int r2, int r3);

int func_02007870(void *self, const unsigned char *data) {
    int v1 = (int)ReadUnalignedShort((unsigned char *)data);
    int v2 = (int)ReadUnalignedShort((unsigned char *)data + 2);
    int v3 = (int)ReadUnalignedShort((unsigned char *)data + 4);
    func_0200d8ac((char *)self, v1, v2, v3);
    return 1;
}
