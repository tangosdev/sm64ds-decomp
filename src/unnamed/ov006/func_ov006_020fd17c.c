extern short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void func_02012718(int a, int b);
extern void func_ov006_020fdaf0(char *c, int i);
extern void func_ov006_020fca1c(char *c, int i);
extern void func_ov006_020fc9b0(char *c, int i);
extern void func_ov006_020fcb4c(char *c, int i);

void func_ov006_020fd17c(char *c, int i)
{
    unsigned short t;
    int lim;

    if (*(unsigned short *)(c + i * 0x38 + 0x4688) != 0)
    {
        (*(unsigned short *)(c + 0x4688 + i * 0x38))--;
        if (*(unsigned short *)(c + 0x4688 + i * 0x38) == 0)
        {
            func_02012718(0x185, *(int *)(c + i * 0x38 + 0x4660));
        }
    }

    *(int *)(c + 0x4660 + i * 0x38) += *(int *)(c + 0x4668 + i * 0x38);
    *(int *)(c + 0x4664 + i * 0x38) += *(int *)(c + 0x466c + i * 0x38);

    t = *(unsigned short *)(c + 0x5c28);
    if (t > 12)
    {
        lim = 0xd80 + ((t - 12) << 7);
    }
    else
    {
        lim = 0x600 + t * 0xa0;
    }

    if (*(int *)(c + 0x466c + i * 0x38) <= lim)
    {
        *(int *)(c + 0x466c + i * 0x38) += t * 4 + 8;
    }

    func_ov006_020fdaf0(c, i);
    func_ov006_020fca1c(c, i);
    func_ov006_020fc9b0(c, i);

    *(unsigned short *)(c + 0x4686 + i * 0x38) =
        _ZN4cstd5atan2E5Fix12IiES1_(*(int *)(c + 0x466c + i * 0x38), *(int *)(c + 0x4668 + i * 0x38));
    *(unsigned short *)((unsigned int)c + i * 0x38 + 0x4684) = *(unsigned short *)(c + 0x4686 + i * 0x38) - 0x4000;

    func_ov006_020fcb4c(c, i);
}
