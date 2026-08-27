void func_ov006_020e3948(char *p)
{
    int i;
    for (i = 0; i < 60; i++, p += 0x18)
    {
        if (*(unsigned char *)(p + 0x4ff4) != 0)
        {
            if (*(unsigned short *)(p + 0x4ff0) != 0)
            {
                *(unsigned short *)(int)(p + 0x4ff0) -= 1;
                *(int *)(int)(p + 0x4fe4) += *(int *)(p + 0x4fec);
                *(int *)(int)(p + 0x4fec) -= 0x40;
            }
            else
            {
                *(unsigned char *)(p + 0x4ff4) = 0;
                *(unsigned char *)(p + 0x4ff5) = 0;
            }
        }
    }
}
