void func_ov006_0212992c(char *self, int idx)
{
    int off = idx * 0x24;
    int *be94 = (int*)(self + 0xbe94 + off);
    int *be9c = (int*)(self + 0xbe9c + off);
    int *be98 = (int*)(self + 0xbe98 + off);
    *be94 = *be94 + *be9c;
    *be98 = *be98 + *(int*)(self + 0xbea0 + off);
    if (*(unsigned short*)(self + 0xbea8 + off) != 0) {
        unsigned short *p16 = (unsigned short*)(self + 0xbeaa + off);
        *p16 = *p16 - 1;
        if ((short)*p16 < 0) *p16 = 0;
        return;
    }
    if (*be9c > 0) {
        *be9c -= 8;
        if ((short)*be9c < 0) *be9c = 0;
        return;
    }
    if (*be9c < 0) {
        *be9c += 8;
        if (*be9c > 0) *be9c = 0;
        return;
    }
    *(unsigned char*)(self + 0xbeb0 + off) = 0;
}
