//cpp
// @symbol _ZN8daDemo_c13simpleModel_cD1Ev
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
#include "Model.h"
#include "SharedFilePtr.h"

extern "C" void _ZN6Memory16operator_delete2EPv(void *);

struct ScaleHolder {
    Vector3 mScale[1];          /* 0x50 */
};

struct daDemo_c {
    struct simpleModel_c : Model, virtual ScaleHolder {
        virtual ~simpleModel_c();
        void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }
    };
};

daDemo_c::simpleModel_c::~simpleModel_c()
{
    SharedFilePtr *file = *(SharedFilePtr **)((char *)this + 0x5c);
    if (file != 0) {
        file->Release();
    }
}
