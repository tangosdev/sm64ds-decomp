extern void func_ov006_020fb8fc(char *c, int a2, int a3, int a4, int a5, int a6);
extern void func_02012718(void *a, int b);
extern unsigned short data_ov006_0213d954;

void func_ov006_020fce04(char *c, int i)
{
    int k = i * 0x38;
    unsigned short *cnt = (unsigned short *)(c + 0x4688 + k);
    if (*cnt != 0) {
        *cnt = *cnt - 1;
        if (*cnt != 0)
            return;
        func_ov006_020fb8fc(c,
                            *(int *)(c + 0x4660 + k),
                            *(int *)(c + k + 0x4000 + 0x664),
                            2,
                            data_ov006_0213d954,
                            0);
        func_02012718((void *)0x18b, *(int *)(c + 0x4660 + k));
        return;
    }
    *(unsigned char *)((c + k) + 0x4000 + 0x68c) = 0;
    *(unsigned char *)((c + k) + 0x4000 + 0x68d) = 0;
}
