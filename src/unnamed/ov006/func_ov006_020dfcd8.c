extern void func_ov006_020df024(char *o);

void func_ov006_020dfcd8(char *o)
{
    int *p = (int *)(((int)o + 0x541c));
    *p = *p - 1;
    if (*(int *)(o + 0x5000 + 0x41c) > 0) return;
    func_ov006_020df024(o);
    if (*(unsigned char *)(o + 0x5000 + 0x46b) != 0) {
        unsigned char *q = (unsigned char *)(((int)o + 0x546b));
        *q = *q - 1;
    }
    *(int *)(o + 0x5000 + 0x418) = 2;
}
