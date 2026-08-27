extern unsigned short data_ov006_0212e58c[];
void func_ov006_020e8d7c(char *c, int idx)
{
    int i;
    char *d = c;
    char *e = c + idx * 0x18;
    char *src1 = e + 0x5208;
    char *src2 = e + 0x520c;
    for (i = 0; i < 3; i++) {
        *(unsigned char *)(d + 0x5290) = 1;
        *(int *)(d + 0x5280) = *(int *)src1;
        *(int *)(d + 0x5284) = *(int *)src2;
        *(unsigned char *)(d + 0x5292) = 0;
        *(unsigned char *)(d + 0x5291) = 0;
        *(short *)(d + 0x528c) = data_ov006_0212e58c[i];
        *(unsigned char *)(d + 0x5293) = 0;
        *(int *)(d + 0x5288) = 0x1400;
        d += 0x14;
    }
}
