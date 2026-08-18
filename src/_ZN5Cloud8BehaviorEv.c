// @symbol _ZN5Cloud8BehaviorEv
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Cloud.h"
extern void* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int, void*);
extern int _ZN5Model12SetPolygonIDEi(void*, int);
extern int Vec3_Dist(void* a, void* b);

#pragma opt_propagation off
int _ZN5Cloud8BehaviorEv(char *c) {
    int d = Vec3_Dist(c+0x74, c+0x5c);
    int rank = 1;
    int base = 2;
    void *a = _ZN8dActor_c15FindWithActorIDEjPS_(0x13a, 0);
    while (a) {
        if (a != (void*)c) {
            int d2 = Vec3_Dist(c+0x74, (char*)a+0x5c);
            if (d > d2) rank++;
        }
        a = _ZN8dActor_c15FindWithActorIDEjPS_(0x13a, a);
    }
    _ZN5Model12SetPolygonIDEi(c+0xd4, (base + rank) & 0xff);
    return 1;
}
