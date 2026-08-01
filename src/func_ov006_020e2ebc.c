int func_ov004_020adbe0(void);
void func_ov004_020b0a54(void* c);

void func_ov006_020e2ebc(char *thiz)
{
    int i;
    char *p;
    if (*(unsigned short*)(thiz + 0x4ee2) != 0) {
        *(unsigned short*)(((int)thiz + 0x4ee2)) -= 1;
        if (*(short*)(thiz + 0x4ee2) <= 0)
            *(unsigned short*)(thiz + 0x4ee2) = 0;
        return;
    }
    if (func_ov004_020adbe0() != 0) {
        *(unsigned char*)(thiz + 0x4000 + 0xee8) = 0;
        *(int*)(thiz + 0x4000 + 0xeac) = 4;
        func_ov004_020b0a54((void*)0x10);
    } else {
        *(int*)(thiz + 0x4000 + 0xeac) = 4;
        func_ov004_020b0a54((void*)0x10);
    }
    *(unsigned char*)(thiz + 0xc3) = 0;
    *(unsigned char*)(thiz + 0x4000 + 0xee5) = 0;
    p = thiz;
    for (i = 0; i < 5; i++) {
        *(unsigned char*)(p + 0x4000 + 0x68a) = 0;
        p += 0x2c;
    }
}
