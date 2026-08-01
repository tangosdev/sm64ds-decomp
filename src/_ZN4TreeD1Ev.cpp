//cpp
// @symbol _ZN4TreeD1Ev
/* recovered: named members + shared header */
#include "Tree.h"
extern "C" {
extern int __destroy_arr(void*, int, int, void*);
extern void _ZN5ActorD2Ev(void*);
extern void *_ZTV4Tree;
extern int _ZN5ModelD1Ev();
int _ZN4TreeD1Ev(char* c){
    *(void**)c = (void*)&_ZTV4Tree;
    __destroy_arr((char*)c+0xd4, 5, 0x50, (void*)&_ZN5ModelD1Ev);
    _ZN5ActorD2Ev(c);
    return (int)c;
}
}
