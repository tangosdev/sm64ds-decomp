#define A(p) ((int)(((long long)(int)(p))))
extern unsigned short data_ov006_0212ee38[];

void func_ov006_02119a18(char *b)
{
    if (*(unsigned char *)(b + 0x5604) == 0) return;
    if (*(unsigned char *)(b + 0x5606) >= 3) return;
    *(unsigned short *)A(b + 0x5600) += 1;
    if (*(unsigned short *)(b + 0x5600) < data_ov006_0212ee38[*(unsigned char *)(b + 0x5606)]) return;
    *(unsigned short *)(b + 0x5600) = 0;
    *(unsigned char *)A(b + 0x5606) += 1;
}
