extern void func_ov006_02114738(void *p);
extern void func_ov006_0210d93c(void *p);
void func_ov006_0210e098(char *c)
{
    func_ov006_02114738(c);
    *(unsigned short *)(c + 0x32) = 0;
    *(int *)(c + 0x34) = 0;
    *(unsigned char *)(c + 0x3c) = 0;
    *(int *)(c + 0x38) = 0;
    *(int *)(c + 0x40) = 0;
    *(unsigned char *)(c + 0x44) = 0;
    func_ov006_0210d93c(c);
}
