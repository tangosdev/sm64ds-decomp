void func_02012790(int a);
void func_ov006_021048e4(unsigned char* r0);
void func_ov006_02105c1c(char* c);

void func_ov006_02105134(char* c)
{
    if (*(unsigned char*)(c + 0x4fe9) != 0) return;
    if (*(int*)(c + 0x4ca8) == 6) return;
    if (*(unsigned char*)(c + 0x4fe1) < *(unsigned char*)(c + 0x4fde)) return;

    func_02012790(0xe);
    func_ov006_021048e4((unsigned char*)c);

    if (*(unsigned char*)(c + 0x4fe2) > 1) {
        *(int*)(c + 0x4ca8) = 3;
        func_ov006_02105c1c(c);
    } else {
        *(unsigned char*)(c + 0x4fe6) = 0;
        func_ov006_02105c1c(c);
        *(int*)(c + 0x4ca8) = 6;
        *(short*)(c + 0x4ec0) = 0x50;
    }
}
