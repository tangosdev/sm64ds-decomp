typedef unsigned char u8;

typedef struct Src {
    char* f0;
    int f4;
    int f8;
    char pad[0xc];
    char* f18;
} Src;

void func_02071ccc(Src* s, int off, int add)
{
    char* base = s->f18;
    char* dst = base + off;
    *(int*)dst = s->f4;
    *(char**)(dst + 4) = s->f0;
    *(int*)(dst + 8) = s->f8;
    if (*(u8*)(s->f0) != 0x2a) {
        *(int*)(dst + 0xc) = s->f4 + add;
        return;
    }
    *(char**)(dst + 0xc) = dst + 0x10;
    *(int*)(dst + 0x10) = *(int*)(s->f4) + add;
}
