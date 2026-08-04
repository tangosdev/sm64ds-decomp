typedef struct { int x, y, z; } Vec3;

struct Obj;

extern void AddVec3(Vec3 *a, Vec3 *b, Vec3 *c);
extern int LenVec3(Vec3 *v);
extern void _Z15ApproachLinear2Rsss(short *v, short t, short s);
extern int func_ov006_020e6e3c(int a, int b);
extern void func_ov006_020c63dc(char *p);
extern void func_ov006_020c6348(int *c);
extern void func_ov006_020c6248(int *c);
extern void func_ov006_020c6188(void *c);
extern void func_ov006_020c5ec8(char *self);
extern void func_ov006_020c5cf4(int *c);
extern void func_ov006_020c5aa4(struct Obj *o);
extern void func_ov006_020c57d4(struct Obj *o);
extern void func_ov006_020c561c(void *c);
extern void func_ov006_020c54f4(void *c);
extern void func_ov006_020c5370(char *c);
extern void func_ov006_020c49d8(void *c);
extern void func_ov006_020c47d4(void *c);

void func_ov006_020c66bc(char *c)
{
    int dx, dy, dz;
    int vx, vy;

    *(short *)(((int)c + 0xea)) += 0x200;

    dx = *(int *)(c + 0x20) - *(int *)(c + 0x9c);
    *(int *)(c + 0xa8) = (int)((((long long)dx << 8) + 0x800) >> 12);
    dy = *(int *)(c + 0x24) - *(int *)(c + 0xa0);
    *(int *)(c + 0xac) = (int)((((long long)dy << 8) + 0x800) >> 12);
    dz = *(int *)(c + 0x28) - *(int *)(c + 0xa4);
    *(int *)(c + 0xb0) = (int)((((long long)dz << 8) + 0x800) >> 12);

    AddVec3((Vec3 *)(c + 0x9c), (Vec3 *)(c + 0xa8), (Vec3 *)(c + 0x9c));

    vx = *(int *)(c + 0xa8);
    if (vx < 0) vx = -vx;
    if (vx < 0x18) {
        vy = *(int *)(c + 0xac);
        if (vy < 0) vy = -vy;
        if (vy < 0x18) {
            *(int *)(c + 0x9c) = *(int *)(c + 0x20);
            *(int *)(c + 0xa0) = *(int *)(c + 0x24);
            *(int *)(c + 0xa4) = *(int *)(c + 0x28);

            switch (*(int *)(c + 0x2c)) {
            case 0:  func_ov006_020c63dc(c); break;
            case 1:  func_ov006_020c6348((int *)c); break;
            case 2:  func_ov006_020c6248((int *)c); break;
            case 3:  func_ov006_020c6188(c); break;
            case 4:  func_ov006_020c5ec8(c); break;
            case 5:  func_ov006_020c5cf4((int *)c); break;
            case 6:  func_ov006_020c5aa4((struct Obj *)c); break;
            case 7:  func_ov006_020c57d4((struct Obj *)c); break;
            case 8:  func_ov006_020c561c(c); break;
            case 9:  func_ov006_020c54f4(c); break;
            case 10: func_ov006_020c5370(c); break;
            default: func_ov006_020c63dc(c); break;
            }

            func_ov006_020e6e3c(0x1c5, *(int *)(c + 0x9c));
        }
    }

    _Z15ApproachLinear2Rsss((short *)(c + 0xec), 0, 1);

    if (*(short *)(c + 0xec) == 1) {
        func_ov006_020e6e3c(0x1c5, *(int *)(c + 0x9c));
    }

    if (LenVec3((Vec3 *)(c + 0xa8)) < 0x1000) {
        func_ov006_020c49d8(c);
    } else {
        func_ov006_020c47d4(c);
    }
}
