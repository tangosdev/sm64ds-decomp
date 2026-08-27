int func_ov002_020efedc(int* p)
{
    unsigned char v = (*(unsigned int*)((char*)p + 8) >> 8) & 3;
    int r = 1;
    if (v == 1) return r;
    if (v != 2) r = 0;
    return r;
}
