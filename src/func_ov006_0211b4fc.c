extern int data_ov006_0212eeb8[];
extern int data_ov006_0212eea0[];
extern unsigned char data_ov006_0212ee0c[];

void func_ov006_0211b4fc(char *c, int i)
{
    unsigned char typeIndex;

    *(unsigned char*)(c + i * 0x24 + 0x5000 + 0x1cd) = 1;

    typeIndex = *(unsigned char*)(c + 0x51d2 + i * 0x24);
    *(int*)(c + 0x51b0 + i * 0x24) += data_ov006_0212eeb8[typeIndex];
    *(int*)(c + i * 0x24 + 0x5000 + 0x1bc) = 0;

    typeIndex = *(unsigned char*)(c + 0x51d2 + i * 0x24);
    *(int*)(c + i * 0x24 + 0x5000 + 0x1b8) = data_ov006_0212eea0[typeIndex];

    typeIndex = *(unsigned char*)(c + 0x51d2 + i * 0x24);
    *(unsigned char*)(c + i * 0x24 + 0x5000 + 0x1d3) = data_ov006_0212ee0c[typeIndex];
    *(unsigned char*)(c + i * 0x24 + 0x5000 + 0x1d1) = 1;
}
