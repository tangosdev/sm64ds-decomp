extern void func_ov006_02114738(void *p);

void func_ov006_02111b20(char *p)
{
    func_ov006_02114738(p);
    *(unsigned char *)(p + 0x38) = 0;
    *(int *)(p + 0x3c) = 0;
}
