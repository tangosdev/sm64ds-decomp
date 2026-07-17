typedef unsigned char u8;

typedef struct Vec3 {
    int x, y, z;
} Vec3;

extern void AddVec3(Vec3 *a, Vec3 *b, Vec3 *dst);

void func_ov006_02125890(char *o)
{
    int i;
    char *w = o;
    char *pw = o + 0xba14;
    char *vw = o + 0xba20;
    for (i = 0; i < 0x20; i++, w += 0x24, pw += 0x24, vw += 0x24) {
        if (*(u8 *)(w + 0xba34)) {
            if (*(int *)(w + 0xba2c) > 0) {
                *(int *)(((int)w + 0xba2c) & 0xFFFFFFFFFFFFFFFF) -= 1;
                if (*(int *)(w + 0xba2c) <= 0) {
                    *(u8 *)(o + i * 0x24 + 0xba34) = 0;
                    return;
                }
            }
            *(int *)(((int)w + 0xba28) & 0xFFFFFFFFFFFFFFFF) -= 0x200;
            AddVec3((Vec3 *)pw, (Vec3 *)vw, (Vec3 *)pw);
            if (*(int *)(w + 0xba28) < 0 && *(int *)(w + 0xba1c) < 0) {
                *(u8 *)(o + i * 0x24 + 0xba34) = 0;
                return;
            }
        }
    }
}
