extern void func_ov007_020c7d60(void *obj, int r6, int r5);
extern void func_ov007_020c7c60(void *obj, int r6, int r5);
extern void func_ov007_020c7b2c(void *obj, int r6, int r5);
extern void func_ov007_020c7a84(void *obj, int r6, int r5);

struct S {
    int unk0;
    int count;
    void **list;
    int flags;
    int mode;
};

void func_ov007_020c7560(struct S *self, int r6, int r5)
{
    int n;
    int i;
    if (self->count <= 1) {
        n = 0;
    } else {
        n = self->count - ((self->flags & 2) ? 0 : 1);
    }
    switch (self->mode) {
    case 0:
        for (i = 0; i < n; i++)
            func_ov007_020c7d60(self->list[i], r6, r5);
        break;
    case 1:
        for (i = 0; i < n; i++)
            func_ov007_020c7c60(self->list[i], r6, r5);
        break;
    case 2:
        for (i = 0; i < n; i++)
            func_ov007_020c7b2c(self->list[i], r6, r5);
        break;
    case 3:
        for (i = 0; i < n; i++)
            func_ov007_020c7a84(self->list[i], r6, r5);
        break;
    }
}
