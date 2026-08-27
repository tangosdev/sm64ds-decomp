int func_ov004_020b04c0(void);
void func_ov006_020eef40(void *node);

void func_ov006_020eee3c(int *thiz, int a1, int a2, int a3)
{
    thiz[4/4] = a1;
    thiz[8/4] = a2;
    thiz[0x14/4] = 0;
    thiz[0x18/4] = 0x4000;
    thiz[0xc/4] = a1;
    thiz[0x10/4] = a2 - 0x40000;
    if (thiz[0x10/4] < 0x8000) {
        int t = func_ov004_020b04c0() << 12;
        if (thiz[0x10/4] > -t) {
            thiz[0x10/4] = 0x8000;
            goto strh;
        }
    }
    if (thiz[0x10/4] >= -0xb8000 - (func_ov004_020b04c0() << 12))
        goto strh;
    thiz[0x10/4] = -0xb8000 - (func_ov004_020b04c0() << 12);
strh:
    *(short*)((char*)thiz + 0x1c) = 0x80;
    *(short*)((char*)thiz + 0x20) = 1;
    *(short*)((char*)thiz + 0x1e) = (short)a3;
    func_ov006_020eef40(thiz);
}
