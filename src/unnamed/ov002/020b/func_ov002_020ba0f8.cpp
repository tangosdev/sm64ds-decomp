//cpp
// @symbol func_ov002_020ba0f8
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"


extern "C" void func_ov002_020ba01c(void* c, int a, int b, int d, int e);
extern "C" {
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int n, struct Vector3* v);
extern void _ZN5Event8ClearBitEj(unsigned int n);
}
extern "C" void func_ov002_020ba4d8(void* c, int i);

extern "C" void func_ov002_020ba0f8(char* c) {
    func_ov002_020ba01c(c, 2, 3, 0x333, 0x1000);
    if (*(unsigned short*)(c + 0x338) != 3) return;
    _ZN5Sound9PlayBank3EjRK7Vector3(0x3e, (struct Vector3*)(c + 0x74));
    {
        int b = (*(unsigned short*)(c + 0xc) == 0xc);
        if (b) {
            char* p = *(char**)(c + 0x348);
            if (p != 0 && *(int*)(p + 0x438) == 0) {
                func_ov002_020e7104(p, 0);
            }
            *(unsigned char*)(c + 0xcc) = *(signed char*)(c + 0x353);
        } else {
            _ZN5Event8ClearBitEj(*(unsigned char*)(c + 0x34e));
        }
    }
    func_ov002_020ba4d8(c, 0);
}
