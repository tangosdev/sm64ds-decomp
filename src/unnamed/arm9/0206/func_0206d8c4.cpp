//cpp
#include "cstd.h"

extern "C" {

#pragma optimize_for_size on /* built for size like func_0206d32c: the loop enters at its test */
extern const char data_020868fc;
extern const char data_02086a08;

struct Elem {
    /* 16 bytes, only ever copied whole. An array member keeps the C++ struct
     * copy the ldm/stm block move C emitted (notes/cpp-class-form.md). */
    int w[4];
};

struct Arr {
    unsigned int count;
    int pad[3];
    struct Elem elems[1];
};

void func_0206d8c4(struct Arr *s, unsigned int index) {
    unsigned int n;
    if (index >= s->count) {
        cstd::__assert(&data_020868fc, (const char *)0x55, &data_02086a08, 1);
    }
    s->count--;
    for (n = s->count; index < n; index++)
        s->elems[index] = s->elems[index + 1];
}
}
