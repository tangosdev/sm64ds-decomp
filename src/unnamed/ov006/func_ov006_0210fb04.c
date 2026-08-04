extern void _ZN5Sound12PlayBank2_2DEj(unsigned int a);

void func_ov006_0210fb04(char* self)
{
    if (*(unsigned char*)(self + 0x73) != 0) {
        ++*(int*)(((int)self + 0x78));
        return;
    }
    *(unsigned char*)(self + 0x74) = 1;
    *(unsigned char*)(self + 0x73) = 1;
    *(int*)(self + 0x80) = 0xb4;
    _ZN5Sound12PlayBank2_2DEj(0x160);
}
