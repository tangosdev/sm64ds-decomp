struct Vector3 { int x, y, z; };
extern void func_ov064_02117a14(char *c, struct Vector3 *a, struct Vector3 *b);

void func_ov064_02117c24(char *c)
{
    if (*(unsigned char *)(c + 0x33a) != 0) {
        if (*(unsigned char *)(c + 0x33c) != 1) {
            if (*(unsigned short *)(c + 0x338) <= 0x14)
                return;
        }
        *(unsigned char *)(c + 0x33b) = 1;
        struct Vector3 v0, v1;
        v0.x = *(int *)(c + 0x354);
        v0.y = *(int *)(c + 0x358);
        v0.z = *(int *)(c + 0x35c);
        v1.x = *(int *)(c + 0x348);
        v1.y = *(int *)(c + 0x34c);
        v1.z = *(int *)(c + 0x350);
        func_ov064_02117a14(c, &v0, &v1);
        return;
    }
    *(short *)(c + 0x338) = 0;
}
