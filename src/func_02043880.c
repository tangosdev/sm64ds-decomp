#include "types.h"
enum { false, true };
typedef struct Node {
    struct Node* prev;
    struct Node* next;
    void* owner;
    u16 c;
    u16 e;
} Node;

typedef struct LNode {
    char pad[0xc];
    struct LNode* next;
    void* obj;
} LNode;

typedef struct Obj {
    char pad0[0xe];
    u8 state;
    u8 dirty;
    u8 f10;
    u8 f11;
    u8 pad12;
    u8 flags;
    int pad14;
    LNode* list;
    char pad1c[0xc];
    Node n28;
    Node n38;
} Obj;

extern int func_0203b27c(void* l, void* n);
extern int func_0203b20c(void* l, void* n);
extern int func_0203b244(void* l, void* n);
extern int func_0204405c(void* l, void* n);
extern void _ZN9ActorBase18MarkForDestructionEv(void* self);
extern Obj* func_02043810(Obj* p);
extern int data_020a4b78;
extern int data_020a4b98;
extern int data_020a4b88;
extern int data_020a4ba8;

#define FLAGP(o) ((u8*)((char*)(o) + 0x13))
#define NODEP(o, OFF) ((Node*)((char*)(o) + (OFF)))


#pragma opt_common_subs off
int func_02043880(Obj* o) {
    if (o->dirty != 0) {
        LNode* it;
        int b = 0;
        o->dirty = b;
        if (o->state == 1) b = 1;
        if (b != 0) {
            func_0203b27c(&data_020a4b78, &o->n28);
            func_0203b27c(&data_020a4b98, &o->n38);
        } else {
            func_0203b27c(&data_020a4b88, &o->n28);
        }
        func_0203b20c(&data_020a4ba8, &o->n28);
        o->state = 2;
        for (it = o->list; it != 0; it = it->next) {
            _ZN9ActorBase18MarkForDestructionEv(it->obj);
        }
    } else {
        u32 state;
        int b;
        Obj* other = func_02043810(o);
        if (other != 0) {
            {
                u8 f = other->flags;
                int t = f & 1;
                if (t == 0) {
                    t = f & 2;
                    if (t == 0) goto Lelse1;
                }
                {
                    u8* q = FLAGP(o);
                    *q |= 2;
                }
                goto Lafter1;
            Lelse1:
                if (o->flags & 2) {
                    u8* q = FLAGP(o);
                    *q &= ~2;
                }
            Lafter1:;
            }
            {
                u8 f = other->flags;
                int t = f & 4;
                if (t == 0) {
                    t = f & 8;
                    if (t == 0) goto Lelse2;
                }
                {
                    u8* q = FLAGP(o);
                    *q |= 8;
                }
                goto Lafter2;
            Lelse2:
                if (o->flags & 8) {
                    u8* q = FLAGP(o);
                    *q &= ~8;
                }
            Lafter2:;
            }
        }
        state = o->state;
        b = (int)((state == 1) ? (char*)1 : (char*)0);
        if (b != false) {
            {
                Node* n = NODEP(o, 0x28);
                int c = (int)((n->e != n->c) ? (char*)1 : (char*)0);
                if (c != false) {
                    Node* m;
                    func_0203b27c(&data_020a4b78, &o->n28);
                    m = NODEP(o, 0x28);
                    m->c = m->e;
                    func_0204405c(&data_020a4b78, m);
                }
            }
            {
                Node* n = NODEP(o, 0x38);
                int c = (int)((n->e != n->c) ? (char*)1 : (char*)0);
                if (c != false) {
                    Node* m;
                    func_0203b27c(&data_020a4b98, &o->n38);
                    m = NODEP(o, 0x38);
                    m->c = m->e;
                    func_0204405c(&data_020a4b98, m);
                }
            }
        } else {
            int b2 = (int)((state == 2) ? (char*)1 : (char*)0);
            if (b2 == false) {
                if (o->f11 != 0) {
                    o->f11 = 0;
                    func_0203b244(&data_020a4b88, &o->n28);
                } else if (o->f10 != 0) {
                    o->f10 = 0;
                    func_0204405c(&data_020a4b78, &o->n28);
                    func_0204405c(&data_020a4b98, &o->n38);
                    o->state = 1;
                }
            }
        }
    }
    return 1;
}
