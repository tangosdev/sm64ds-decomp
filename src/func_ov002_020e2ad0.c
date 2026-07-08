void func_ov002_020e2ad0(char* p)
{
    if (*(int*)(p + 0x68c) <= 0x1000) return;
    if (*(unsigned short*)(p + 0x6c4) == 0) {
        *(int*)(p + 0xa8) = (int)(((long long)*(int*)(p + 0xa8) * 0x900 + 0x800) >> 12);
        *(int*)(((long long)(int)(p + 0x98)) & 0xFFFFFFFFFFFFFFFFLL) >>= 1;
        *(char*)(p + 0x71b) = 1;
    } else {
        if (*(int*)(p + 0x68c) < 0xb000) *(int*)(p + 0x68c) = 0xb000;
    }
}
