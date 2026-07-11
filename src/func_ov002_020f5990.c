// NONMATCHING: div=5 — arg shift-chain register crossing: ROM keeps (f0<<4)>>16 in-place in r1
// and (f4<<4)>>16 in r2; mwcc colors rawA=r2/rawB=r1 and spills the lsl through ip. Same residual
// under BOTH C and C++ frontends (C PTMF dispatch recipe byte-matches the member-pointer call block).
// Shapes tried: inline args, named temps, split statements, compound <<=/>>=, u64-identity launders,
// array subscript spelling, volatile on either load, assignment-expr-in-arg. Wall family 67(a).
#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))
typedef int s32;
typedef short s16;
typedef unsigned char u8;

extern void func_ov002_020f39ec(char* c, int i);
extern void func_ov002_020f2790(char* c, int p1, int p2, int p3, s16 p4);

typedef void (*MF)(char*, int);
struct PM { int fn; int dv; };
extern struct PM data_ov002_02110e7c[];

struct Sub {
    int f0;
    int f4;
    char pad8[0x28 - 8];
    int f28;
    s16 f2c;
    char pad2e[0x44 - 0x2e];
    u8 f44;
    u8 pad45;
    u8 f46;
    char pad47[0x49 - 0x47];
    u8 f49;
};

void func_ov002_020f5990(char* c)
{
    int i;
    struct Sub* s = (struct Sub*)c;
    {
        int* cnt = (int*)AT(c, 0x1f8);
        *cnt = *cnt + 1;
        *cnt = *cnt & 3;
    }
    for (i = 0; i < 4; i++, s = (struct Sub*)((char*)s + 0x4c)) {
        if (s->f44 == 0) continue;
        {
            struct PM* e = &data_ov002_02110e7c[s->f46];
            int dv = e->dv;
            char* obj = c + (dv >> 1);
            MF fp;
            if (dv & 1) fp = *(MF*)(*(char**)obj + e->fn);
            else fp = (MF)e->fn;
            fp(obj, i);
        }
        func_ov002_020f39ec(c, i);
        if (s->f49 == 0) continue;
        if (i != *(int*)(c + 0x1f8)) continue;
        func_ov002_020f2790(c, (s->f0 << 4) >> 0x10, (s->f4 << 4) >> 0x10, s->f28, s->f2c);
    }
}
