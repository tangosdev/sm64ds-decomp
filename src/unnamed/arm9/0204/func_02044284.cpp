//cpp
#pragma opt_common_subs off
typedef short s16;

struct Obj {
    virtual void v00();
    virtual void v04();
    virtual int v08(void *p);

    s16 state;    // 0x4
    s16 idx;      // 0x6
    void **arr;   // 0x8
    int f0c;      // 0xc
    s16 f10;      // 0x10
    s16 f12;      // 0x12
};

extern "C" int func_02044284(Obj *self, int flag)
{
    void **arr = self->arr;
    s16 i;

    if (arr == 0 || self->state == 2)
        return 2;

    i = self->idx;
    while (arr[i] != 0) {
        self->state = self->v08(arr[i]);
        if (self->state != 2)
            break;
        self->idx++;
        self->f12 = 0;
        if (flag == 0)
            break;
        i = self->idx;
        arr = self->arr;
    }
    return self->state;
}
