struct S { char pad[0x24]; };

void func_ov006_02114dd0(struct S *o)
{
    int i;
    for (i = 0; i < 0x40; i++) {
        *(unsigned char*)((char*)o + 0x5000 + 0x9bc) = 0;
        *(int*)((char*)o + 0x5000 + 0x9b8) = 0;
        *(int*)((char*)o + 0x5000 + 0x9b4) = 0;
        o++;
    }
}
