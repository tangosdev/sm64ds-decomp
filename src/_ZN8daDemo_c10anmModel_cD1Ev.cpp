//cpp
// @symbol _ZN8daDemo_c10anmModel_cD1Ev
/* D1, the complete-object destructor.
 *
 * The shape here is the whole finding. The cartridge destroys the ModelAnim /
 * Model BASE before the Vector3 array at the class's own offset -- and a normal
 * C++ destructor cannot do that: members are always destroyed before bases. A
 * VIRTUAL base can, because D1 runs members, then non-virtual bases, then
 * virtual bases. So the Vector3 lives in a virtual base, and that reproduces the
 * ROM's order exactly.
 *
 * The base holds only the array. The remaining fields are read at raw offsets
 * rather than declared, because a declared member would sit BEFORE the virtual
 * base in the layout and push the array off 0x50/0x64. */
#include "ModelAnim.h"
#include "SharedFilePtr.h"

extern "C" void _ZN6Memory16operator_delete2EPv(void *);

typedef void (*VFN)(void *);

struct ScaleHolder {
    Vector3 mScale[1];          /* 0x64 */
};

struct daDemo_c {
    struct anmModel_c : ModelAnim, virtual ScaleHolder {
        virtual ~anmModel_c();
        void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }
    };
};

daDemo_c::anmModel_c::~anmModel_c()
{
    char *c = (char *)this;
    void *p;
    int i;

    p = *(void **)(c + 0x70);
    if (p != 0) ((SharedFilePtr *)(p))->Release();
    for (i = 0; i < *(unsigned char *)(c + 0x80); i++) {
        p = (*(void ***)(c + 0x74))[i];
        if (p != 0) ((SharedFilePtr *)(p))->Release();
    }
    if (*(void **)(c + 0x7c) != 0) {
        for (i = 0; i < *(unsigned char *)(c + 0x81); i++) {
            p = (*(void ***)(c + 0x78))[i];
            if (p != 0) ((SharedFilePtr *)(p))->Release();
        }
        p = *(void **)(c + 0x7c);
        if (p != 0) {
            (*(VFN)((*(int **)p)[1]))(p);
        }
    }
}
