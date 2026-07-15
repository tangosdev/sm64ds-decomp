struct Vec3 { int x, y, z; };
extern void AddVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);
extern void ApproachLinear(short *a, short b, short c);
extern void func_ov006_020c49d8(void *c);

void func_ov006_020c6088(char *c)
{
    int base_y = *(int*)(c + 0x24);
    int pos_y = *(int*)(c + 0xa0);
    if (pos_y < base_y) {
        int vel = *(int*)(c + 0xac);
        if (vel < 0) {
            *(int*)(c + 0xac) = -vel;
            *(short*)(c + 0xea) = 0;
            goto after_y;
        }
    }
    if (pos_y > base_y + 0x60000) {
        int vel = *(int*)(c + 0xac);
        if (vel > 0) {
            *(int*)(c + 0xac) = -vel;
            *(short*)(c + 0xea) = 0;
        }
    }
after_y:
    {
    int base_x = *(int*)(c + 0x20);
    int pos_x = *(int*)(c + 0x9c);
    if (pos_x < base_x - 0x60000) {
        int vel = *(int*)(c + 0xa8);
        if (vel < 0) {
            *(int*)(c + 0xa8) = -vel;
            *(short*)(c + 0xea) = 0;
            goto after_x;
        }
    }
    if (pos_x > base_x + 0x60000) {
        int vel = *(int*)(c + 0xa8);
        if (vel > 0) {
            *(int*)(c + 0xa8) = -vel;
            *(short*)(c + 0xea) = 0;
        }
    }
after_x:;
    }

    AddVec3((struct Vec3*)(c + 0x9c), (struct Vec3*)(c + 0xa8), (struct Vec3*)(c + 0x9c));
    if (*(int*)(c + 0xa8) > 0)
        ApproachLinear((short*)(c + 0xe6), 0x3000, 0x200);
    else
        ApproachLinear((short*)(c + 0xe6), -0x3000, 0x200);
    func_ov006_020c49d8(c);
}
