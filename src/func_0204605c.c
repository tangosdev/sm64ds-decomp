struct Element { char pad[0x24]; unsigned int flags; char pad2[0x30 - 0x24 - 4]; };
struct Obj { void *pad0; struct Element *arr; };

void func_0204605c(struct Obj *self, unsigned int value, int index)
{
    struct Element *e = &self->arr[index];
    unsigned int *flags = (unsigned int *)(((long long)(int)&e->flags) & 0xFFFFFFFFFFFFFFFFLL);
    *flags &= ~0x3f000000;
    *flags |= value << 24;
}
