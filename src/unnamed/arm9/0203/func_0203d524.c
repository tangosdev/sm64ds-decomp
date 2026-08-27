struct V2 { int x; int y; };
int func_0203d524(struct V2* a, struct V2* b) {
    return (int)(((long long)a->x * b->y + 0x800) >> 12)
         - (int)(((long long)a->y * b->x + 0x800) >> 12);
}
