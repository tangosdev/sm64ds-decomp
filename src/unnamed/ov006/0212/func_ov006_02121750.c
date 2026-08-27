extern short data_ov006_02140538;
void func_ov006_02121750(char *c, short v)
{
    data_ov006_02140538 = v;
    *(short *)(c + 0x5dba) = v;
}
