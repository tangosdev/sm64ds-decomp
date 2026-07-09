/* func_0204a5c8 at 0x0204a5c8, size 0x168
 * Matched byte-for-byte with mwccarm 1.2/sp2p3
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern void func_0204af3c(void* p);
extern void func_0204af38(void* p);
extern void func_0204b244(void* self, void* node);
extern void func_0204be40(void* self, void* node);
extern void func_0204b81c(void* self, void* node);
extern void func_0204b028(void* self, void* node);
extern void func_0204bbd8(void* self, void* node);
extern void func_0204b460(void* self, void* node);

typedef struct Node1 {
    u32 pad0 : 4;
    u32 kind : 2;
    u32 pad1 : 5;
    u32 flag11 : 1;
    u32 pad2 : 4;
    u32 flag16 : 1;
    u32 pad3 : 15;
    char pad4[0x33 - 4];
    u8 idx33;
} Node1;

typedef struct LNode {
    struct LNode* next;
    char pad[0x42 - 4];
    u8 idx42;
} LNode;

typedef struct SubTarget {
    u16 pad0 : 7;
    u16 kind2 : 2;
    u16 pad1 : 7;
    char pad2[0xf - 2];
    u8 idxF;
} SubTarget;

typedef struct Sub1 {
    Node1* f0;
    char pad[0x14 - 4];
    SubTarget* f14;
} Sub1;

typedef struct Ctx {
    char pad0[8];
    LNode* f8;
    char pad1[0x10 - 0xc];
    LNode* f10;
    char pad2[0x18 - 0x14];
    Sub1* f18;
} Ctx;

void func_0204a5c8(char* self)
{
    Ctx* ctx = *(Ctx**)(self + 0x34);
    Sub1* sub1 = ctx->f18;
    LNode* n;
    Node1* node1 = sub1->f0;
    void (*fn1)(void*, void*) = 0;
    void (*fn2)(void*);

    func_0204af3c(*(char**)(self + 0x20) + node1->idx33 * 0x14);

    switch (node1->kind) {
    case 0: fn1 = func_0204be40; break;
    case 1: fn1 = func_0204b81c; break;
    case 2: fn1 = func_0204b244; break;
    }

    fn2 = node1->flag11 ? func_0204af3c : func_0204af38;

    n = ctx->f8;
    if (n != 0) {
        do {
            fn2(*(char**)(self + 0x20) + n->idx42 * 0x14);
            fn1(self, n);
            n = n->next;
        } while (n != 0);
    }

    if (!node1->flag16) return;

    func_0204af3c(*(char**)(self + 0x20) + sub1->f14->idxF * 0x14);

    switch (sub1->f14->kind2) {
    case 0: fn1 = func_0204bbd8; break;
    case 1: fn1 = func_0204b460; break;
    case 2: fn1 = func_0204b028; break;
    }

    n = ctx->f10;
    if (n == 0) return;
    do {
        fn1(self, n);
        n = n->next;
    } while (n != 0);
}
