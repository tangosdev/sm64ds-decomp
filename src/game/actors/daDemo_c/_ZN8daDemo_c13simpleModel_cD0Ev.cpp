//cpp
// @symbol _ZN8daDemo_c13simpleModel_cD0Ev
/* D0, the deleting destructor. Same class shape as the D1 file beside this one;
 * one destructor definition emits D0/D1/D2 and objisolate keeps the variant this
 * file's delinks entry names. The class operator delete is what routes the tail
 * call to Memory::operator_delete2 (0x0203cbcc) rather than the global _ZdlPv --
 * without it the bytes still match and only the relocation destination differs. */
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
