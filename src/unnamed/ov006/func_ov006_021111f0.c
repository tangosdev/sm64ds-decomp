void func_ov006_02114738(int *p);
void func_ov006_02110e28(void *p);

void func_ov006_021111f0(short *thiz)
{
    func_ov006_02114738((int*)thiz);
    *(short*)((char*)thiz + 0x32) = 0;
    *(short*)((char*)thiz + 0x34) = 0x40;
    *(short*)((char*)thiz + 0x36) = 0;
    func_ov006_02110e28(thiz);
}
