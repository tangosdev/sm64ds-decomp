struct Vec3 { int x, y, z; };
struct Elem {
    struct Vec3** a;   /* 0x0 */
    struct Vec3** b;   /* 0x4 */
    int len;           /* 0x8 */
    struct Vec3 v;     /* 0xc */
};

extern void SubVec3(struct Vec3* a, struct Vec3* b, struct Vec3* c);
extern int LenVec3(struct Vec3* v);

void func_ov007_020c6314(char* t, int idx, int flag)
{
    int i;
    struct Elem* e;

    switch (flag) {
    case 0:
        for (i = 0; i < *(unsigned short*)((char*)(*(struct Elem***)(t + 0x38))[idx] + 8); i++) {
            e = (*(struct Elem****)(t + 0x34))[idx][i];
            SubVec3((struct Vec3*)(t + 0x7c), &(*(struct Vec3***)(t + 0x2c))[idx][i], &e->v);
            e->len = LenVec3(&e->v);
        }
        break;
    case 1:
        for (i = 0; i < *(unsigned short*)((char*)(*(struct Elem***)(t + 0x38))[idx] + 8); i++) {
            e = (*(struct Elem****)(t + 0x34))[idx][i];
            SubVec3(*e->a, *e->b, &e->v);
            e->len = LenVec3(&e->v);
        }
        break;
    case 2:
        for (i = 0; i < *(unsigned short*)((char*)(*(struct Elem***)(t + 0x38))[idx] + 8); i++) {
            e = (*(struct Elem****)(t + 0x34))[idx][i];
            SubVec3((struct Vec3*)(t + 0x7c), &(*(struct Vec3***)(t + 0x28))[idx][i], &e->v);
            e->len = LenVec3(&e->v);
        }
        break;
    }
}
