unsigned short func_0205402c(unsigned short *r0)
{
    unsigned short flags = *r0;
    *r0 = 0;
    if (flags & 1)     *(volatile unsigned char *)0x4000240 = 0;
    if (flags & 2)     *(volatile unsigned char *)0x4000241 = 0;
    if (flags & 4)     *(volatile unsigned char *)0x4000242 = 0;
    if (flags & 8)     *(volatile unsigned char *)0x4000243 = 0;
    if (flags & 0x10)  *(volatile unsigned char *)0x4000244 = 0;
    if (flags & 0x20)  *(volatile unsigned char *)0x4000245 = 0;
    if (flags & 0x40)  *(volatile unsigned char *)0x4000246 = 0;
    if (flags & 0x80)  *(volatile unsigned char *)0x4000248 = 0;
    if (flags & 0x100) *(volatile unsigned char *)0x4000249 = 0;
    return flags;
}
