#include "types.h"
struct Elem {
    int first;  /* 0x0 */
    int x4;     /* 0x4 */
    int x8;     /* 0x8 */
};

struct Obj {
    char pad[0x1c];
    int count;          /* 0x1c */
    struct Elem arr[1];  /* 0x20 */
};

struct Elem* func_020523e0(struct Obj* obj, int idx) {
    struct Elem* e;
    if (idx < 0) return 0;
    if ((u32)idx >= (u32)obj->count) return 0;
    e = &obj->arr[idx];
    if (e->first == -1) return 0;
    return e;
}
