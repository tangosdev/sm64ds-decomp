extern void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);

void func_ov006_0211d4e8(char* c, int i) {
    int off = i * 0x1c;
    char* e = c + off;
    int* a = (int*)(c + 0x4bd0);
    int* b = (int*)(c + 0x4bd8);
    *(unsigned char*)(e + 0x4000 + 0xbe2) = 1;
    *(int*)((char*)a + off) = *(int*)((char*)a + off) + *(int*)((char*)b + off);
    *(int*)((char*)b + off) = *(int*)((char*)b + off) - 0x100;
    if (*(int*)((char*)a + off) >> 0xc > 0xc0)
        return;
    *(int*)((char*)a + off) = 0xc0000;
    *(int*)((char*)b + off) = 0;
    *(unsigned char*)(e + 0x4000 + 0xbe1) = 1;
    *(short*)(e + 0x4b00 + 0xde) = 0x30;
    func_ov004_020b0cac(0xc, 0x80, 0x40, 1, -1, 0xd);
}
