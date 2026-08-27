extern void func_ov004_020b2444(int a1, int a2, int num, int a4, int a5, int sel, int idx);

typedef struct {
    int x;
    int y;
    unsigned char p0[2];
    unsigned short num;
    unsigned char p1;
    unsigned char flag;
    unsigned char p2[2];
} Elem;

typedef struct {
    unsigned char _pad[0x5014];
    Elem arr[24];
} Obj;

void func_ov006_020dc870(Obj *o)
{
    int i;
    if (*(int *)((char *)o + 0x51c8) == 5) {
        if (*(int *)((char *)o + 0x51cc) == 0)
            return;
    }
    for (i = 0; i < 0x18; i++) {
        if (o->arr[i].flag != 0) {
            func_ov004_020b2444(o->arr[i].x >> 12, o->arr[i].y >> 12, o->arr[i].num, 0, -1, 0, 0);
        }
    }
}
