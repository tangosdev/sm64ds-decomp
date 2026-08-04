extern void func_ov004_020b1e34(void *thiz, int a, int b, int c);

void func_ov006_020f5620(char *thiz)
{
    if (*(unsigned char *)(thiz + 0x540b) == 0) return;
    func_ov004_020b1e34(thiz, 0xe0, 0x14, 1);
}
