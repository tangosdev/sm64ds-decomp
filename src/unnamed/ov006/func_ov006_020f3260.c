extern void func_ov006_020f0dd8(void *o);
extern void func_ov006_020f2cb8(void *o);

void func_ov006_020f3260(char *o)
{
    *(short *)(o + 0x5164) = 0;
    func_ov006_020f0dd8(o);
    *(int *)(o + 0x4f78) = 1;
    func_ov006_020f2cb8(o);
}
