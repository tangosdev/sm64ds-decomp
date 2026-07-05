struct Element { char pad[0x2c]; unsigned int flags; char pad2[0x30 - 0x2c - 4]; };
struct Obj { void *pad0; struct Element *arr; };

void func_02045ec4(struct Obj *self, unsigned int value, int index)
{
    struct Element *e = &self->arr[index];
    unsigned int *flags = (unsigned int *)(((long long)(int)&e->flags) & 0xFFFFFFFFFFFFFFFFLL);
    *flags &= 0xffff;
    *flags |= value << 16;
}
