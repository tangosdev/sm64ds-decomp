#include "types.h"
struct Node {
    struct Node* next;
    u32 lo;
    u32 hi;
};

extern struct Node* data_020aa3f0;

enum Flag { F0, F1 };

unsigned int _ZN3IRQ7DisableEv(void);
void _ZN3IRQ7RestoreEj(unsigned int);
void func_0206cf98(int a, u32 b, u32 c);

void func_0205dc7c(u32* q) {
    struct Node* prev;
    struct Node* collected;
    struct Node* tail;
    u32 r6, r5, r4;
    u32 saved;
    struct Node* head;
    struct Node* node;
    struct Node* nx;
    struct Node** hp = &data_020aa3f0;

    for (;;) {
        collected = 0;
        r6 = q[1];
        r5 = q[1] + q[2];
        r4 = r5 + (u32)((int)(q[2] + q[3]) - q[2]);
        tail = 0;
        saved = _ZN3IRQ7DisableEv();
        prev = 0;
        head = *hp;
        node = head;
        while (node) {
            u32 lo, hi;
            lo = node->lo;
            nx = node->next;
            hi = node->hi;
            if ((lo >= r6 && lo < r5) ||
                (hi >= r5 && hi < r4)) {
                if (tail) tail->next = node; else collected = node;
                if (head == node) { *hp = nx; head = nx; }
                node->next = 0;
                tail = node;
                if (prev) prev->next = nx;
            } else {
                prev = node;
            }
            node = nx;
        }
        _ZN3IRQ7RestoreEj(saved);
        if (collected == 0) break;
        do {
            void (*cb)(u32);
            cb = (void(*)(u32))collected->lo;
            nx = collected->next;
            if (cb) cb(collected->hi);
            collected = nx;
        } while (collected);
    }
    {
        u32 v = q[8];
        u32 b = q[2];
        u32 a1 = q[1];
        enum Flag flag = (v == 0) ? F0 : F1;
        func_0206cf98(flag, a1, b);
    }
}
