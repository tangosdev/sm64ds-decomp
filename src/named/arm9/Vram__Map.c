void Vram__Map(int mask)
{
    if (mask & 0x1)   *(volatile unsigned char*)0x4000240 = 0x80;
    if (mask & 0x2)   *(volatile unsigned char*)0x4000241 = 0x80;
    if (mask & 0x4)   *(volatile unsigned char*)0x4000242 = 0x80;
    if (mask & 0x8)   *(volatile unsigned char*)0x4000243 = 0x80;
    if (mask & 0x10)  *(volatile unsigned char*)0x4000244 = 0x80;
    if (mask & 0x20)  *(volatile unsigned char*)0x4000245 = 0x80;
    if (mask & 0x40)  *(volatile unsigned char*)0x4000246 = 0x80;
    if (mask & 0x80)  *(volatile unsigned char*)0x4000248 = 0x80;
    if (mask & 0x100) *(volatile unsigned char*)0x4000249 = 0x80;
}
