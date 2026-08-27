extern void func_ov006_02109530(char* a, char* b, int c);

void func_ov006_0210927c(char* a)
{
    *(short*)(a + 0x2c) = 0x25;
    *(a + 0x32) = 1;
    func_ov006_02109530(a, a + 0x20, 0x100);
}
