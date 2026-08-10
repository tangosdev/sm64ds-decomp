void func_ov006_020e82c8(unsigned char *c)
{
    unsigned char *r2;
    unsigned char *p;
    unsigned char k;

    r2 = c;
    r2 += 0x5000;
    if (r2[0x553] != 0) {
        return;
    }
    p = (unsigned char *)(c + 0x5553);
    *p = (unsigned char)(*p + 1);
    k = 0x1f;
    r2[0x1f2] = k;
}