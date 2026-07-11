
extern int data_ov006_0212e924[];
void func_ov006_020f6b00(char *this)
{
    int idx;
    char *new_var;
    if ((*((short *) ((this + 0x5300) + 0xe6))) < 2)
    {
        return;
    }
    idx = ((((unsigned short) (*((unsigned short *) ((this + 0x5300) + 0xea)))) << 1) - 1) - (*((short *) ((this + 0x5300) + 0xe8)));
    new_var = this + 0x5300;
    *((unsigned char *) ((this + (idx * 0x18)) + 0x51bb)) = 1;
    {
        short *p = (short *) (((long long)(this + 0x53e8)) & 0xFFFFFFFFFFFFFFFFLL);
        *p = (*p) + 1;
    }
    if ((*((short *) (new_var + 0xe8))) >= data_ov006_0212e924[*((unsigned char *) (this + 0x540a))])
    {
        *((int *) (this + 0x53d8)) = 3;
    }
}
