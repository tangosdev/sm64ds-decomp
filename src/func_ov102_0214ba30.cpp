//cpp
extern "C" {
int _ZN5Actor13DistToCPlayerEv(void* p);
void func_ov102_0214c0b8(void* p);
}
extern unsigned char data_0209f2d8;

extern "C" void func_ov102_0214ba30(char* c)
{
    if (_ZN5Actor13DistToCPlayerEv(c) < 0x7d0000)
        return;
    if ((*(int*)(c + 0xb0) & 8) == 0) {
        int b = (data_0209f2d8 == 1);
        if (b == 0)
            return;
    }
    func_ov102_0214c0b8(c);
    {
        int* f = (int*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF);
        *f = *f | 0x10000001;
    }
    *(unsigned char*)(c + 0x3f3) = 1;
}
