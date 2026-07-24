typedef unsigned char u8;
typedef unsigned int u32;

struct Vec3 { int x, y, z; };

typedef struct {
    void *obj;
    int dist;
} SoundSlot;

extern u32 data_02099fac;
extern int data_02099fb0;
extern SoundSlot data_020a4bf8[2];
extern SoundSlot data_020a4c18[];

extern void *func_02050cdc(int kind, int id);
extern int func_02049018(struct Vec3 *v);
extern int func_0204fa2c(int *p, int a);
extern void func_0204f934(int **p);

void *func_02048720(struct Vec3 *v, int kind, int id)
{
    u32 lim;
    void *elem;
    u8 typ;
    u32 pri;
    int i;
    u32 dist;
    u32 p;
    SoundSlot *slot;
    u32 best_pri;
    u32 best_dist;
    int best_idx;
    int pick;
    int count;
    SoundSlot *table;

    lim = data_02099fac;
    best_idx = -1;
    pick = best_idx;
    best_pri = 0xff;
    best_dist = 0;

    elem = func_02050cdc(kind, id);
    if (elem == 0) {
        return 0;
    }

    typ = ((u8 *)elem)[5];
    pri = ((u8 *)elem)[4];

    switch (typ) {
    case 2:
        count = 2;
        table = data_020a4bf8;
        break;
    case 9:
        count = data_02099fb0;
        table = data_020a4c18;
        break;
    default:
        return 0;
    }

    dist = (u32)func_02049018(v);

    if (id >= 0x4b && id <= 0x4c) {
        lim = 0x384;
    }
    if (dist > lim) {
        return 0;
    }

    for (i = 0; i < count; i++) {
        if (table[i].obj == 0)
            pick = i;
    }

    if (pick < 0) {
        for (i = 0; i < count; i++) {
            p = *((u8 *)table[i].obj + 0x3d);
            if (best_pri > p) {
                best_pri = p;
                best_idx = i;
                best_dist = (u32)table[i].dist;
            } else if (best_pri == p) {
                if (best_dist < (u32)table[i].dist) {
                    best_dist = (u32)table[i].dist;
                    best_idx = i;
                }
            }
        }

        if (pri > best_pri) {
            pick = best_idx;
        } else if (pri == best_pri && dist <= best_dist) {
            pick = best_idx;
        }
    }

    if (pick < 0) {
        return 0;
    }
    if (pick >= 0 && pick < count) {
        func_0204fa2c((int *)&table[pick], 1);
        func_0204f934((int **)&table[pick]);
        table[pick].dist = (int)dist;
        return &table[pick];
    }
    return 0;
}
