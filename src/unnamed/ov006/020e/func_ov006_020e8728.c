extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern unsigned char data_ov006_0212e57c[];

#pragma opt_common_subs off
void func_ov006_020e8728(char *c, int idx)
{
    unsigned int r;
    *(int *)(c + (idx << 5) + 0x52c4) = 0;
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(unsigned char *)(c + (idx << 5) + 0x52d5) = (unsigned char)((r << 3) >> 15);
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(unsigned char *)(c + (idx << 5) + 0x52d6) = (unsigned char)((r << 3) >> 15);
    *(unsigned char *)(c + (idx << 5) + 0x52d8) = 1;
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(unsigned short *)(c + (idx << 5) + 0x52ce) = (unsigned char)(((r << 4) >> 15) + 8);
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(unsigned short *)(c + (idx << 5) + 0x52d0) = (unsigned char)(((r << 5) >> 15) + 0x10);
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(unsigned char *)(c + (idx << 5) + 0x52da) = data_ov006_0212e57c[(r << 1) >> 15];
}
