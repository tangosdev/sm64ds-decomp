//cpp
extern "C" void func_ov007_020bfe4c(void *o, int x, int y, int z, void *out);

/* Array form on purpose: a three-int-member struct copies field by field,
   while the ROM copies these with one ldm/stm. */
struct Vec3 {
    int w[3];
};

struct Group {
    char pad[8];
    unsigned short count; /* 0x8 */
    char pad2[0x24 - 0xa];
    int *f24; /* 0x24 */
    int *f28; /* 0x28 */
};

struct ObjA8 {
    char pad[0x2c];
    int f2c; /* 0x2c */
};

struct Picker {
    char pad0[8];
    int groupCount; /* 0x8 */
    char pad1[0x28 - 0xc];
    Vec3 **groupPoints; /* 0x28 */
    Vec3 **a2c;         /* 0x2c */
    char pad2[0x38 - 0x30];
    Group **groups; /* 0x38 */
    char pad3[0x58 - 0x3c];
    int hitGroup;   /* 0x58: picked group, -1 = none (see func_ov007_020c5a6c) */
    int hitIndex;   /* 0x5c: picked point in that group, -1 = none */
    Vec3 hitOffset; /* 0x60 */
    char pad4[0x70 - 0x6c];
    Vec3 pos; /* 0x70 */
    Vec3 f7c; /* 0x7c */
    char pad5[0xa8 - 0x88];
    ObjA8 *a8; /* 0xa8 */
};

extern "C" void func_ov007_020c5dec(Picker *c) {
    int i;
    for (i = 0; i < c->groupCount; i++) {
        int j;
        for (j = 0; j < c->groups[i]->count; j++) {
            ObjA8 *o = c->a8;
            Group *group = c->groups[i];
            func_ov007_020bfe4c(o, group->f24[j], group->f28[j], -o->f2c, &c->a2c[i][j]);
            c->groupPoints[i][j] = c->a2c[i][j];
        }
    }
    c->pos = c->f7c;
    c->hitGroup = c->hitIndex = -1;
    c->hitOffset.w[0] = c->hitOffset.w[1] = c->hitOffset.w[2] = 0;
}
