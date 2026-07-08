void func_ov002_020c3cf0(char* self)
{
    *(unsigned char*)(self + 0x6e3) = 0;
    *(unsigned char*)(self + 0x6e5) = *(unsigned char*)(self + 0x6e3);
    (*(unsigned char*)(((int)self + 0x6e6) & 0xFFFFFFFFFFFFFFFFLL))++;
}
