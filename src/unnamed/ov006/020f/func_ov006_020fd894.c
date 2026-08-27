extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern void func_ov006_020fcd8c(char *o, int i);
extern void func_ov006_020fdaf0(char *o, int i);
extern void func_ov006_020fca1c(char *o, int i);
extern void func_ov006_020fc9b0(char *o, int i);
extern void func_ov006_020fcb4c(char *o, int i);

void func_ov006_020fd894(char *o, int i)
{
    if (*(unsigned char *)(o + 0x4690 + i * 0x38) == 0)
    {
        *(unsigned char *)(o + 0x4690 + i * 0x38) += 1;
        if (*(unsigned short *)(o + 0x5c28) > 0xc)
        {
            *(int *)(o + i * 0x38 + 0x466c) =
                ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 3) >> 0xf) << 7) + 0xb80
                + ((*(unsigned short *)(o + 0x5c28) - 0xc) << 7);
        }
        else
        {
            *(int *)(o + i * 0x38 + 0x466c) =
                *(unsigned short *)(o + 0x5c28) * 0xa0
                + (((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 3) >> 0xf) << 7) + 0x400);
        }
    }
    else
    {
        *(int *)(o + 0x4664 + i * 0x38) += *(int *)(o + i * 0x38 + 0x466c);
    }
    func_ov006_020fcd8c(o, i);
    func_ov006_020fdaf0(o, i);
    func_ov006_020fca1c(o, i);
    func_ov006_020fc9b0(o, i);
    func_ov006_020fcb4c(o, i);
}
