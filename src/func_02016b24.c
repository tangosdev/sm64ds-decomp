typedef unsigned int u32;

struct Obj {
    char pad[8];
    char* p8;
    char* pc;
};

void func_02016b24(struct Obj* o, u32 mask)
{
    u32 count = *(u32*)(o->p8 + 0x24);
    char* p = o->pc;
    u32 i;
    for (i = 0; i < count; i++) {
        *(u32*)(((int)p + 0x24)) |= mask;
        p += 0x30;
    }
}
