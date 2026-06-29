/* func_02049f58 at 0x02049f58
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
typedef unsigned short u16;
typedef unsigned int u32;

extern void func_0204a730(void* o, void* n);
extern int func_0204d8e8(void* p, void* n);
extern void func_0204d9a0(void* p, int v);

typedef struct Sub {
    u32 b_lo : 14;
    u32 b14 : 1;        /* bit 14 */
    u32 b_hi : 17;
    char pad0[0x24];
    u16 f28;            /* 0x28 */
} Sub;

typedef struct Node {
    struct Node* next;  /* 0x0 */
    char pad0[0x8];
    int fc;             /* 0xc */
    char pad1[0x4];
    int f14;            /* 0x14 */
    Sub** f18;          /* 0x18 */
    u32 b0 : 1;         /* 0x1c bit 0 */
    u32 b1 : 1;
    u32 b2 : 1;         /* bit 2 */
    u32 b_rest : 29;
    char pad2[0x18];
    u16 f38;            /* 0x38 */
} Node;

typedef struct Obj {
    int f0;
    Node* f4;           /* 0x4 */
    char pad0[0x4];
    int fc;             /* 0xc */
} Obj;

void func_02049f58(Obj* o)
{
    Node* n = o->f4;
    Node* next;
    if (n == 0) return;
    do {
        Sub* s = *n->f18;
        next = n->next;
        if (n->b2 == 0) {
            func_0204a730(o, n);
        }
        if ((s->b14 != 0 && s->f28 != 0 && n->f38 > s->f28) || n->b0 != 0) {
            if (n->fc == 0 && n->f14 == 0) {
                int r = func_0204d8e8(&o->f4, n);
                func_0204d9a0(&o->fc, r);
            }
        }
        n = next;
    } while (next != 0);
}
