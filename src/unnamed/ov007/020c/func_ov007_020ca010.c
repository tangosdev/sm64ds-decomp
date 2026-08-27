typedef struct Vec3 {
    int x;
    int y;
    int z;
} Vec3;

struct Obj;

struct Node {
    Vec3 *pos;
    int pad[7];
    int flags;
};

struct Thing {
    struct Node **nodes;
};

struct Param {
    int count;
    void *p04;
    int pad[3];
    int p14;
};

struct Mgr {
    int unk00;
    struct Param *param;
    int unk08;
    Vec3 v0c;
    Vec3 *buf18;
    struct Thing *t1c;
    Vec3 arrA[2];
    Vec3 arrB[2];
    void *p50[2];
    void *p58[2];
    void *p60[2];
    void *p68[2];
};

extern int func_ov007_020c3df4(int a, void* b);
extern struct Thing * func_ov007_020c798c(void *p0, int p1, void *p2, int p3);
extern void func_ov007_020c7804(struct Obj* self);
extern void* func_ov007_020c844c(int p, int q);
extern void* func_ov007_020c80a4(void);

struct Mgr *func_ov007_020ca010(struct Param *param)
{
    struct Mgr *obj;
    int n;
    int i;
    int j;
    struct Thing *thing;

    obj = (struct Mgr *)func_ov007_020c3df4(0, (void *)0x70);
    obj->unk00 = 0;
    obj->param = param;
    n = param->count;
    obj->v0c = (Vec3){0, 0, 0};
    obj->buf18 = (Vec3 *)func_ov007_020c3df4(0, (void *)(n * 12));
    obj->t1c = func_ov007_020c798c((void *)2, param->count, param->p04, 1);
    thing = obj->t1c;
    for (i = 0; i < param->count; i++) {
        Vec3 v = {0, 0, 0};
        v.z = i << 12;
        obj->buf18[i] = v;
        thing->nodes[i]->pos = &obj->buf18[i];
    }
    func_ov007_020c7804((struct Obj *)thing);
    thing->nodes[0]->flags |= 1;
    for (j = 0; j < 2; j++) {
        {
            Vec3 a = {0, 0, 0};
            a.x = j << 12;
            obj->arrA[j] = a;
        }
        {
            Vec3 b = {0, 0, 0};
            b.y = j << 12;
            obj->arrB[j] = b;
        }
        obj->p50[j] = func_ov007_020c844c((int)&obj->arrA[j], param->p14);
        obj->p58[j] = func_ov007_020c844c((int)&obj->arrB[j], param->p14);
        obj->p60[j] = func_ov007_020c80a4();
        obj->p68[j] = func_ov007_020c80a4();
    }
    return obj;
}
