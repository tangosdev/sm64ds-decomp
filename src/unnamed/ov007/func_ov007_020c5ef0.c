typedef struct {
    int x, y, z;
} Vec3;

/* A point; +0 is its position (func_ov007_020c5a6c), the six words after it
 * are cleared together here and in func_ov007_020c5948 but have no reader
 * that names them. */
struct Obj {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
};

/* A point list as func_ov007_020c798c builds it. */
struct Ov007PointList {
    struct Obj **items; /* 0x0 */
    int count;          /* 0x4 */
};

/* The 0xd0-byte touch picker func_ov007_020c690c builds (func_ov007_020b9f0c
 * passes it); names as in that file and func_ov007_020c684c, only the slots
 * this function touches. The tag stays O: three declarations below keep
 * their banked struct O * spelling (the declaration-agreement gate). */
typedef struct O {
    char pad0[8];
    int groupCount; /* 0x8 */
    char pad_c[0x30];
    struct Ov007PointList **pointLists; /* 0x3c: per group (func_ov007_020c798c) */
    char pad_40[0x48];
    Vec3 posOffset; /* 0x88: func_ov007_020c5188 adds it to pos for one step */
} Ov007Picker;

extern void func_ov007_020c6af0(Ov007Picker *c);
extern void func_ov007_020c6b58(struct O *c);
extern void func_ov007_020c64c4(struct O *c, int b);
extern void func_ov007_020c62cc(struct O *c, int b);

void func_ov007_020c5ef0(Ov007Picker *c) {
    int i, j;
    func_ov007_020c6af0(c);
    func_ov007_020c6b58(c);
    func_ov007_020c64c4(c, 0);
    func_ov007_020c62cc(c, 1);
    for (i = 0; i < c->groupCount; i++) {
        struct Ov007PointList *in = c->pointLists[i];
        for (j = 0; j < in->count; j++) {
            struct Obj *o = in->items[j];
            o->f18 = 0;
            o->f14 = o->f18;
            o->f10 = o->f14;
            o->fc = o->f10;
            o->f8 = o->fc;
            o->f4 = o->f8;
        }
    }
    c->posOffset.z = 0;
    c->posOffset.y = c->posOffset.z;
    c->posOffset.x = c->posOffset.y;
}
