#include "types.h"
struct dActor_c { char pad[0xc]; unsigned short kind; };
extern struct dActor_c* _ZN8dActor_c10FindWithIDEj(u32 id);
struct dActor_c* func_ov018_021118fc(char* c) {
    struct dActor_c* r4 = 0;
    if (*(u32*)(c + 0x194) & 0x8000000) {
        struct dActor_c* a = _ZN8dActor_c10FindWithIDEj(*(u32*)(c + 0x198));
        if (a) {
            int ok = (a->kind == 0xbf) ? 1 : (int)r4;
            if (ok) {
                if (a != *(struct dActor_c**)(c + 0x378))
                    r4 = a;
                *(struct dActor_c**)(c + 0x378) = a;
            }
        }
    } else {
        *(struct dActor_c**)(c + 0x378) = r4;
    }
    return r4;
}
