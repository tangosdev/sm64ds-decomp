#include "types.h"
struct HeapAllocator;
struct NestedHeapIterator;

extern char data_020a4d60[];
extern s16 data_02086384[];

unsigned int func_0205b608(void);
char* _ZN18NestedHeapIterator4NextEP13HeapAllocator(char* self, char* a);
int func_0205af78(unsigned int x);
void func_0204f2d4(char* p);
int func_02052438(int* p);
int func_02052458(int* p);
void func_0205ad3c(unsigned int a, int b);
int func_02052420(int* o);
void func_0204f3e0(unsigned char* c);

void func_0204fafc(void) {
    char* node;
    char* next;
    unsigned int mask;
    int v;

    mask = func_0205b608();
    node = _ZN18NestedHeapIterator4NextEP13HeapAllocator(data_020a4d60, 0);
    if (node == 0) {
        return;
    }
    do {
        next = _ZN18NestedHeapIterator4NextEP13HeapAllocator(data_020a4d60, node);
        if (*(u8*)(node + 0x2d) == 0) {
            if (func_0205af78(*(u32*)(node + 0x30))) {
                *(u8*)(node + 0x2d) = 1;
            }
        }
        if (*(u8*)(node + 0x2d) != 0 && (mask & (1 << *(u8*)(node + 0x3c))) == 0) {
            func_0204f2d4(node);
        } else {
            int t1, t2, t3;
            func_02052438((int*)(node + 0x1c));
            t1 = data_02086384[*(u8*)(node + 0x41)];
            t2 = data_02086384[*(u8*)(node + 0x40)];
            t3 = data_02086384[func_02052458((int*)(node + 0x1c)) >> 8];
            v = t3 + (t2 + t1);
            if (v < -0x2d3) {
                v = -0x2d3;
            } else if (v > 0) {
                v = 0;
            }
            if (v != *(s16*)(node + 0x3e)) {
                func_0205ad3c(*(u8*)(node + 0x3c), v);
                *(s16*)(node + 0x3e) = v;
            }
            if (*(u8*)(node + 0x2c) == 2) {
                if (func_02052420((int*)(node + 0x1c))) {
                    func_0204f3e0((unsigned char*)node);
                }
            }
        }
        node = next;
    } while (next != 0);
}
