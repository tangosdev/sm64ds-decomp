//cpp
#include "Heap.h"

extern "C" {

extern void func_02059788(void);
extern void func_02059bc0(void);
extern void func_02059578(int a, int b);
extern void func_0201a054(void);
extern void func_020197b8(void);

extern int func_0201a03c;
extern int data_020a621c;
extern mHeap::Heap_t *data_020a0e9c;

void main(void) {
    func_02059788();
    func_02059bc0();
    func_02059578((int)&func_0201a03c, (int)&data_020a621c);
    data_020a0e9c->SetNodeID(2);
    func_0201a054();
    data_020a0e9c->SetNodeID(3);
    func_020197b8();
}
}
