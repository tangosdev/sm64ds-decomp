struct E { char pad[0x4c]; };

void func_ov002_020f5678(char *base, int i)
{
    struct E *e = (struct E *)base + i;
    *(int *)((char *)e + 0x28) = 0xccc;
    *(unsigned short *)((char *)e + 0x2c) = 0x155;
}
