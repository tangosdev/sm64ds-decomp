//cpp
extern "C" {
void func_ov006_0211e220(char *c, int idx);
void _ZN5Sound12PlayBank2_2DEj(unsigned int);
void func_ov006_0211f040(char *c, int idx)
{
    int off;
    if (*(int *)(c + 0x4000 + 0xbe8) != 4)
        return;
    off = idx * 0x24;
    if (*(unsigned short *)(c + 0x466e + off) != 0) {
        *(unsigned short *)(c + 0x466e + off) = *(unsigned short *)(c + 0x466e + off) - 1;
        return;
    }
    func_ov006_0211e220(c, idx);
    *(unsigned char *)(c + off + 0x4000 + 0x67a) = 0;
    *(unsigned char *)(c + off + 0x4000 + 0x677) = 0;
    if (*(unsigned char *)(c + 0x4000 + 0xc26) != idx)
        return;
    _ZN5Sound12PlayBank2_2DEj(0x1c7);
}
}
