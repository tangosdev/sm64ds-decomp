void func_ov006_020dec5c(char* self, int a, int b)
{
    if (*(unsigned char*)(self + 0x16) != 1)
        return;
    *(int*)self += a;
    *(int*)(((int)self + 4) & 0xFFFFFFFFFFFFFFFFLL) += b;
}
