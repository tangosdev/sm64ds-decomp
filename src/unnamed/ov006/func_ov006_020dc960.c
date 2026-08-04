void func_ov006_020dc960(char *c, int i)
{
    unsigned char (*s)[0x18] = (unsigned char (*)[0x18])c;
    unsigned char (*d)[0x10] = (unsigned char (*)[0x10])c;
    *(int *)(&d[i][0x5014]) = *(int *)(&s[i][0x4ac0]);
    *(int *)(&d[i][0x5018]) = *(int *)(&s[i][0x4ac4]);
    *(short *)(&d[i][0x501e]) = *(unsigned char *)(&s[i][0x4ad3]);
    *(unsigned char *)(&d[i][0x5020]) = 1;
    *(short *)(&d[i][0x501c]) = 0x18;
}
