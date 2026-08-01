#include "types.h"
struct Actor { char pad[0xc]; unsigned short kind; };
extern struct Actor* _ZN5Actor10FindWithIDEj(u32 id);
struct Actor* func_ov018_021118fc(char* c) {
    struct Actor* r4 = 0;
    if (*(u32*)(c + 0x194) & 0x8000000) {
        struct Actor* a = _ZN5Actor10FindWithIDEj(*(u32*)(c + 0x198));
        if (a) {
            int ok = (a->kind == 0xbf) ? 1 : (int)r4;
            if (ok) {
                if (a != *(struct Actor**)(c + 0x378))
                    r4 = a;
                *(struct Actor**)(c + 0x378) = a;
            }
        }
    } else {
        *(struct Actor**)(c + 0x378) = r4;
    }
    return r4;
}
