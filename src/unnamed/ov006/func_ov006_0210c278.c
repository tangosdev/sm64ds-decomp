void func_ov006_0210c278(unsigned char *o)
{
    if (*(int*)(o + 4) <= 0) return;
    int *p = (int*)(((int)o + 4));
    *p = *p - 1;
    if ((*(int*)(o + 4) & 7) == 0) {
        *((unsigned char*)(((int)o + 8))) ^= 1;
    }
}
