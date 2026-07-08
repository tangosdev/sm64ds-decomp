typedef unsigned short u16;

u16 func_02061960(void);

typedef struct {
    char pad[4];
    u16 f4;
} Obj;

int func_0206a29c(Obj *self) {
    u16 mask;
    u16 v;
    u16 i;
    u16 cur;
    mask = func_02061960();
    if (mask == 0) return 0;
    cur = self->f4;
    i = 0;
    v = cur;
    do {
        if (mask & (1 << (v - 1))) {
            if (cur != v) {
                self->f4 = v;
                break;
            }
        }
        i = i + 1;
        v = (v == 0x10) ? 1 : v + 1;
    } while (i < 0x10);
    return 1;
}
