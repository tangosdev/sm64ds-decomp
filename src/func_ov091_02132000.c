struct Vector3 { int x, y, z; };
extern void func_ov091_02131cb0(void *c, const struct Vector3 *v0, const struct Vector3 *v1);

void func_ov091_02132000(char *c)
{
    if (*(unsigned char *)(c + 0x356) != 0) {
        if (*(unsigned short *)(c + 0x354) <= 0x14)
            return;
        *(int *)(c + 0x320) = 1;
        *(int *)(c + 0x98) = 0xa000;
        struct Vector3 v0, v1;
        v0.x = *(int *)(c + 0x338);
        v0.y = *(int *)(c + 0x33c);
        v0.z = *(int *)(c + 0x340);
        v1.x = *(int *)(c + 0x32c);
        v1.y = *(int *)(c + 0x330);
        v1.z = *(int *)(c + 0x334);
        func_ov091_02131cb0(c, &v0, &v1);
        return;
    }
    *(short *)(c + 0x354) = 0;
}
