typedef unsigned char u8;

extern int data_ov006_0212eb70[];

void func_ov006_020fae90(u8 *c)
{
    int i;
    for (i = 0; i < 4; i++, c += 0xc) {
        *(u8 *)(c + 0x5bd4) = 1;
        *(u8 *)(c + 0x5bd5) = 1;
        *(int *)(c + 0x5bcc) = data_ov006_0212eb70[i] << 12;
        *(int *)(c + 0x5bd0) = 0xa0000;
        *(u8 *)(c + 0x5bd6) = 0;
    }
}
