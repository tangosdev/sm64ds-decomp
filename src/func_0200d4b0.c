extern unsigned char data_0209f250;
extern void func_0200ee8c(int arg0);

void func_0200d4b0(char* self, unsigned int playerID, int arg2)
{
    unsigned char tmp = data_0209f250;
    if (playerID == tmp)
    {
        volatile int dummy;
        char* other;
        *(short*)(self + 0x19a) = *(short*)(self + 0x17c);
        other = *(char**)(self + 0x110);
        *(int*)(((int)other + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) |= 0x20000000;
        func_0200ee8c(arg2);
    }
}
