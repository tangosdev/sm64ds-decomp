#include "Ov007OamNode.h"
#include "Ov007MenuMem.h"

extern void func_ov007_020c1048(Ov007OamList *list);

/* Builds an empty OAM node list. */
Ov007OamList *func_ov007_020c1068(void) {
    Ov007OamList *list = func_ov007_020c3df4(0, sizeof(Ov007OamList));
    func_ov007_020c1048(list);
    return list;
}

/* Frees an OAM node list. Long-branch veneer: long_calls emits the pooled
 * `ldr ip,[pc]; bx ip` absolute tail-call; bracketed because the pragma is
 * positional. */
#pragma long_calls on
void func_ov007_020c105c(void *p) {
    func_ov007_020c3d1c(p);
}
#pragma long_calls off

/* Empties an OAM node list. */
void func_ov007_020c1048(Ov007OamList *list) {
    list->tail = 0;
    list->head = list->tail;
}
