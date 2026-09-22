extern int data_ov062_0211deb0[];
extern int Chuckya_ChangeState(unsigned char *c, void *p);
int func_ov062_02116274(unsigned char *c)
{
    short *p94 = (short *)((int)c + 0x94);
    short val = *p94;
    unsigned short *p100 = (unsigned short *)((int)c + 0x100);
    val += 0x500;
    *p94 = val;
    if (*p100 == 0)
    {
        Chuckya_ChangeState(c, data_ov062_0211deb0);
    }
    return 1;
}
