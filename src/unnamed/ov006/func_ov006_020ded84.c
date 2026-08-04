void func_ov006_020ded84(char* t, int a, int b, int c, signed char e)
{
    if (a == 2) {
        *(unsigned char*)(t + 0x16) = 0;
        *(unsigned char*)(t + 0x13) = 1;
    } else {
        *(unsigned char*)(t + 0x16) = (unsigned char)a;
        *(unsigned char*)(t + 0x13) = 0;
    }
    *(signed char*)(t + 0x17) = e;
    *(unsigned char*)(t + 0x15) = 1;
    *(int*)t = b;
    *(int*)(t + 4) = c;
    *(int*)(t + 8) = 0;
    *(int*)(t + 0xc) = 0;
    if (*(unsigned char*)(t + 0x13) != 0)
        *(unsigned char*)(t + 0x12) = 7;
    else
        *(unsigned char*)(t + 0x12) = 5;
    *(unsigned char*)(t + 0x14) = 3;
}
