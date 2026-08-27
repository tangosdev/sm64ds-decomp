typedef struct V3 { int x, y, z; } V3;

int func_0203d024(V3* a, V3* b){
    if (a->x != b->x || a->y != b->y || a->z != b->z) return 1;
    return 0;
}
